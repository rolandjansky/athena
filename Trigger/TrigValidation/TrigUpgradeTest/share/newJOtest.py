#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ConfigFlags import ConfigFlagContainer    
from AthenaConfiguration.CfgLogMsg import cfgLogMsg
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq
from AthenaCommon.AlgSequence import dumpMasterSequence
from AthenaCommon.AppMgr import theApp
#theApp.setup()

cfgLogMsg.setLevel("debug")
flags = ConfigFlagContainer()

flags.set( "AthenaConfiguration.GlobalFlags.isMC", False )
flags.set( "AthenaConfiguration.GlobalFlags.InputFiles",
           ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] )

flags.set( "TrigConfigSvc.Flags.inputLVL1configFile", "LVL1config_Physics_pp_v7.xml" )
flags.set( "L1Decoder.Flags.doMuon", False )

acc = ComponentAccumulator()
from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
acc.executeModule( TrigBSReadCfg, flags )

from AtlasGeoModel.GeoModelConfig import GeoModelCfg
acc.executeModule( GeoModelCfg, flags )





# that is how the L1 decoder can be added but it needs more work to bring all needed services (i.e. muon rois decoding)
acc.addSequence( seqOR( "hltTop") )
from L1Decoder.L1DecoderMod import L1DecoderMod
acc.executeModule( L1DecoderMod, flags, sequence="hltTop" )
l1 = acc.getEventAlgo( "L1Decoder" )
from TrigUpgradeTest.TestUtils import applyMenu
applyMenu( l1 )

# here the menu starts
acc.addSequence( seqAND( "hltSteps"), sequence="hltTop" )
for step in range(1, 6):
    acc.addSequence( parOR( "hltStep%d" % step), sequence="hltSteps" )

# adding calo requires  more infrastructure than we actually have
#from TrigUpgradeTest.EgammaCaloMod import EgammaCaloMod
#acc.addConfig( EgammaCaloMod, flags, sequence="hltStep1" )

acc.printConfig()

fname = "newJOtest.pkl"
print "Storing config in the config", fname
with file(fname, "w") as p:
    acc.store( p )
    p.close()
