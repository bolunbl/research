//
//  Template for using ParseAPI to iterate over functions and basic
//  blocks to display the line map and inline call sequence for every
//  machine instruction.
//

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <iostream>
#include <ostream>
#include <string>

#include <CFG.h>
#include <CodeObject.h>
#include <CodeSource.h>
#include <Function.h>
#include <Symtab.h>
#include <Instruction.h>
#include <LineInformation.h>

using namespace Dyninst;
using namespace ParseAPI;
using namespace SymtabAPI;
using namespace InstructionAPI;
using namespace std;

Symtab * the_symtab = NULL;
CodeObject * the_code_obj = NULL;

#define INDENT  "   "

//----------------------------------------------------------------------

void
doInlineSeqn(Offset addr)
{
    FunctionBase *func, *parent;

    if (the_symtab->getContainingInlinedFunction(addr, func) && func != NULL)
    {
	parent = func->getInlinedParent();
	while (parent != NULL) {
	    //
	    // func is inlined iff it has a parent
	    //
	    InlinedFunction *ifunc = static_cast <InlinedFunction *> (func);
	    pair <string, Offset> callsite = ifunc->getCallsite();

	    string filenm = callsite.first;
	    long lineno = callsite.second;
	    string pname = func->getName();

	    cout << INDENT << INDENT << "inline:  l=" << lineno
		 << "  f='" << filenm
		 << "'  p='" << pname << "'\n";

	    func = parent;
	    parent = func->getInlinedParent();
	}
    }
}

//----------------------------------------------------------------------

void
doInstruction(Offset addr)
{
    string file = "";
    long line = 0;

    vector <Statement *> svec;

    the_symtab->getSourceLines(svec, addr);

    if (! svec.empty()) {
	file = svec[0]->getFile();
	line = svec[0]->getLine();
    }

    cout << INDENT << "stmt:  0x" << hex << addr << dec
	 << "  l=" << line
	 << "  f='" << file << "'\n";

    doInlineSeqn(addr);
}

//----------------------------------------------------------------------

void
doBlock(Block * block)
{
    cout << "\nblock:\n";

    map <Offset, InstructionAPI::Instruction::Ptr> imap;

    block->getInsns(imap);

    for (auto iit = imap.begin(); iit != imap.end(); ++iit)
    {
	Offset addr = iit->first;
	doInstruction(addr);
    }
}

//----------------------------------------------------------------------

void
doFunction(ParseAPI::Function * func)
{
    cout << "\n------------------------------------------------------------\n"
	 << "func:  0x" << hex << func->addr() << dec
	 << "  '" << func->name() << "'\n";

    const ParseAPI::Function::blocklist & blist = func->blocks();

    for (auto bit = blist.begin(); bit != blist.end(); ++bit)
    {
	Block * block = *bit;
	doBlock(block);
    }
}

//----------------------------------------------------------------------

int
main(int argc, char **argv)
{
    if (argc < 2) {
        errx(1, "missing file name");
    }
    char *filename = argv[1];

    if (! Symtab::openFile(the_symtab, filename)) {
	errx(1, "Symtab::openFile failed: %s", filename);
    }
    the_symtab->parseTypesNow();

    SymtabCodeSource * code_src = new SymtabCodeSource(the_symtab);
    the_code_obj = new CodeObject(code_src);

    the_code_obj->parse();

    // iterate over function list

    const CodeObject::funclist & funcList = the_code_obj->funcs();

    for (auto fit = funcList.begin(); fit != funcList.end(); ++fit)
    {
	ParseAPI::Function * func = *fit;
	doFunction(func);
    }

    return 0;
}
