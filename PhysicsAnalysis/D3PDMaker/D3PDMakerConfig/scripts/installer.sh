#!/bin/sh

curdir=`pwd`
cd $TestArea

#D3PDMakers
#cmt co PhysicsAnalysis/D3PDMaker/JetD3PDMaker
#cmt co PhysicsAnalysis/D3PDMaker/MissingETD3PDMaker
#cmt co -r MissingETD3PDMaker-00-10-18 PhysicsAnalysis/D3PDMaker/MissingETD3PDMaker #
#cmt co -r JetD3PDAnalysis-00-01-06 PhysicsAnalysis/D3PDMaker/JetD3PDAnalysis
#cmt co -r D3PDMakerTest-00-07-42-20 PhysicsAnalysis/D3PDMaker/D3PDMakerTest

#Jet
#cmt co -r JetRec-01-02-68-05 Reconstruction/Jet/JetRec #
#cmt co -r JetRec-01-02-68-branch Reconstruction/Jet/JetRec #
#cmt co -r JetMomentTools-00-00-97-09 Reconstruction/Jet/JetMomentTools #
#cmt co -r JetUtils-01-01-64 Reconstruction/Jet/JetUtils
#cmt co -r JetInterface-00-00-13 Reconstruction/Jet/JetInterface

#MisingET
#cmt co -r MissingET-03-03-55 Reconstruction/MissingET #
#cmt co Reconstruction/MissingET #
#cmt co -r MissingETEvent-00-04-23 Reconstruction/MissingETEvent #

${curdir}/mySetupWorkArea.py
cd WorkArea/cmt
cmt br cmt config
cmt br source setup.sh
cmt br gmake -j 4 quiet=1
cd ${curdir}
