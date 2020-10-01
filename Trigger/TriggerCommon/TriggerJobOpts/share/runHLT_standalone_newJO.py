#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1



flags.Detector.GeometryPixel = True
flags.Detector.GeometrySCT   = True
flags.Detector.GeometryTRT   = True
flags.Detector.GeometryID    = True
flags.Detector.GeometryBpipe = True
flags.Detector.GeometryCavern = False
flags.Detector.GeometryPixel = True
flags.Detector.GeometrySCT   = True
flags.Detector.GeometryTRT   = True

flags.Detector.GeometryLAr   = True
flags.Detector.GeometryTile  = True
flags.Detector.GeometryMDT   = True
flags.Detector.GeometryTGC   = True
flags.Detector.GeometryCSC   = True
flags.Detector.GeometryRPC   = True


flags.Detector.RecoPixel = True
flags.Detector.RecoSCT   = True


# Output configuration - currently testing offline workflow
flags.Trigger.writeBS = False
flags.Output.doWriteRDO = True
flags.Output.RDOFileName = 'RDO_TRIG.pool.root'

flags.Trigger.CostMonitoring.doCostMonitoring = True
flags.Scheduler.CheckDependencies = True
flags.Scheduler.ShowDataDeps = True
flags.Scheduler.ShowDataFlow = True
flags.Scheduler.ShowControlFlow = True

import importlib
setupMenuPath = "TriggerMenuMT.HLTMenuConfig.Menu."+flags.Trigger.triggerMenuSetup+"_newJO"
setupMenuModule = importlib.import_module( setupMenuPath )
assert setupMenuModule is not None, "Could not import module {}".format(setupMenuPath)
assert setupMenuModule.setupMenu is not None, "Could not import setupMenu from {}".format(setupMenuPath)
flags.needFlagsCategory('Trigger')
setupMenuModule.setupMenu(flags)
flags.Exec.MaxEvents=50
flags.Input.isMC = False
flags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]


flags.Concurrency.NumThreads=1
flags.Concurrency.NumConcurrentEvents=1

flags.InDet.useSctDCS=False
flags.InDet.usePixelDCS=False

flags.lock()


from AthenaCommon.Constants import INFO,DEBUG,WARNING
acc = MainServicesCfg( flags )
acc.getService('AvalancheSchedulerSvc').VerboseSubSlots = True

# this delcares to the scheduer that EventInfo object is produced
acc.addEventAlgo( CompFactory.SGInputLoader( Load = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )] ),
                      "AthAlgSeq" )


from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
acc.merge(ByteStreamReadCfg( flags ))


from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig( flags ))

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu as generateHLTMenu
from TriggerJobOpts.TriggerConfig import triggerRunCfg
acc.merge( triggerRunCfg( flags, seqName = "AthMasterSeq", menu=generateHLTMenu ) )

from RegionSelector.RegSelConfig import regSelCfg
acc.merge( regSelCfg( flags ) )


acc.getEventAlgo( "TrigSignatureMoniMT" ).OutputLevel=DEBUG
acc.getEventAlgo( "L1Decoder" ).ctpUnpacker.UseTBPBits=True # test setup



from AthenaCommon.Logging import logging
logging.getLogger('forcomps').setLevel(DEBUG)
acc.foreach_component("*/L1Decoder").OutputLevel = DEBUG
acc.foreach_component("*/L1Decoder/*Tool").OutputLevel = DEBUG # tools
acc.foreach_component("*HLTTop/*Hypo*").OutputLevel = DEBUG # hypo algs
acc.foreach_component("*HLTTop/*Hypo*/*Tool*").OutputLevel = INFO # hypo tools
acc.foreach_component("*HLTTop/RoRSeqFilter/*").OutputLevel = INFO# filters
acc.foreach_component("*HLTTop/*Input*").OutputLevel = DEBUG # input makers
acc.foreach_component("*HLTTop/*HLTEDMCreator*").OutputLevel = WARNING # messaging from the EDM creators
acc.foreach_component("*HLTTop/*GenericMonitoringTool*").OutputLevel = WARNING # silcence mon tools (addressing by type)


acc.printConfig(withDetails=False, summariseProps=True, printDefaults=True)


fname = "runHLT_standalone_newJO.pkl"
print( "Storing config in the file {}".format( fname ) )
with open(fname, "wb") as p:
    acc.store( p )
    p.close()
status = acc.run()
if status.isFailure():
    import sys
    sys.exit(1)
