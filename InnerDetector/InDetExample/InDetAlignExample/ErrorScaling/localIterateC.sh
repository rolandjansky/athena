#
# starting from nominal error scaling
#./iterateTrkError.py -c -n10 jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
#
# starting from error scaling from third A iteration
#./iterateTrkError.py -c -n10 -pparam_iterA03.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py >! outTest.log

export STAGE_SVCCLASS=atlcal

rm outTest.log

./iterateTrkError.py -c -s1 -n2 -k jobOptionsErrorTuning.py jobOptionsAlignmentSet.py | tee outTest.log
#./iterateTrkError.py -c -k -s1 -n2 --param=highpTMuon2_A_2/param_iterA11.py jobOptionsErrorTuning.py jobOptionsAlignmentSet.py > outTest.log
