
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
isRealData=False
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
BSRDOInput=["/castor/cern.ch/user/a/acerri/ATNTests/daq.csc13.0000000.Single.Stream.LB0000.Athena._0101.data_10evts"]
include('TriggerRelease/runHLT_standalone.py')
