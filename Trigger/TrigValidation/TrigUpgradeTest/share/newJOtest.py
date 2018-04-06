#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.CfgLogMsg import cfgLogMsg
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq
from AthenaCommon.AlgSequence import dumpMasterSequence
from AthenaCommon.AppMgr import theApp
#theApp.setup()

cfgLogMsg.setLevel("debug")
flags = ConfigFlags

flags.set( "global.isMC", False )
flags.set( "global.InputFiles",
           ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] )

flags.set( "Trigger.inputLVL1ConfigFile", "LVL1config_Physics_pp_v7.xml" )
flags.set( "Trigger.L1Decoder.doMuon", False )

flags.lock()

acc = ComponentAccumulator()
from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
acc.addConfig( TrigBSReadCfg, flags )

from AtlasGeoModel.GeoModelConfig import GeoModelCfg
acc.addConfig( GeoModelCfg, flags )





# that is how the L1 decoder can be added but it needs more work to bring all needed services (i.e. muon rois decoding)
acc.addSequence( seqOR( "hltTop") )
from L1Decoder.L1DecoderConfig import L1DecoderCfg
acc.addConfig( L1DecoderCfg, flags, sequence="hltTop" )
l1 = acc.getEventAlgo( "L1Decoder" )
from TrigUpgradeTest.TestUtils import applyMenu
applyMenu( l1 )


# adding calo requires  more infrastructure than we actually have
#from TrigUpgradeTest.EgammaCaloMod import EgammaCaloMod
#acc.addConfig( EgammaCaloMod, flags, sequence="hltStep1" )

acc.printConfig()

fname = "newJOtest.pkl"
print "Storing config in the config", fname
with file(fname, "w") as p:
    acc.store( p )
    p.close()
