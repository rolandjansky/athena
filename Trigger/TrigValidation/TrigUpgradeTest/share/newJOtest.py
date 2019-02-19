#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq, findAlgorithm, findOwningSequence
from AthenaCommon.AlgSequence import dumpMasterSequence
from AthenaCommon.AppMgr import theApp

from TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO import setupMenu

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1


flags.needFlagsCategory('Trigger')
setupMenu(flags)


flags.Input.isMC = False
flags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] 

flags.Trigger.LVL1ConfigFile = "LVL1config_Physics_pp_v7.xml" 
flags.Trigger.L1Decoder.forceEnableAllChains = True


flags.lock()

from AthenaCommon.Constants import INFO,DEBUG,WARNING
acc = ComponentAccumulator()

from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
acc.merge(TrigBSReadCfg(flags ))


from TrigUpgradeTest.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig(flags ))


from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu
from TriggerJobOpts.TriggerConfig import triggerRunCfg
acc.merge( triggerRunCfg( flags, generateMenu ) )

from RegionSelector.RegSelConfig import RegSelConfig
rsc, regSel = RegSelConfig( flags )
regSel.enableCalo=True
regSel.enableID=True
regSel.enablePixel = True
regSel.enableSCT = True
regSel.enableTRT = True
acc.merge( rsc )
acc.addService(regSel)

from TrigUpgradeTest.InDetConfig import TrigInDetCondConfig
acc.merge( TrigInDetCondConfig( flags ) )

acc.getEventAlgo( "TrigSignatureMoniMT" ).OutputLevel=DEBUG
print acc.getEventAlgo( "TrigSignatureMoniMT" )

topSequence = acc.getSequence("HLTTop")
muonCacheCreatorName = "MuonCacheCreator"
mcc = findAlgorithm( topSequence, muonCacheCreatorName )
if mcc:
    owner = findOwningSequence( topSequence, muonCacheCreatorName )
    owner.remove( mcc )
    top = acc.getSequence("AthAlgSeq")
    top += mcc


from AthenaConfiguration.ComponentAccumulator import forcomps
from AthenaCommon.Logging import logging
logging.getLogger('forcomps').setLevel(DEBUG)
forcomps(acc, "*/L1Decoder").OutputLevel = DEBUG
forcomps(acc, "*/L1Decoder/*Tool").OutputLevel = DEBUG # tools
forcomps(acc, "*HLTTop/*Hypo*").OutputLevel = DEBUG # hypo algs
forcomps(acc, "*HLTTop/*Hypo*/*Tool*").OutputLevel = DEBUG # hypo tools
forcomps(acc, "*HLTTop/RoRSeqFilter/*").OutputLevel = DEBUG # filters
forcomps(acc, "*HLTTop/*Input*").OutputLevel = DEBUG # input makers
forcomps(acc, "*HLTTop/*GenericMonitoringTool*").OutputLevel = WARNING # silcence mon tools (addressing by type)


# # from TrigUpgradeTest.TestUtils import applyMenu
# # applyMenu( acc.getEventAlgo( "L1Decoder" ) )
# #acc.getEventAlgo( "L1Decoder" ).OutputLevel=DEBUG
# #acc.getEventAlgo( "L2ElectronCaloHypo" ).OutputLevel=DEBUG
# #acc.getEventAlgo( "FastEMCaloAlgo" ).OutputLevel=DEBUG
# #acc.getEventAlgo( "Filter_for_L2PhotonCaloHypo" ).OutputLevel=DEBUG

acc.printConfig()


fname = "newJOtest.pkl"
print "Storing config in the config", fname
with file(fname, "w") as p:
    acc.store( p )
    p.close()




