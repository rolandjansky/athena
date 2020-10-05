# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import TrigCostMonitor.TrigCostMonitorConf as Conf
from AthenaCommon.Logging import logging

#----------------------------------------------------------------------
# Global constants
#
steeringL2_online_doOperationalInfo=10  # frequency of operartional info collection at L2
steeringEF_online_doOperationalInfo=1   # frequency of operartional info collection at EF
steeringHLT_online_doOperationalInfo=10  # frequency of operartional info collection at HLT

#----------------------------------------------------------------------
# Base class which defines target() method used by TrigSteer config
#
class TrigCostTool_Target(Conf.TrigCostTool):
    __slots__ = []
    
    def __init__(self, name):
        super(Conf.TrigCostTool, self).__init__(name)

        from TrigSteering.TrigSteeringConfig import RandomScaler
        self.scalerTool = RandomScaler('TrigCostScaler')

        from CoolLumiUtilities.BunchGroupCondAlgDefault import BunchGroupCondAlgDefault
        BunchGroupCondAlgDefault()
        
    def target(self):
        return [self.monitoringTarget]

#----------------------------------------------------------------------
# Prepare TrigCostTool instance using target as clue
#
def prepareCostTool(target):
    
    log  = logging.getLogger('prepareCostTool:'+str(target))
    tool = TrigCostTool_Target("Trig"+target)

    developmentMode = False
    useSaveTools = False
    
    if target.count('L2') == 1:
        tool.level = 'L2'
        tool.keyResult = 'HLTResult_L2'
        tool.monitoringStream = 'L2CostMonitoring'
    elif target.count('EF') == 1:
        tool.level = 'EF'
        tool.keyResult = 'HLTResult_EF'
        tool.monitoringStream = 'EFCostMonitoring'
    elif target.count('HLT') == 1:
        tool.level = 'HLT'
        tool.keyResult = 'HLTResult_HLT'  # HLT result stores which XPU node the trigger was running on
        log.info('Setting up in prepareCostTool - using monitoringStream = CostMonitoring')
        tool.monitoringStream = 'CostMonitoring' # Migration to run 2 menu, using conf. as in LS1_v1 menu
    else:
        raise Exception('Unknown target level: '+target)
    
    tool.monitoringLogic  = 'no-calib no-debug no-physics'
    tool.monitoringTarget = target
    tool.purgeCostStream  = True  # Remove cost stream if no data to save 
    tool.writeAlways      = False # This is set to True by postSetupCostForCAF()
    tool.writeConfig      = False # Now know why this should be false online, consumes too much bandwidth. Will read from COOL at T0 instead.
    tool.writeConfigDB    = False # If reading from the DB, do we want to save the data? Should not be needed as EBWeight calc is in this package.
    tool.useConfDb        = True  # Ask toolConf to fetch online config from DB. Only used currently if running costoForCAF
    tool.costForCAF       = False # This is set to True by postSetupCostForCAF()
    tool.doEBWeight       = False # Calculate EB weighting factors. Use 
    tool.useConfSvc       = True  # Ask toolConf to fetch current config from config service
    tool.saveEventTimers  = True
    tool.stopAfterNEvent  = 800
    tool.execPrescale     = 1.0
    tool.doConfigReduction = True # Online, only send config info from 1/50 PU nodes to reduce wasted bandwidth (only 1 copy needed at T0) 
    
    tool.toolConf     = Conf.Trig__TrigNtConfTool('Conf'+target)
    tool.toolEBWeight = Conf.Trig__TrigNtEBWeightTool('Ebwt'+target)
    tool_elem = Conf.Trig__TrigNtElemTool('Elem'+target) # Not currently utilised
    tool_exec = Conf.Trig__TrigNtExecTool('Exec'+target)
    tool_lvl1 = Conf.Trig__TrigNtLvl1Tool('Lvl1'+target)
    tool_hlt2 = Conf.Trig__TrigNtHltRTool('Hlt2'+target)
    tool_robs = Conf.Trig__TrigNtRobsTool('Robs'+target)

    #
    # Add TrigNtSaveTool tool for offline running of athenaMT/PT/HLT
    #
    if useSaveTools:
        save_cost = Conf.Trig__TrigNtSaveTool('SaveCost_'+target)
        save_rate = Conf.Trig__TrigNtSaveTool('SaveRate_'+target)
        #
        save_cost.fileName      = 'TrigCost'+tool.level+'.root'
        save_cost.writeFile     = True
        save_cost.writeCostOnly = True
        save_cost.writeRateOnly = False
        #
        save_rate.fileName      = 'TrigRate'+tool.level+'.root'
        save_rate.writeFile     = True
        save_rate.writeCostOnly = False
        save_rate.writeRateOnly = True

    if developmentMode: # This will generate a lot of output
        tool_exec.printOPI         = True
        tool_exec.printSEQ         = True 
        tool_robs.printDebug       = True
        tool.toolConf.printConfig  = True #This is a crazy amout of output 
        tool.printEvent            = True
    
    if target.count('CostExec') == 1:
        
        # Old settings
        #tool_robs.cleanROBs      = False
        #tool_robs.keepSubDet     = False
        #tool_elem.saveNavigation = False
        #tool_exec.saveSeqTimer   = False

        # Updating CostExec to save more by default
        # New settings
        tool.doTiming         = True
        tool.saveEventTimers  = True
        #
        tool_robs.cleanROBs        = False
        tool_robs.keepSubDet       = False
        tool_elem.saveNavigation   = True
        tool_elem.filterTE         = True
        tool_exec.saveSeqTimer     = True
        ## INFO: If using this tool to get the HLT result from storegate in the combined trigger,
        ## TrigSteering needs re-ordering as the OPIMonitoring tools are run before the ResultBuilder.
        ## Alternativly, don't use HLTResult by setting useSteering=True
        tool_hlt2.saveFailedChains = True
        tool_hlt2.useSteering      = True
        tool_hlt2.keyResult = 'HLTResult_HLT'
        if tool.level != "HLT":
          tool_hlt2.keyResult = 'HLTResult_L2'
                
        if   tool.level == "L2":
            tool.eventTools += [ tool_lvl1 ]
            tool.scaleTools += [ tool_exec ]
            tool.scaleTools += [ tool_robs ]
        elif tool.level == "EF":
            tool.scaleTools += [ tool_exec ]
            tool.scaleTools += [ tool_hlt2 ]
        elif tool.level == "HLT":
            tool.eventTools += [ tool_lvl1 ]
            tool.scaleTools += [ tool_exec ]
            tool.scaleTools += [ tool_robs ]
            tool.scaleTools += [ tool_hlt2 ]

        # Removing by default the Save tools - we now have the D3PDMaker for this
        if useSaveTools:
            tool.alwaysTools += [save_cost, save_rate]
            
    elif target.count('CostAthena') == 1:
        log.error('The CostAthena mode of TrigCostMonitor has been depricated and removed.')
                    
    else:
        raise Exception('Unknown target: '+target)
    
    return tool
        
#----------------------------------------------------------------------
# List of available monitoring tools for TrigSteering
#
TrigCostToolsList = [ prepareCostTool('CostExecL2'),
                      prepareCostTool('CostExecEF'),
                      prepareCostTool('CostExecHLT'),
                      #prepareCostTool('CostAthenaL2'),
                      #prepareCostTool('CostAthenaEF'),
                      #prepareCostTool('CostAthenaHLT')
                    ]

#----------------------------------------------------------------------
# prepare TrigCostRun configuration
#
def prepareTrigSerialize(log):
    
    from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
    from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
    
    trigCnv = TrigSerializeConvHelper(doTP = True)
    trigSer = TrigSerTPTool('TrigSerTPTool')

    from TrigEDMConfig.TriggerEDM import getL2BSTypeList, getEFBSTypeList, getTPList

    trigSer.TPMap = getTPList()
    trigSer.ActiveClasses = getL2BSTypeList()+getEFBSTypeList()
    
    from AthenaCommon.AppMgr import ToolSvc

    if not hasattr(ToolSvc, trigSer.name()):
        ToolSvc += trigSer
        log.info('Added TrigSerTPTool to ToolSvc')
    else:
        log.info('TrigSerTPTool already exists')
        
    if not hasattr(ToolSvc, trigCnv.name()):
        ToolSvc += trigCnv
        log.info('Added TrigSerializeConvHelper to ToolSvc')
    else:
        log.info('TrigSerializeConvHelper already exists')
    
#----------------------------------------------------------------------
# prepare TrigCostRun configuration
#
def prepareCostRun(name, option = 'hlt'):
    
    log = logging.getLogger('prepareCostRun')

    log.info('Setup for BS reading...')
    prepareTrigSerialize(log)

    from TrigEDMConfig.TriggerEDM import EDMLibraries
    from TrigNavigation.TrigNavigationConfig import HLTNavigationOnline

    run = Conf.TrigCostRun(name)

    run.printEvent  = True
    run.keyStream   = ''
    
    run.keyResultL2  = 'HLTResult_L2'
    run.keyResultEF  = 'HLTResult_EF'
    run.keyResultHLT = 'HLTResult_HLT'

    run.keyConfigL2  = 'HLT_TrigMonConfigCollection_OPI_L2_monitoring_config'
    run.keyConfigEF  = 'HLT_TrigMonConfigCollection_OPI_EF_monitoring_config'
    run.keyConfigHLT = 'HLT_TrigMonConfigCollection_OPI_HLT_monitoring_config'
   
    run.keyEventL2   = 'HLT_TrigMonEventCollection_OPI_L2_monitoring_event'
    run.keyEventEF   = 'HLT_TrigMonEventCollection_OPI_EF_monitoring_event' 
    run.keyEventHLT  = 'HLT_TrigMonEventCollection_OPI_HLT_monitoring_event'  

    run.doL2 = False
    run.doEF = False
    run.doHLT = False

    if option.count('l2'):
      run.doL2 = True

    if option.count('ef'):
      run.doEF = True

    if option.count('hlt'):
      run.doHLT = True

    run.navigation = HLTNavigationOnline()    
    run.navigation.ReferenceAllClasses = False
    run.navigation.Dlls = EDMLibraries

    tool_conf = Conf.Trig__TrigNtConfTool('RunCostConf')
    tool_post = Conf.Trig__TrigNtPostTool('RunCostPost')
    
    tool_conf.printConfig  = False
    tool_conf.useDB        = True

    #
    # Configure old style output
    #
    if option.count('useSaveTools'):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'THistSvc'):
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr += THistSvc()

        svcMgr.THistSvc.Output += ["TrigCostReadBS DATAFILE='TrigCostReadBS.root' OPT='RECREATE'"]

        save_cost = Conf.Trig__TrigNtSaveTool('RunCostSave_full')
        save_cost.writeFile        = False
        save_cost.fileName         = ''
        save_cost.streamConfig     = 'TrigCostReadBS'
        save_cost.streamEvent      = 'TrigCostReadBS'
        save_cost.writeRateOnly    = False

        save_rate = Conf.Trig__TrigNtSaveTool('RunCostSave_rate')
        save_rate.writeFile        = False
        save_rate.fileName         = ''
        save_rate.streamConfig     = ''
        save_rate.streamEvent      = 'TrigCostReadBS'
        save_rate.writeRateOnly    = True
        save_rate.treeNameEvent    = 'event_rate_only'
        save_rate.treeNameConfig   = ''
        save_rate.printPostSummary = False
        
        run.tools     = [tool_conf, tool_post]
        run.toolsSave = [save_cost, save_rate]

    if option.count('emon'):
        run.tools = []
        try:            
            from TrigCost2IS.TrigCost2ISConf import Trig__TrigNtCost2IS
            tool_2is = Trig__TrigNtCost2IS('RunCost2IS')
            
            log.info('Configuring emon algorithm')
            log.info('Picked up HLT tool: RunCost2IS')

            run.tools = [tool_2is]
        except Exception:
            log.info('HLT tools are not available... continue without them')

    log.info('Prepared TrigCostRun algorithm instance: '+run.getName())
    return run
    
#----------------------------------------------------------------------
# prepare TrigCostAlg configuration for reading online LV1 and HLT results
#
def prepareReadOnlineAlg(name, suffix):

    log = logging.getLogger('prepareReadOnlineAlg')
    log.warning('TrigCost Online has not been updated yet for combined HLT running!')
    
    # TODO NOTE:
    # This function has not been updated yet for combined HLT running.

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc()

    alg = Conf.TrigCostAlg(name)
    alg.mergeEvent = True
    alg.printEvent = True

    conf_tool = Conf.Trig__TrigNtConfTool('CostNtConf_'+suffix)
    lvl1_tool = Conf.Trig__TrigNtLvl1Tool('CostNtLvl1_'+suffix)
    hlt2_tool = Conf.Trig__TrigNtHltRTool('CostNtHlt2_'+suffix)
    hlt3_tool = Conf.Trig__TrigNtHltRTool('CostNtHlt3_'+suffix)
    post_tool = Conf.Trig__TrigNtPostTool('CostNtPost_'+suffix)
    save_tool = Conf.Trig__TrigNtSaveTool('CostNtSave_'+suffix)

    conf_tool.useDB     = True
    hlt2_tool.keyResult = 'HLTResult_L2'
    hlt3_tool.keyResult = 'HLTResult_EF'

    svcMgr.THistSvc.Output += ["TrigRateOnline DATAFILE='TrigRateOnline.root' OPT='RECREATE'"]
       
    save_tool.streamConfig  = 'TrigRateOnline'
    save_tool.streamEvent   = 'TrigRateOnline'
    save_tool.writeFile     = False
    save_tool.writeRateOnly = True
    
    #
    # Add all tools. If using combined trigger - then no HLT l3
    #
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if not TriggerFlags.doHLT():
        alg.tools += [conf_tool, lvl1_tool, hlt2_tool, hlt3_tool, post_tool, save_tool]
    else:
        hlt2_tool.keyResult = 'HLTResult_HLT'
        alg.tools += [conf_tool, lvl1_tool, hlt2_tool, post_tool, save_tool]
    
    return alg
        
#----------------------------------------------------------------------
# Configure full offline cost algorithm
#
def setupCostExtras(config = ''):

    log = logging.getLogger('setupCostExtras')

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        
    if not hasattr(svcMgr, "AthenaEventLoopMgr"):
        from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
        svcMgr += AthenaEventLoopMgr()

    svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
    log.info('Set AthenaEventLoopMgr.EventPrintoutInterval = 100')
    
    if not hasattr(svcMgr, "AtRndmGenSvc"):
        from AthenaServices.AthenaServicesConf import AtRndmGenSvc
        svcMgr += AtRndmGenSvc()
        
    svcMgr.AtRndmGenSvc.OutputLevel = 4
    log.info('Set AtRndmGenSvc.OutputLevel = 4')

    if not hasattr(svcMgr, "AtRanluxGenSvc"):
        from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
        svcMgr += AtRanluxGenSvc()

    svcMgr.AtRanluxGenSvc.OutputLevel = 4
    log.info('Set AtRanluxGenSvc.OutputLevel = 4')
        
    if config.count('TIME') > 0:
        log.info('Configure TrigTimerSvc...')
        from TrigTimeAlgs.TrigTimeAlgsConf import TrigTimerSvc
        
        if not hasattr(svcMgr, 'TrigTimerSvc'):
            svcMgr += TrigTimerSvc()
        
        svcMgr.TrigTimerSvc.IncludeName = ".+"
        svcMgr.TrigTimerSvc.ExcludeName = "()"
        svcMgr.TrigTimerSvc.OutputLevel = 4
        
        log.info('Setup detailed timing:')
        print (svcMgr.TrigTimerSvc    )

#----------------------------------------------------------------------
# Configure TrigSteering algorithm
#
def configSteeringOPI(topSeq, name, config, log):
    
    if not hasattr(topSeq, name):
        log.info(name+' does not exist at AlgSequence') 
        return

    trigSteer = getattr(topSeq, name)
    
    trigSteer.doTiming=True
    trigSteer.doOperationalInfo=1

    log.info('Set '+name+'.doTiming=True')
    log.info('Set '+name+'.doOperationalInfo='+str(trigSteer.doOperationalInfo))

    if config.count('NOPS'):
        trigSteer.LvlConverterTool.ignorePrescales=True
        log.info('Set '+name+'.LvlConverterTool.ignorePrescales=True')
        
        if name.count('L2') or name.count('HLT'):
            trigSteer.LvlConverterTool.Lvl1ResultAccessTool.ignorePrescales=True
            log.info('Set '+name+'.LvlConverterTool.Lvl1ResultAccessTool.ignorePrescales=True')
        
            if 'ignoreL1Prescales' in trigSteer.LvlConverterTool.properties():
                trigSteer.LvlConverterTool.ignoreL1Prescales = True
                log.info('Set '+name+'.LvlConverterTool.ignoreL1Prescales = True')
            else:
                log.info('Missing property: '+name+'.LvlConverterTool.ignoreL1Prescales')            

    if 'OPITools' in trigSteer.properties():
        for tool in trigSteer.OPITools:
            if tool.getType().count('TrigCostTool'):
                tool.writeAlways = True
                log.info('Set '+name+'.'+tool.name()+'.writeAlways = True')

#----------------------------------------------------------------------
# Set various trigger options needed for typical offline cost/rate job
#
def setupCostJob(config = 'OPI ROB NOPS'):

    log = logging.getLogger('setupCostJob')
    
    if config.count('OPI'):
        log.info('Configure TrigSteer algorithm(s)')

        from AthenaCommon.AlgSequence import AlgSequence    
        topSeq = AlgSequence()
        
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if TriggerFlags.doHLT():
            configSteeringOPI(topSeq, 'TrigSteer_HLT', config, log)
        else: 
            if TriggerFlags.doEF():
                configSteeringOPI(topSeq, 'TrigSteer_EF', config, log)
            if TriggerFlags.doLVL2():
                configSteeringOPI(topSeq, 'TrigSteer_L2', config, log)
        
    if config.count('ROB'):
        log.info('Will try to configure online monitoring for ROBDataProviderSvc...')
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        
        try:
            log.info('ROBDataProviderSvc type: '+svcMgr.ROBDataProviderSvc.getType())
            
            if svcMgr.ROBDataProviderSvc.getType() == 'Lvl2ROBDataProviderSvc':
                log.info('Job already uses online version ROBDataProvider')
            else:
                from AthenaCommon.Include import include
                include('TrigDataAccessMonitoring/MonROBDataProviderSvc.py')
                log.info('Setup "online" monitoring for ROBDataProvider')
        except Exception:
            log.warning('Failed to setup "online" version of ROBDataProvider')
            
        try:
            svcMgr.ROBDataProviderSvc.doDetailedROBMonitoring = True
            log.info('Set ROBDataProviderSvc.doDetailedROBMonitoring=True')
        except Exception:
            log.warning('Failed to set ROBDataProviderSvc.doDetailedROBMonitoring')
            
#----------------------------------------------------------------------
# Setup debug parameters for steering
#
def setSteerDebug(name, option, log):

    from AthenaCommon.AlgSequence import AlgSequence    
    topSeq = AlgSequence()
    
    if hasattr(topSeq, name):
        trigSteer = getattr(topSeq, name)
        
        if 'OPITools' not in trigSteer.properties():
            return
        
        for tool in trigSteer.OPITools:
            if tool.getType().count('TrigCostTool'):
                tool.OutputLevel = 2
                log.info('Setup debug options for: '+tool.name())
                
                if option.count('evt') or option.count('event'):
                    tool.printEvent = True
                    log.info('Set evt debug option')
                    
                for nt in tool.scaleTools:
                    if nt.getType().count("TrigNtExec") > 0 and option.count('opi') > 0:
                        nt.printSEQ = True
                        nt.printOPI = True
                        log.info('Setup opi debug option for: '+nt.name())

                    if nt.getType().count("TrigNtRobs") > 0 and option.count('rob') > 0:
                        nt.printDebug = True
                        nt.OutputLevel = 2
                        log.info('Setup rob debug option for: '+nt.name())
                        
#----------------------------------------------------------------------
# Setup debug parameters for cost
#                   
def setupCostDebug(option = "cost"):

    log = logging.getLogger('setupCostDebug')

    from AthenaCommon.AlgSequence import AlgSequence    
    topSeq = AlgSequence()    
    print (topSeq)

    setSteerDebug('TrigSteer_L2', option, log)
    setSteerDebug('TrigSteer_EF', option, log)
    setSteerDebug('TrigSteer_HLT', option, log)

    if option.count('cost') > 0 and hasattr(topSeq, 'TrigCostAlg_cost'):
        trigCost = getattr(topSeq, 'TrigCostAlg_cost')
        trigCost.OutputLevel = 2
        log.info('Setup debug options for TrigCostAlg_cost')
        
    if option.count('rate') > 0 and hasattr(topSeq, 'TrigCostAlg_rate'):
        trigCost = getattr(topSeq, 'TrigCostAlg_rate')
        trigCost.OutputLevel = 2
        log.info('Setup debug options for TrigCostAlg_rate')
        
#----------------------------------------------------------------------
# Enable cost monitoring on every event
#
def postSetupOnlineCost():
    
    log = logging.getLogger('postSetupOnlineCost')

    global steeringL2_online_doOperationalInfo
    global steeringEF_online_doOperationalInfo
    global steeringHLT_online_doOperationalInfo
    
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr        
    from AthenaCommon.AlgSequence import AlgSequence
    
    topSeq = AlgSequence()

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.Online.doValidation():
        log.info('Trigger is in validation mode: setting all doOperationalInfo=1 to make it reproducible')
        steeringL2_online_doOperationalInfo=1
        steeringEF_online_doOperationalInfo=1
        steeringHLT_online_doOperationalInfo=1
        log.info('Set global flag for L2: doOperationalInfo=%d', steeringL2_online_doOperationalInfo)
        log.info('Set global flag for EF: doOperationalInfo=%d', steeringEF_online_doOperationalInfo)
        log.info('Set global flag for HLT: doOperationalInfo=%d', steeringHLT_online_doOperationalInfo)
    
    if hasattr(topSeq, 'TrigSteer_L2'):        
        topSeq.TrigSteer_L2.doOperationalInfo=steeringL2_online_doOperationalInfo
        log.info('Set TrigSteer_L2.doOperationalInfo='+str(topSeq.TrigSteer_L2.doOperationalInfo))

        if 'OPITools' in topSeq.TrigSteer_L2.properties():
            for tool in topSeq.TrigSteer_L2.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.doOperationalInfo = steeringL2_online_doOperationalInfo
                    log.info('Set '+tool.name()+'.doOperationalInfo='+str(topSeq.TrigSteer_L2.doOperationalInfo))
        
        if 'doDetailedROBMonitoring' in svcMgr.ROBDataProviderSvc.properties():
            svcMgr.ROBDataProviderSvc.doDetailedROBMonitoring = True
            log.info('Set ROBDataProviderSvc.doDetailedROBMonitoring=True')
            
    if hasattr(topSeq, 'TrigSteer_EF'):
        topSeq.TrigSteer_EF.doOperationalInfo=steeringEF_online_doOperationalInfo
        log.info('Set TrigSteer_EF.doOperationalInfo='+str(topSeq.TrigSteer_EF.doOperationalInfo))

        if 'OPITools' in topSeq.TrigSteer_EF.properties():
            for tool in topSeq.TrigSteer_EF.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.doOperationalInfo = steeringEF_online_doOperationalInfo
                    log.info('Set '+tool.name()+'.doOperationalInfo='+str(topSeq.TrigSteer_EF.doOperationalInfo))
        
    if hasattr(topSeq, 'TrigSteer_HLT'):        
        topSeq.TrigSteer_HLT.doOperationalInfo=steeringHLT_online_doOperationalInfo
        log.info('Set TrigSteer_HLT.doOperationalInfo='+str(topSeq.TrigSteer_HLT.doOperationalInfo))

        if 'OPITools' in topSeq.TrigSteer_HLT.properties():
            for tool in topSeq.TrigSteer_HLT.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.doOperationalInfo = steeringHLT_online_doOperationalInfo
                    log.info('Set '+tool.name()+'.doOperationalInfo='+str(topSeq.TrigSteer_HLT.doOperationalInfo))
        
        if 'doDetailedROBMonitoring' in svcMgr.ROBDataProviderSvc.properties():
            svcMgr.ROBDataProviderSvc.doDetailedROBMonitoring = True
            log.info('Set ROBDataProviderSvc.doDetailedROBMonitoring=True')
        else:
            log.info('ROBDataProviderSvc does not have property doDetailedROBMonitoring - will not do cost monitor for ROS.')

#----------------------------------------------------------------------
# Set options for running cost on CAF - used together with CostExecL2/EF options!!!
#
def preSetupCostForCAF():
    log = logging.getLogger('preSetupCostForCAF')
    
    global steeringL2_online_doOperationalInfo
    global steeringEF_online_doOperationalInfo
    global steeringHLT_online_doOperationalInfo

    steeringL2_online_doOperationalInfo=1
    steeringEF_online_doOperationalInfo=1
    steeringHLT_online_doOperationalInfo=1

    log.info('Set global flag for L2: doOperationalInfo=%d', steeringL2_online_doOperationalInfo)
    log.info('Set global flag for EF: doOperationalInfo=%d', steeringEF_online_doOperationalInfo)
    log.info('Set global flag for HLT: doOperationalInfo=%d', steeringHLT_online_doOperationalInfo)

#----------------------------------------------------------------------
# Set options for running cost on CAF - used together with CostExecL2/EF options!!!
#
def postSetupCostForCAF():

    log = logging.getLogger('postSetupCostForCAF')

    from AthenaCommon.AlgSequence import AlgSequence
    
    topSeq = AlgSequence()
    
    if hasattr(topSeq, 'TrigSteer_L2'):        
        if 'OPITools' in topSeq.TrigSteer_L2.properties():
            for tool in topSeq.TrigSteer_L2.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.writeAlways = True
                    tool.costForCAF = True
                    log.info('Set '+tool.name()+'.writeAlways = True')

    if hasattr(topSeq, 'TrigSteer_EF'):
        if 'OPITools' in topSeq.TrigSteer_EF.properties():
            for tool in topSeq.TrigSteer_EF.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.writeAlways = True
                    tool.costForCAF = True
                    log.info('Set '+tool.name()+'.writeAlways = True')
                    
    if hasattr(topSeq, 'TrigSteer_HLT'):
        if 'OPITools' in topSeq.TrigSteer_HLT.properties():
            for tool in topSeq.TrigSteer_HLT.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.writeAlways = True
                    tool.costForCAF = True
                    log.info('Set '+tool.name()+'.writeAlways = True')

#----------------------------------------------------------------------
# Set options for calculating Enhanced Bias weighting factors
#
def postSetupEBWeighting():

    log = logging.getLogger('postSetupEBWeighting')
    from AthenaCommon.AlgSequence import AlgSequence
    topSeq = AlgSequence()
    
    if hasattr(topSeq, 'TrigSteer_L2'):        
        if 'OPITools' in topSeq.TrigSteer_L2.properties():
            for tool in topSeq.TrigSteer_L2.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.doEBWeight = True
                    log.info('Set '+tool.name()+'.doEBWeight = True')

    if hasattr(topSeq, 'TrigSteer_EF'):
        if 'OPITools' in topSeq.TrigSteer_EF.properties():
            for tool in topSeq.TrigSteer_EF.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.doEBWeight = True
                    log.info('Set '+tool.name()+'.doEBWeight = True')
                    
    if hasattr(topSeq, 'TrigSteer_HLT'):
        if 'OPITools' in topSeq.TrigSteer_HLT.properties():
            for tool in topSeq.TrigSteer_HLT.OPITools:
                if tool.getType().count('TrigCostTool'):
                    tool.doEBWeight = True
                    log.info('Set '+tool.name()+'.doEBWeight = True')
                    
#----------------------------------------------------------------------
# Read input files form local text file
#
def readInputFiles(filename = 'input_files.txt', file_key = 'RDO'):
    log = logging.getLogger('TrigCostMonitor:readInputFiles')
    
    import os
    filelist = []

    try:
        log.info('reading input files from: '+filename)
        mylist = open(filename)
        
        for line in mylist.readlines():        
            if len(file_key) > 0 and line.count(file_key) < 1:
                continue

            file = line.rstrip()
            fdir = os.path.dirname(file)
            
            if os.path.isdir(fdir) and not os.path.isfile(file):
                log.warning('Skipped non existing file: '+file)
                continue
            
            filelist.append(file)
            log.info('Added input file: '+file)
                
        mylist.close()
    except Exception:
        log.warning('Exception when reading file: '+filename)
    
    return filelist
