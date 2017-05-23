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

# include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
# svcMgr.ByteStreamInputSvc.FullFileName = [ "./input.data" ]

# # This is the list of proxies to set up so that retrieval attempt will trigger the BS conversion
# if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
#     from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
#     svcMgr += ByteStreamAddressProviderSvc()
# svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RoIBResult/RoIBResult" ]

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()


# from SGComps.SGCompsConf import SGInputLoader
# topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
# topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','RoIBResult') ]




data = {'noreco': [';', ';', ';']}  # in the lists there are the events

data['tracks'] = ['eta:1,phi:1,pt:18000; eta:1,phi:-1.2,pt:3500;',
                      'eta:0.5,phi:0,pt:12000; eta:1,phi:-1.2,pt:6500;',
                      'eta:-0.6,phi:1.7,pt:9000;']

data['ctp'] = [ 'HLT_g100',  'HLT_2g50 HLT_e20', 'HLT_mu20 HLT_mu8 HLT_2mu8 HLT_mu8_e8' ]

data['l1emroi'] = ['1,1,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.2,0,EM3,EM7',
                   '-0.6,0.2,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM50',
                   '-0.6,1.5,0,EM3,EM7,EM7']

data['l1muroi'] = ['0,0,0,MU0;',
                   '-1,0.5,0,MU6,MU8; -1,0.5,0,MU6,MU8,MU10',
                   '-1.5,-0.1,0,MU6,MU8']


# EOF input generation        
from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)

include("TrigUpgradetest/L1CF.py")
include("TrigUpgradetest/HLTCF.py")




muChains  = [ 'HLT_mu20', 'HLT_mu8', 'HLT_2mu8' ]
eChains   = [ 'HLT_e20' ]
gChains   = [ 'HLT_g100', 'HLT_2g50' ]
mueChains = [ 'HLT_mu8_e8' ]

steps = [ parOR("step0Filtering"), parOR("step1Tracking") ]

stepNo = 0
steps[stepNo] += seqFilter( "Step0MU", Inputs=["L1MU"], Outputs=["step0MU"], Chains=muChains )
steps[stepNo] += seqFilter( "Step0MU_E", Inputs=["L1MU", "L1EM"], Outputs=["step0MUE_MU","step0MUE_EM"], Chains=mueChains )
steps[stepNo] += seqFilter( "Step0EM", Inputs=["L1EM"], Outputs=["step0EM"], Chains=(eChains + gChains)  )



trkInputMerger = merger("trkInputMerger", Inputs=["step0MU", "step0MUE_MU", "step0MUE_E", "step0EM"], Output="TrackingRoIs")
allRoITrk = reco("allRoITrk", Output="TrigTracks", FileName="tracks.dat", Input="TrackingRoIs") # we do nto care how many tracks are made, just want see if right RoIs arrive at the input of this reco

# the seq of merger & tracking shoudl be enabled by any of the filter
stepNo += 1

steps[stepNo] += stepSeq("emtrk",  useExisting( "Step0EM" ),   [ trkInputMerger, allRoITrk ] )
steps[stepNo] += stepSeq("mutrk",  useExisting( "Step0MU" ),   [ trkInputMerger, allRoITrk ] )
steps[stepNo] += stepSeq("muetrk", useExisting( "Step0MU_E" ), [ trkInputMerger, allRoITrk ] )

TopHLTSeq += addSteps(steps)

theApp.EvtMax = 3




