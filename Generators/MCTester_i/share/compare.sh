#!/bin/bash   
# Written by N. Davidson for "Analysis" step of MC-TESTER

FILE1=myFileName1.root
FILE2=myFileName2.root
PLATFORM="slc4_ia32_gcc34" #change to slc4_amd64_gcc34 as needed"

#location of ROOT macrocs for booklet creation
MCTESTER_VERSION=1.23.1
MCTESTER_DIR=/afs/cern.ch/sw/lcg/external/MCGenerators/mctester/${MCTESTER_VERSION}
MCTESTER_ANALYZE_DIR=${MCTESTER_DIR}/share/analyze

export MC_TESTER_LIBS_DIR=${MCTESTER_DIR}/${PLATFORM}/lib

WORKING_DIR=`pwd`

#change to MCTester directory and run macros
cd $MCTESTER_ANALYZE_DIR 
root -b -q "rootlogon.C" "ANALYZE.C(\"${WORKING_DIR}\",\"${WORKING_DIR}/${FILE1}\",\"${WORKING_DIR}/${FILE2}\")" 
root -b -q "rootlogon.C" "BOOKLET.C(\"${WORKING_DIR}\")"
cd $WORKING_DIR

#copy base .tex file needed for booklet and create
cp ${MCTESTER_ANALYZE_DIR}/tester.tex ./ 
latex tester.tex
latex tester.tex 
latex tester.tex 
dvipdf tester

#do a bit of clean up
rm -rf tester.aux tester.log texput.log tester.toc 
rm -rf mc-results.aux booklet.aux 
