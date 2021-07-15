#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.Logging import logging
log = logging.getLogger('runHLT_standalone_newJO')

from AthenaConfiguration.ComponentAccumulator import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior = 1

flags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'

flags.Detector.GeometryPixel = True
flags.Detector.GeometrySCT = True
flags.Detector.GeometryTRT = True
flags.Detector.GeometryID = True
flags.Detector.GeometryBpipe = True
flags.Detector.GeometryCavern = False
flags.Detector.GeometryPixel = True
flags.Detector.GeometrySCT = True
flags.Detector.GeometryTRT = True

flags.Detector.GeometryLAr = True
flags.Detector.GeometryTile = True
flags.Detector.GeometryMDT = True
flags.Detector.GeometryTGC = True
flags.Detector.GeometryCSC = True
flags.Detector.GeometryRPC = True

# Output configuration - currently testing offline workflow
flags.Trigger.writeBS = False
flags.Trigger.EDMVersion = 3
flags.Output.doWriteRDO = True
flags.Output.RDOFileName = 'RDO_TRIG.pool.root'

flags.Trigger.CostMonitoring.doCostMonitoring = True
flags.Trigger.doRuntimeNaviVal = True
flags.Scheduler.CheckDependencies = True
flags.Scheduler.ShowDataDeps = True
flags.Scheduler.ShowDataFlow = True
flags.Scheduler.ShowControlFlow = True
flags.Scheduler.EnableVerboseViews = True

flags.Exec.MaxEvents = 50
# TODO this two should be resolved in a smarter way (i.e. required passing the tag from the driver test, however now, parsing of string with - fails)
flags.IOVDb.GlobalTag = lambda f: 'OFLCOND-MC16-SDR-25' if f.Input.isMC else "CONDBR2-HLTP-2018-01"
flags.Common.isOnline = lambda f: not f.Input.isMC
flags.Common.MsgSourceLength=70
flags.Trigger.doLVL1=True # run L1 sim also on data
flags.Concurrency.NumThreads = 1

flags.InDet.useSctDCS = False
flags.InDet.usePixelDCS = False

# Calo is currently the only client of Transient BS
flags.Trigger.doTransientByteStream = lambda f: f.Input.Format == 'POOL' and f.Trigger.doCalo

# command line handling
# options that are defined in: AthConfigFlags are handled here
# they override values from above
parser = flags.getArgumentParser()
args = flags.fillFromArgs(parser=parser)
log.info("Command line arguments:")
import sys
log.info(" ".join(sys.argv))
flags.lock()
# Enable when debugging deduplication issues
# ComponentAccumulator.debugMode = "trackCA trackEventAlog ... and so on"


from AthenaCommon.Constants import DEBUG, INFO, WARNING
acc = MainServicesCfg(flags)

acc.getService('AvalancheSchedulerSvc').VerboseSubSlots = True

# this delcares to the scheduler that EventInfo object comes from the input
loadFromSG = [('xAOD::EventInfo', 'StoreGateSvc+EventInfo')]

if flags.Input.Format == 'BS':
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(flags))
else:
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))


from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig(flags))

from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu as generateHLTMenu
from TriggerJobOpts.TriggerConfig import triggerRunCfg
menu = triggerRunCfg(flags, menu=generateHLTMenu)
# uncomment to obtain printout of menu (and associated components)
# menu.printConfig(withDetails=True, summariseProps=True)
acc.merge(menu)

if flags.Trigger.doTransientByteStream and flags.Trigger.doCalo:
    from TriggerJobOpts.TriggerTransBSConfig import triggerTransBSCfg_Calo
    acc.merge(triggerTransBSCfg_Calo(flags), sequenceName="HLTBeginSeq")

if flags.Input.isMC and flags.Trigger.doMuon:
    loadFromSG += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' ), ( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO' )]

if flags.Trigger.doLVL1:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationCfg
    acc.merge(Lvl1SimulationCfg(flags), sequenceName="HLTBeginSeq")

acc.addEventAlgo(CompFactory.SGInputLoader(Load=loadFromSG), sequenceName="AthAlgSeq")

# The L1 presacles do not get created in the menu setup
from TrigConfigSvc.TrigConfigSvcCfg import createL1PrescalesFileFromMenu
createL1PrescalesFileFromMenu(flags)


acc.getEventAlgo("TrigSignatureMoni").OutputLevel = INFO
#acc.getEventAlgo("L1Decoder").ctpUnpacker.UseTBPBits = not flags.Input.isMC # test setup on data


logging.getLogger('forcomps').setLevel(DEBUG)
acc.foreach_component("*/L1Decoder").OutputLevel = INFO
acc.foreach_component("*/L1Decoder/*Tool").OutputLevel = INFO # tools
acc.foreach_component("*HLTTop/*Hypo*").OutputLevel = INFO # hypo algs
acc.foreach_component("*HLTTop/*Hypo*/*Tool*").OutputLevel = INFO # hypo tools
acc.foreach_component("*HLTTop/RoRSeqFilter/*").OutputLevel = INFO# filters
acc.foreach_component("*/FPrecisionCalo").OutputLevel = INFO# filters
acc.foreach_component("*/CHElectronFTF").OutputLevel = INFO# filters
acc.foreach_component("*HLTTop/*Input*").OutputLevel = INFO # input makers
acc.foreach_component("*HLTTop/*HLTEDMCreator*").OutputLevel = INFO # messaging from the EDM creators
acc.foreach_component("*HLTTop/*GenericMonitoringTool*").OutputLevel = WARNING # silence mon tools (addressing by type)

if log.getEffectiveLevel() <= logging.DEBUG:
    acc.printConfig(withDetails=False, summariseProps=True, printDefaults=True)


fName =  args.configOnly if args.configOnly else "runHLT_standalone_newJO.pkl" 
log.info("Storing config in the file %s ", fName)
with open(fName, "wb") as p:
    acc.store(p)
    p.close()

if not args.configOnly:
    log.info("Running ...")
    status = acc.run()
    if status.isFailure():
        import sys
        sys.exit(1)
else:
    log.info("The configOnly option used ... exiting.")
