# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging  # loads logger
from PerfMonComps.PerfMonFlags import jobproperties
from AthenaCommon.Include import include
from AthenaCommon.GlobalFlags import GlobalFlags
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
# serializer replacing usuall online (this needs to be moved to TrigNavigation in DEV) ???
from TrigSerializeResult.TrigSerializeResultConf import TrigTSerializer

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc, theApp
from AthenaCommon.Include import include

from RecExConfig.Configured import Configured

from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR

def getHLTOutputLevel():
    level=DEBUG
    if globals().has_key('HLTOutputLevel'):
        level= globals()['HLTOutputLevel']
    return level

def makeKeysList(inputDict):
    tmpList={}
    for typename,v in inputDict.iteritems():
        if type(v) is type({}):
            stored = typename
            if 'collection' in v.keys():
                stored = v['collection']
            tmpList[ stored ] = v['sgkeys']
        else:
            tmpList[ typename ] = v
    return tmpList


def setTHistSvcOutput(outputList):
    """Build the Output list of the THistSvc. This is used below and to
    configure athenaMT/PT when running from the online DB but with the
    offline THistSvc"""
    
    if 1 not in [ o.count('SHIFT') for o in outputList ]:
        outputList += [ "SHIFT DATAFILE='shift-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('EXPERT') for o in outputList ]:
        outputList += [ "EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('run_1') for o in outputList ]:
        outputList += [ "run_1 DATAFILE='lbn-monitoring.root' OPT='RECREATE'"]        
    if 1 not in [ o.count('RUNSTAT') for o in outputList ]:
        outputList += [ "RUNSTAT DATAFILE='runstat-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('DEBUG') for o in outputList ]:        
        outputList += [ "DEBUG DATAFILE='debug-monitoring.root' OPT='RECREATE'"]

    return
        
    
def monitoringTools(steering):
    log = logging.getLogger("monitoringTools" )
    LBNdepth=4
    if 'EF' in steering.name():
        LBNdepth = 6
    def assign_prop_value(a):
        try:
            a.LBNHistoryDepth = LBNdepth
        except:
            return False
        return True

    try:
        Set = set
    except NameError:
        from sets import Set
    setOfEnabled = Set(TriggerFlags.enableMonitoring()) # this is set of enabled monitoring targets

    log.info("requested targets: "+str(setOfEnabled)+" pruning other tools")
    # prune subalgorithms
    for alg in steering.getChildren():
        tokeep = [ x for x in alg.AthenaMonTools if len(Set(x.target()) & setOfEnabled) != 0 ]
        log.debug( "will keep "+str( [ x.getFullName() for x in tokeep ]) )
        toscratch = list(set(alg.AthenaMonTools)-set(tokeep))        
        log.debug( "will scratch "+str( [ x.getFullName() for x in toscratch ]) )
        del toscratch        
        
        [ assign_prop_value(x) for x in tokeep ]
        alg.AthenaMonTools = tokeep
        
        if "Time" in TriggerFlags.enableMonitoring() or "TimeTree" in TriggerFlags.enableMonitoring():
            alg.doTiming=True            

        if "Log" in  TriggerFlags.enableMonitoring():
            alg.OutputLevel=getHLTOutputLevel()


    # prune steering monitoring tools
    steering.pruneSteeringMonTools(TriggerFlags.enableMonitoring())
    [ assign_prop_value(x) for x in steering.MonTools  ]

    from AthenaCommon.AppMgr import ServiceMgr
    # Enable performance monitoring
    if 'PerfMon' in TriggerFlags.enableMonitoring():
        from PerfMonComps.JobOptCfg import PerfMonSvc
        jobproperties.PerfMonFlags.doMonitoring = True
        ServiceMgr += PerfMonSvc()

    log.debug("Setting up offline THistSvc")
    if not hasattr(ServiceMgr, 'THistSvc'):        
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
    if hasattr(ServiceMgr.THistSvc, "Output"): # this is offline THistSvc fo which we want to setup files
        setTHistSvcOutput(ServiceMgr.THistSvc.Output)
        

def keyToLabel(key):
    """ The key is usually HLT_blablah, this function returns second part of it or empty string
    """
    if '_' not in key:
        return ''
    else:
        return key[key.index('_'):].lstrip('_')

class HLTSimulationGetter(Configured):

    log = logging.getLogger("HLTTriggergetter.py")

    def configure(self):
        
        log = logging.getLogger("HLTTriggergetter.py")

        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()

        #scheduling eventinfo
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'HltEventLoopMgr'):
            from RecExConfig.ObjKeyStore import objKeyStore
            if not objKeyStore.isInInput( "xAOD::EventInfo_v1"):
                from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
                topSequence+=xAODMaker__EventInfoCnvAlg()
	        

        log.info("Loading RegionSelector")
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += RegSelSvcDefault()

        # Configure the Data Preparation for Calo
        # This is a hack - configurables and toolhandles can be changed for next release
        if TriggerFlags.doCalo():  
            include('TrigT2CaloCommon/TrigDataAccessConfigured.py')
        
        if TriggerFlags.doFTK():
            # FTK algorithm inclusions
            from TrigFTKSim.TrigFTKSimConf import FTKMergerAlgo
            FTKMerger = FTKMergerAlgo( "FTKMergerAlgo" , OutputLevel=INFO)
            topSequence+= FTKMerger
        
        if TriggerFlags.doLVL2():
            log.info("configuring LVL2")
            from TrigSteering.TrigSteeringConfig import TrigSteer_L2, ReruningTrigSteer_L2
            if TriggerFlags.doFEX():
                log.info("configuring LVL2 for normal running (FEX + Hypo)")
                TrigSteer_L2 = TrigSteer_L2('TrigSteer_L2', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())
                TrigSteer_L2.doHypo = TriggerFlags.doHypo()
                
            if not TriggerFlags.doFEX() and TriggerFlags.doHypo():
                log.info("configuring LVL2 for re-running (Hypo only)")
                TrigSteer_L2 = ReruningTrigSteer_L2('TrigSteer_L2', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())

                #            if not TriggerFlags.writeBS():
            from TrigEDMConfig.TriggerEDM import getL2PreregistrationList, getEDMLibraries
            TrigSteer_L2.Navigation.ClassesToPreregister = getL2PreregistrationList()
                    
            TrigSteer_L2.Navigation.Dlls = getEDMLibraries()
            
            monitoringTools(TrigSteer_L2)            
            topSequence += TrigSteer_L2

        
        if TriggerFlags.doEF():
            log.info("configuring EF")
            from TrigSteering.TrigSteeringConfig import TrigSteer_EF,ReruningAfterL2TrigSteer_EF
            if TriggerFlags.doFEX():            
                TrigSteer_EF = TrigSteer_EF('TrigSteer_EF', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())
                TrigSteer_EF.doHypo = TriggerFlags.doHypo()
                
            if not TriggerFlags.doFEX() and TriggerFlags.doHypo():
                TrigSteer_EF = ReruningAfterL2TrigSteer_EF('TrigSteer_EF', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())

            # if not TriggerFlags.writeBS():
            from TrigEDMConfig.TriggerEDM import getEFPreregistrationList, getEDMLibraries
            TrigSteer_EF.Navigation.ClassesToPreregister = getEFPreregistrationList()

            TrigSteer_EF.Navigation.Dlls = getEDMLibraries()

            monitoringTools(TrigSteer_EF)
            topSequence += TrigSteer_EF

            
            ### merged system
        if TriggerFlags.doHLT():
            log.info("configuring merged HLT")
           
            from TrigSteering.TrigSteeringConfig import TrigSteer_HLT, ReruningTrigSteer_HLT
            #, ReruningAfterL2TrigSteer_EF
            if TriggerFlags.doFEX():
                log.info("configuring HLT merged system, for normal running (FEX + Hypo)")
                TrigSteer_HLT = TrigSteer_HLT('TrigSteer_HLT', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())
                TrigSteer_HLT.doHypo = TriggerFlags.doHypo()
 

            if not TriggerFlags.doFEX() and TriggerFlags.doHypo():
                log.info("configuring merged HLT for re-running (Hypo only)")
                TrigSteer_HLT = ReruningTrigSteer_HLT('TrigSteer_HLT', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())             
                #            if not TriggerFlags.writeBS():
            from TrigEDMConfig.TriggerEDM import getL2PreregistrationList, getEFPreregistrationList, getHLTPreregistrationList, getEDMLibraries
            TrigSteer_HLT.Navigation.ClassesToPreregister = list(set(getL2PreregistrationList() + getEFPreregistrationList() + getHLTPreregistrationList())) 
            TrigSteer_HLT.Navigation.Dlls = getEDMLibraries()                       

            monitoringTools(TrigSteer_HLT)
            topSequence += TrigSteer_HLT


        if TriggerFlags.writeBS():            
            # declare objects to go to BS (from the lists above)
            if TriggerFlags.doLVL2():                
                from TrigEDMConfig.TriggerEDM import getL2BSList
                TrigSteer_L2.Navigation.ClassesToPayload = getL2BSList()
                TrigSteer_L2.Navigation.ClassesToPreregister = []
            
            if TriggerFlags.doEF():
                from TrigEDMConfig.TriggerEDM import getEFBSList
                TrigSteer_EF.Navigation.ClassesToPayload = getEFBSList()
                TrigSteer_EF.Navigation.ClassesToPreregister = []
                try:
                    from TrigEDMConfig.TriggerEDM import getEFDSList
                    TrigSteer_EF.Navigation.ClassesToPayload_DSonly = getEFDSList()
                except ImportError:
                    log.warning("DataScouting not available in this release")

                ### merged system
            if TriggerFlags.doHLT():
                from TrigEDMConfig.TriggerEDM import getL2BSList, getEFBSList, getHLTBSList
                TrigSteer_HLT.Navigation.ClassesToPayload = list(set(getL2BSList() + getEFBSList() + getHLTBSList())) 
                TrigSteer_HLT.Navigation.ClassesToPreregister = []
                try:
                    from TrigEDMConfig.TriggerEDM import getEFDSList,getHLTDSList
                    TrigSteer_HLT.Navigation.ClassesToPayload_DSonly = getEFDSList() + getHLTDSList()
                except ImportError:
                    log.warning("DataScouting not available in this release")
                

            #configure tools for serialization
            from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
            TrigSerToolTP = TrigSerTPTool('TrigSerTPTool')
            from TrigEDMConfig.TriggerEDM import getTPList
            TrigSerToolTP.TPMap = getTPList()
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += TrigSerToolTP

            from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
            TrigSerializeConvHelper = TrigSerializeConvHelper(doTP = True)
            ToolSvc += TrigSerializeConvHelper

            #do not activate T/P of EF classes at L2
            if TriggerFlags.doLVL2(): 
                from TrigEDMConfig.TriggerEDM import getL2BSTypeList
                TrigSerToolTP.ActiveClasses = getL2BSTypeList()
            if TriggerFlags.doEF():
                from TrigEDMConfig.TriggerEDM import getL2BSTypeList, getEFBSTypeList
                TrigSerToolTP.ActiveClasses = getL2BSTypeList() + getEFBSTypeList()

            ### merged system
            if TriggerFlags.doHLT():
                from TrigEDMConfig.TriggerEDM import getL2BSTypeList, getEFBSTypeList,getHLTBSTypeList
                TrigSerToolTP.ActiveClasses = list(set(getL2BSTypeList() + getEFBSTypeList() + getHLTBSTypeList())) 

        from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
        result = HLTTriggerResultGetter()
        return True


         
