
# define slices to run...
doAllTeIDSCAN                        =True
doAllTeSiTrack                       =True
doAllTeTRTxK                         =True
doAllTeTRTxK_TRTTrkHypo              =True
doAllTeTRTxK_newAlgo                 =True
doAllTeTRTxK_TRTTrkHypo_newAlgo      =True
doAllTeTRTxK_TRTTrkHypo_newAlgo_Pixel=True

# other settings...
doM5L1Menu=True
#generateOnlineMenu=False
testCosmic=True
isOnline=False
doM4Data=True
doM3Data=True
isRealData=True
doM4L1Menu=True
doEnableCosmicMuonAccessInEF=True
doSerialization=True
T2CaloFakeLVL1InLVL2=True
doDBConfig=False
OutputLevel=INFO
HLTOutputLevel=DEBUG
forceLVL2Accept=False
FakeLVL1=False


# define input data
BSRDOInput=["/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data"]
include('TriggerRelease/runHLT_standalone.py')
