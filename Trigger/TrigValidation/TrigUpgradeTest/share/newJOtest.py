

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ConfigFlags import ConfigFlagContainer    
from AthenaConfiguration.CfgLogMsg import cfgLogMsg
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq
from AthenaCommon.AlgSequence import dumpMasterSequence
from AthenaCommon.AppMgr import theApp
#theApp.setup()

cfgLogMsg.setLevel("debug")
flags = ConfigFlagContainer()

flags.set("AthenaConfiguration.GlobalFlags.isMC",False)
flags.set("AthenaConfiguration.GlobalFlags.InputFiles",
          ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"])

acc = ComponentAccumulator()
from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
acc.executeModule( TrigBSReadCfg, flags )

from AtlasGeoModel.GeoModelConfig import GeoModelCfg
acc.executeModule( GeoModelCfg, flags )

# HLT top CF, will be able to add it once MR 8533 is there
# This commented could should stay 
# acc.addSequence( seqOR( "hltTop") )
# acc.addSequence( seqAND("hltSteps" ), sequence="hltTop" )
# for step in range(1, 6):
#     acc.addSequence( parOR("hltStep%d" %s ), sequence="hltSteps" )

# for now we run trivial tester, 
from AthenaCommon.Constants import DEBUG
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderMTTest
acc.addEventAlgo( ROBDataProviderMTTest("Tester", OutputLevel=DEBUG) )


# that is how the L1 decoder can be added but it needs more work to bring all needed services (i.e. TrigConfiguration)
# from L1Decoder.L1DecoderMod import L1DecoderMod
# acc.executeModule( L1DecoderMod, flags )
# l1 = acc.getEventAlgo( "L1Decoder" )
# from TrigUpgradeTest.TestUtils import applyMenu
# applyMenu( l1 )

#acc.printConfig() waiting for MR 8533 is there

with file("newJOtest.pkl", "w") as p:
    acc.store( p )
    p.close()

