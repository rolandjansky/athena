#
# starting from nominal error scaling
#./iterateTrkError.py -c -n10 jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
#
# starting from error scaling from third A iteration
#./iterateTrkError.py -c -n10 -pparam_iterA03.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py >! outTest.log

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'

#cd /afs/cern.ch/user/c/cerio/scratch1/17.1.2.1
cd $RELAREA
#asetup 17.1.2.1
asetup $REL

export STAGE_SVCCLASS=atlcal

#cd /afs/cern.ch/user/a/atlidali/scratch1/users/bcerio/17.1.2.1/InnerDetector/InDetExample/InDetAlignExample/ErrorScaling
#cd /afs/cern.ch/user/c/cerio/scratch1/17.1.2.1/InnerDetector/InDetExample/InDetAlignExample/ErrorScaling
cd $ERSCPATH
rm outTest.log

#./iterateTrkError.py -c --batch -s1 -n6  ReadInDet_jobOptions.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
#./iterateTrkError.py -c -k --batch -s1 -n10 jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
./iterateTrkError.py -c -k --batch -s1 -n10 jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
#./iterateTrkError.py -c -k --batch -s1 -n10 -d 0.35 --param=highpTMuon2_A_2/param_iterA11.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log

#./iterateTrkError.py -c --batch -s1 -n9 -i10 --param=param_iterC09.py ReadInDet_jobOptions.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
