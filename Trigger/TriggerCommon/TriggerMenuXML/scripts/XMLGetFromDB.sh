#!/bin/bash
#../scripts/XMLDumperFromDB.sh 369 388 526 Cosmic_v1_FieldOff
#ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/triggerDB triggerDB

#export DISPLAY=""

#All files taken from MC DB (L1 only hence no HLT ps given)
#../scripts/XMLDumperFromDB.sh 34 19 lumi1E31_14.5.0 
#../scripts/XMLDumperFromDB.sh 35 20 lumi1E31_15.0.0 
../scripts/XMLDumperFromDB.sh 36 21 Physics_lumi1E31_15.4.0 
../scripts/XMLDumperFromDB.sh 37 22 Physics_lumi1E31_15.5.0 
../scripts/XMLDumperFromDB.sh 46 31 Physics_lumi1E31_simpleL1Calib_15.5.2
../scripts/XMLDumperFromDB.sh 135 74 Physics_HI_v1_15.6.9
