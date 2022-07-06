#!/usr/bin/env python

#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
log = logging.getLogger('runHLT_standalone_newJO')

from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaConfiguration.ComponentAccumulator import CompFactory
from AthenaConfiguration.Enums import Format, LHCPeriod
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior = 1

# Output configuration - currently testing offline workflow
flags.Trigger.writeBS = False
flags.Trigger.EDMVersion = 3
flags.Output.doWriteRDO = True
flags.Output.RDOFileName = 'RDO_TRIG.pool.root'

flags.Trigger.doHLT = True
flags.Trigger.CostMonitoring.doCostMonitoring = True
flags.Trigger.doRuntimeNaviVal = True
flags.Scheduler.CheckDependencies = True
flags.Scheduler.ShowDataDeps = True
flags.Scheduler.ShowDataFlow = True
flags.Scheduler.ShowControlFlow = True
flags.Scheduler.EnableVerboseViews = True
from CaloClusterCorrection.constants \
     import CALOCORR_JO, CALOCORR_POOL
flags.Calo.ClusterCorrection.defaultSource = [CALOCORR_POOL, CALOCORR_JO] # temporary, until a complete support for cool is present

flags.Exec.MaxEvents = 50
# TODO this two should be resolved in a smarter way (i.e. required passing the tag from the driver test, however now, parsing of string with - fails)
flags.Common.isOnline = lambda f: not f.Input.isMC
# temporarily roll back to v5 for Run3 MC due to incompatibility between MC21 RDO and v6 MDT conditions
flags.IOVDb.GlobalTag = lambda f: ('OFLCOND-MC16-SDR-RUN2-08-02' if f.GeoModel.Run is LHCPeriod.Run2 else 'OFLCOND-MC21-SDR-RUN3-05') if f.Input.isMC else "CONDBR2-HLTP-2018-03"
flags.Common.MsgSourceLength=70
flags.Trigger.doLVL1=True # run L1 sim also on data
flags.Trigger.enableL1CaloPhase1=False
flags.Trigger.enableL1CaloLegacy=True
flags.Concurrency.NumThreads = 1

# disable Prescales for initial dev phase to allow only few chains in the menu
flags.Trigger.disableCPS=True

# select chains, as in runHLT_standalone
flags.addFlag("Trigger.enabledSignatures",[])  
flags.addFlag("Trigger.disabledSignatures",[]) 
flags.addFlag("Trigger.selectChains",[])       
flags.addFlag("Trigger.disableChains",[]) 


flags.Trigger.enabledSignatures = ['Muon', 'Photon','Electron']
#flags.Trigger.disableChains=["HLT_2mu4_l2io_invmDimu_L1BPH-2M9-0DR15-2MU3VF", "HLT_2mu4_l2io_invmDimu_L1BPH-2M9-0DR15-2MU3V", "HLT_2mu6_l2io_invmDimu_L1BPH-2M9-2DR15-2MU5VF"]
# exclude jets for now, since their MenuSeuqnece Structure needs more work to migrate

#--------------#
#Leave commented lines for tests, since this is under development
#from AthenaCommon.Constants import DEBUG
#flags.Exec.OutputLevel=DEBUG
#flags.Trigger.triggerMenuModifier=[ 'emptyMenu','HLT_mu8_L1MU5VF']
#flags.Trigger.selectChains = [ 'HLT_mu8_L1MU5VF']#'HLT_j0_perf_L1RD0_FILLED']#'HLT_mu26_ivarmedium_mu6_l2io_probe_L1MU14FCH'] #'HLT_mu0_muoncalib_L1MU14FCH', 'HLT_mu6_L1MU5VF','HLT_mu6_msonly_L1MU5VF'] #'HLT_mu0_muoncalib_L1MU14FCH',#HLT_mu6_L1MU5VF
#--------------#

# if set to True, use standalone menu generation code
oldMenuCode=False


if flags.Trigger.Online.isPartition:
    flags.GeoModel.AtlasVersion = flags.Trigger.OnlineGeoTag
# else rely on the auto-configuration from input file


flags.InDet.useSctDCS = False
flags.InDet.usePixelDCS = False

# command line handling
# options that are defined in: AthConfigFlags are handled here
# they override values from above
parser = flags.getArgumentParser()
args = flags.fillFromArgs(parser=parser)
log.info("Command line arguments:")
import sys
log.info(" ".join(sys.argv))
flags.lock()
flags.dump()
# Enable when debugging deduplication issues
# ComponentAccumulator.debugMode = "trackCA trackEventAlog ... and so on"


acc = MainServicesCfg(flags)

acc.getService('AvalancheSchedulerSvc').VerboseSubSlots = True

# this delcares to the scheduler that EventInfo object comes from the input
loadFromSG = [('xAOD::EventInfo', 'StoreGateSvc+EventInfo')]

if flags.Input.Format is Format.BS:
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(flags))
else:
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(flags))


from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig(flags))



if oldMenuCode:
#  LoadAndGenerateMenu loads test _NewJO menu
    from TriggerMenuMT.HLT.Config.GenerateMenuMT_newJO import LoadAndGenerateMenu as generateHLTMenu
else:
    from TriggerMenuMT.HLT.Config.GenerateMenuMT_newJO import generateMenuMT as generateHLTMenu


from TriggerJobOpts.TriggerConfig import triggerRunCfg
menu = triggerRunCfg(flags, menu=generateHLTMenu)
# uncomment to obtain printout of menu (and associated components)
# menu.printConfig(withDetails=True, summariseProps=True)
acc.merge(menu)

from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
acc.merge(LumiBlockMuWriterCfg(flags), sequenceName="HLTBeginSeq")

if flags.Trigger.doTransientByteStream and flags.Trigger.doCalo:
    from TriggerJobOpts.TriggerTransBSConfig import triggerTransBSCfg_Calo
    acc.merge(triggerTransBSCfg_Calo(flags), sequenceName="HLTBeginSeq")

if flags.Input.isMC and flags.Trigger.doMuon:
    loadFromSG += [( 'RpcPadContainer' , 'StoreGateSvc+RPCPAD' ), ( 'TgcRdoContainer' , 'StoreGateSvc+TGCRDO' )]

if flags.Trigger.doLVL1:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationCfg
    acc.merge(Lvl1SimulationCfg(flags), sequenceName="HLTBeginSeq")

acc.addEventAlgo(CompFactory.SGInputLoader(Load=loadFromSG), sequenceName="AthAlgSeq")

#track overlay needs this to ensure that the collections are copied correctly (due to the hardcoding of the name in the converters)
if flags.Overlay.doTrackOverlay:
    from TrkEventCnvTools.TrkEventCnvToolsConfigCA import TrkEventCnvSuperToolCfg
    acc.merge(TrkEventCnvSuperToolCfg(flags))

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
