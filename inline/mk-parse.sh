#!/bin/sh

EXT=/home/bl24/include

CXX=g++
CXXFLAGS='-g -O -std=c++11'

prog=parse

if test "x$1" != x ; then
    prog="$1"
fi

src="${prog}.cpp"

if test ! -f "$src" ; then
    echo "no such file: $src"
    echo "usage ./mk-parse <progname>"
    exit 1
fi

set --  \
    $CXXFLAGS  \
    "$src"  -o  "$prog"   \
    -I$EXT/boost/include  \
    -I$EXT/symtabAPI/include  \
    -L$EXT/symtabAPI/lib  \
    -lparseAPI  -linstructionAPI  -lsymtabAPI  \
    -ldynDwarf  -ldynElf  -lcommon  \
    -L$EXT/libelf/lib  -lelf  \
    -L$EXT/libdwarf/lib  -ldwarf 

echo $CXX $@
echo

$CXX $@

