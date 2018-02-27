
doL1Unpacking=False
include("testHLT_MT.py")


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ConfigFlags import ConfigFlagContainer    
from AthenaConfiguration.CfgLogMsg import cfgLogMsg
cfgLogMsg.setLevel("debug")
flags = ConfigFlagContainer()
acc = ComponentAccumulator()
from L1Decoder.L1DecoderMod import L1DecoderMod

acc.addConfig( L1DecoderMod, flags )
l1 = acc.getEventAlgo( "L1Decoder" )

from TrigUpgradeTest.TestUtils import applyMenu
applyMenu( l1 )
l1.OutputLevel = DEBUG
for u in l1.roiUnpackers:
    u.OutputLevel = DEBUG


