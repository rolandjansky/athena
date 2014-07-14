#!/bin/sh
curdir=`pwd`
cd $TestArea

${curdir}/mySetupWorkArea.py
cd WorkArea/cmt
cmt br cmt config
cmt br source setup.sh
cmt br gmake -j 4 quiet=1
cd ${curdir}
