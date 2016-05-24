# these jobOptions are intended for AthAnalysisBase releases
# and work in a similar way to ut_validate_run1_bs_to_xaod.cxx
# of the RootCore Analysis releases: i.e. the input file is either
# the output of Run1BStoxAOD (if it's successful) or a 
# fixed reference file from the public afs area (if Run1BStoxAOD is not successful)

theApp.EvtMax=100   

import datetime
# datetime is 0 for Monday and 6 for Sunday. Need to transform to ATLAS' 0 for Sunday, 6 for Saturday
today = (datetime.datetime.today().weekday() + 1) % 7 
print "today is ",today
yesterday = (today + 6) % 7
s_nightly_aod_yesterday = "/afs/cern.ch/atlas/software/builds/nightlies/20.7.X-VAL/AtlasAnalysis/rel_%s/NICOS_area/NICOS_atntest207XVAL64BS6G49AnlOpt/triganalysistest_testconfiguration_work/TrigHLT_Run1BStoxAOD/myAOD.pool.root" % yesterday
print "the path for AOD from yesterday's ATN is: ", s_nightly_aod_yesterday
s_ref_file = 'root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data12_xAODmadeWithAtlasAnalysis207XVAL151124_forATNAnalysisBase.root'
print "check if the ATN output file exists..."
s_file = "";

import os
if not (os.path.isfile(s_nightly_aod_yesterday)):
    print "WARNING: it seems that the yesterday's ATN TrigHLT_Run1BStoxAOD in AtlasAnalysis,20.7.X-VAL was unsuccessful, going to use input file from the eos space"
    s_file = s_ref_file
else:
    s_file = s_nightly_aod_yesterday

# if this is AthAnalysisBase,2.3, it won't work with the root file prepared in current 20.7.X-VAL due to different xAOD schema version
rc_release = os.environ['ROOTCORE_RELEASE_SERIES']
if int(rc_release) == 23:
    print "This is ROOTCORE_RELEASE_SERIES = 23, will use the corresponding input file"
    s_file = s_ref_file

print s_file," will be used as the input for this test"

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [s_file]

###############################
algseq = CfgMgr.AthSequencer("AthAlgSeq")  
algseq += CfgMgr.AthenaTestHarness("Run1BStoxAODTrigger")

