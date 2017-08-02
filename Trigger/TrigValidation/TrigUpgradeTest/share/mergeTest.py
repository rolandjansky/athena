###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Configure the scheduler
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.ShowDataFlow=True
svcMgr.ForwardSchedulerSvc.ShowControlFlow=True

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()


# idea for the input is to try various ways of passing RoIs to reco
# event 1
# two EM RoI, one filterd out, one reaching reco
#
# event 2
# two EM RoIs passed by EM filter
#
# event 3
# EM & MU RoIs passed by E+MU chain
#
# event 4
# EM & MU RoIs passed independently because of e and MU chains
# for RoIs identification the RoI word is "used" and flags event number (starting from 0)

data = {'noreco': [';', ';', ';', ';']}  # in the lists there are the events

data['tracks'] = ['eta:1,phi:1,pt:18000; eta:1,phi:-1.2,pt:3500;',
                  'eta:0.5,phi:0,pt:12000; eta:1,phi:-1.2,pt:6500;',
                  'eta:-0.6,phi:1.7,pt:9000;',
                  'eta:-0.6,phi:1.7,pt:9000;' ]

data['ctp'] = [ 'HLT_g100',
                'HLT_2g50 HLT_e20',
                'HLT_mu8_e8',
                'HLT_g100 HLT_mu8']

data['l1emroi'] = ['1,1,0,EM3,EM7,EM15,EM20,EM50,EM100; -2,-2,0,EM3,EM7', # the second should not survive
                   '-0.6,0.2,1,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,1,EM3,EM7,EM15,EM20,EM50',
                   '-0.6,1.5,2,EM3,EM7',
                   '-1,-1,3,EM3,EM7,EM15,EM20,EM50,EM100;']

data['l1muroi'] = ['0,0,0,MU0;',
                   '-1,0.5,1,MU6,MU8; -1,0.5,1,MU6,MU8,MU10',
                   '-1.5,-0.1,2,MU6,MU8',
                   '1,0.5,3,MU6,MU8']


# EOF input generation        
from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)

include("TrigUpgradeTest/L1CF.py")
include("TrigUpgradeTest/HLTCF.py")




muChains  = [ 'HLT_mu20', 'HLT_mu8', 'HLT_2mu8' ]
eChains   = [ 'HLT_e20' ]
gChains   = [ 'HLT_g100', 'HLT_2g50' ]
mueChains = [ 'HLT_mu8_e8' ]

steps = [ parOR("step0Filtering"), parOR("step1Tracking") ]

stepNo = 0
steps[stepNo] += seqFilter( "Step0MU", Inputs=["L1MU"], Outputs=["step0MU"], Chains=muChains )
steps[stepNo] += seqFilter( "Step0MU_E", Inputs=["L1MU", "L1EM"], Outputs=["step0MUE_MU","step0MUE_EM"], Chains=mueChains )
steps[stepNo] += seqFilter( "Step0EM", Inputs=["L1EM"], Outputs=["step0EM"], Chains=(eChains + gChains)  )



trkInputMerger = merger("trkInputMerger", Inputs=["step0MU", "step0MUE_MU", "step0MUE_EM", "step0EM"], Output="TrackingRoIs")
allRoITrk = TestRecoAlg("allRoITrk", Output="TrigTracks", FileName="noreco.dat", Input="TrackingRoIs") # we do not care how many tracks are made, just want see if right RoIs arrive at the input of this reco

# the seq of merger & tracking shoudl be enabled by any of the filter
stepNo += 1

from AthenaCommon.CFElements import stepSeq

steps[stepNo] += stepSeq("emtrk",  useExisting( "Step0EM" ),   [ trkInputMerger, allRoITrk ] )
steps[stepNo] += stepSeq("mutrk",  useExisting( "Step0MU" ),   [ trkInputMerger, allRoITrk ] )
steps[stepNo] += stepSeq("muetrk", useExisting( "Step0MU_E" ), [ trkInputMerger, allRoITrk ] )

TopHLTSeq += addSteps(steps)

theApp.EvtMax = 4




