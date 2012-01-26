
# define slices to run...
doAllTeMBSpacePoints          =True
doAllTeMBStandaloneSpacePoints=True

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
FakeLVL1=True


# define input data
BSRDOInput=["/castor/cern.ch/user/a/acerri/ATNTests/daq.csc13.0000000.Single.Stream.LB0000.Athena._0101.data_10evts"]
include('TriggerRelease/runHLT_standalone.py')
