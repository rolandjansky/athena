#
# starting from nominal error scaling
#./iterateTrkError.py -c -n10 jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
#
# starting from error scaling from third A iteration
#./iterateTrkError.py -c -n10 -pparam_iterA03.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py >! outTest.log


#source ~/w1/users/amorley/cmthome/setup.sh -tag=16.0.2.5

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'

#cd /afs/cern.ch/user/a/atlidali/scratch1/users/bcerio/17.1.2.1
cd $RELAREA

asetup $REL
export STAGE_SVCCLASS=atlcal

#cd /afs/cern.ch/user/a/atlidali/w0/users/amorley/testarea/17.0.1.2/ErrorScalingOld/run/
#cd /afs/cern.ch/user/a/atlidali/w0/users/bcerio/testarea/17.0.3/InnerDetector/InDetExample/InDetAlignExample/ErrorScaling
#cd /afs/cern.ch/user/a/atlidali/scratch1/users/bcerio/17.1.2.1/InnerDetector/InDetExample/InDetAlignExample/ErrorScaling
cd $ERSCPATH
rm outTest.log

#./iterateTrkError.py -c --batch -s1 -n6  ReadInDet_jobOptions.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
./iterateTrkError.py -a -k --batch -s1 -n10 jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log

#./iterateTrkError.py -c --batch -s1 -n9 -i10 --param=param_iterC09.py ReadInDet_jobOptions.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
