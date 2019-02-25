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

    #try:
    #    Set = set
    #except NameError:
    #    from sets import Set
    setOfEnabled = set(TriggerFlags.enableMonitoring()) # this is set of enabled monitoring targets

    log.info("requested targets: "+str(setOfEnabled)+" pruning other tools")
    # prune subalgorithms
    for alg in steering.getChildren():
        tokeep = [ x for x in alg.AthenaMonTools if len(set(x.target()) & setOfEnabled) != 0 ]
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

    def __init__(self, menu):
        Configured.__init__(self)
        self.postConfigure(menu)

    def configure(self):
        
        log = logging.getLogger("HLTTriggergetter.py")

        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()

        #scheduling eventinfo
        from RecExConfig.ObjKeyStore import objKeyStore
        if ( not objKeyStore.isInInput( "xAOD::EventInfo") ) and ( not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ) ):
            from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
            topSequence += xAODMaker__EventInfoCnvAlg()
	        

        log.info("Loading RegionSelector")
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += RegSelSvcDefault()

        # Configure the Data Preparation for Calo
        if TriggerFlags.doCalo():
            try:
                from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigDataAccess
                ServiceMgr.ToolSvc += TrigDataAccess()
            except ImportError:
                include('TrigT2CaloCommon/TrigDataAccessConfigured.py')
        
        if TriggerFlags.doFTK():
            # FTK algorithm inclusions
            # TrigFTK_DataProviderSvc moved to TrigFTK_RecExample
            pass
            
        if TriggerFlags.doHLT():
            log.info("configuring HLT Steering")
            
            from TrigSteering.TrigSteeringConfig import TrigSteer_HLT, ReruningTrigSteer_HLT
            if TriggerFlags.doFEX():
                from RecExConfig.RecFlags  import rec
                from AthenaCommon.GlobalFlags  import globalflags

                # schedule the conversion of the L1Calo ROIB data to topo simulation input

                # this ROIB converter should only be scheduled if
                # - we run on data
                # - L1Calo simulation has not been scheduled
                # - and ROIB converter has not been scheduled yet before
                if globalflags.DataSource()=='data' \
                       and not hasattr( topSequence, 'CPCMX' ) \
                       and not hasattr( topSequence, 'RoiB2TopoInputDataCnv' ):
                    log.info("Setting up RoiB2TopoInputDataCnv")
                    from L1TopoSimulation.L1TopoSimulationConfig import RoiB2TopoInputDataCnv
                    roib2Topo = RoiB2TopoInputDataCnv()
                    topSequence += roib2Topo

                log.info("configuring HLT merged system, for normal running (FEX + Hypo)")
                TrigSteer_HLT = TrigSteer_HLT('TrigSteer_HLT', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())
                TrigSteer_HLT.doHypo = TriggerFlags.doHypo()

            if not TriggerFlags.doFEX() and TriggerFlags.doHypo():
                log.info("configuring merged HLT for re-running (Hypo only)")
                TrigSteer_HLT = ReruningTrigSteer_HLT('TrigSteer_HLT', hltFile=TriggerFlags.inputHLTconfigFile(), lvl1File=TriggerFlags.inputLVL1configFile())             
                
            # TrigSteer_HLT.doL1TopoSimulation = TriggerFlags.doL1Topo() # this later needs to be extented to also run when we take data with L1Topo
            TrigSteer_HLT.doL1TopoSimulation = True # always needs to run if the HLT is simulated
            if hasattr(TrigSteer_HLT.LvlTopoConverter, 'MuonInputProvider'):
                print "TrigSteer_HLT.LvlTopoConverter has attribute MuonInputProvider"

                try: # this is temporary until TrigT1Muctpi-00-06-29 is in the release
                    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
                    from AthenaCommon.AppMgr import ToolSvc
                    ToolSvc += L1MuctpiTool()
                    TrigSteer_HLT.LvlTopoConverter.MuonInputProvider.MuctpiSimTool = L1MuctpiTool()
                except ImportError:
                    pass

                from AthenaCommon.GlobalFlags  import globalflags
                if globalflags.DataSource()!='data':
                    log.info("Muon eta/phi encoding with reduced granularity for MC (L1Topo emulation for HLT seeding)")
                    TrigSteer_HLT.LvlTopoConverter.MuonInputProvider.MuonEncoding = 1 # reduced granularity muon input
                else:
                    log.info("Muon eta/phi encoding with full granularity for data (L1Topo emulation for HLT seeding) - should be faced out")
                    TrigSteer_HLT.LvlTopoConverter.MuonInputProvider.MuonEncoding = 0 # full granularity muon input - should be faced out

            from TrigEDMConfig.TriggerEDM import  getHLTPreregistrationList, getEDMLibraries
            TrigSteer_HLT.Navigation.ClassesToPreregister = getHLTPreregistrationList()
            
            TrigSteer_HLT.Navigation.Dlls = getEDMLibraries()

            monitoringTools(TrigSteer_HLT)
            topSequence += TrigSteer_HLT


        if TriggerFlags.writeBS():
            # declare objects to go to BS (from the lists above)
            ## if TriggerFlags.doLVL2():                
            ##     from TrigEDMConfig.TriggerEDM import getL2BSList
            ##     TrigSteer_L2.Navigation.ClassesToPayload = getL2BSList()
            ##     TrigSteer_L2.Navigation.ClassesToPreregister = []
            ## 
            ## if TriggerFlags.doEF():
            ##     from TrigEDMConfig.TriggerEDM import getEFBSList
            ##     TrigSteer_EF.Navigation.ClassesToPayload = getEFBSList()
            ##     TrigSteer_EF.Navigation.ClassesToPreregister = []
            ##     try:
            ##         from TrigEDMConfig.TriggerEDM import getEFDSList
            ##         TrigSteer_EF.Navigation.ClassesToPayload_DSonly = getEFDSList()
            ##     except ImportError:
            ##         log.warning("DataScouting not available in this release")

            if TriggerFlags.doHLT():
                from TrigEDMConfig.TriggerEDM import  getHLTBSList 
                TrigSteer_HLT.Navigation.ClassesToPayload = getHLTBSList() 
                TrigSteer_HLT.Navigation.ClassesToPreregister = []
                try:
                    from TrigEDMConfig.TriggerEDM import getHLTDSList 
                    TrigSteer_HLT.Navigation.ClassesToPayload_DSonly = getHLTDSList()
                except ImportError:
                    log.warning("DataScouting not available in this release")
                

            #configure tools for serialization
            from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
            TrigSerToolTP = TrigSerTPTool('TrigSerTPTool')
            from TrigEDMConfig.TriggerEDM import getTPList
            TrigSerToolTP.TPMap = getTPList(TriggerFlags.doHLT()) 
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += TrigSerToolTP

            from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
            TrigSerializeConvHelper = TrigSerializeConvHelper(doTP = True)
            ToolSvc += TrigSerializeConvHelper

            #do not activate T/P of EF classes at L2
            ## if TriggerFlags.doLVL2(): 
            ##     from TrigEDMConfig.TriggerEDM import getL2BSTypeList
            ##     TrigSerToolTP.ActiveClasses = getL2BSTypeList()
            ## if TriggerFlags.doEF():
            ##     from TrigEDMConfig.TriggerEDM import getL2BSTypeList, getEFBSTypeList
            ##     TrigSerToolTP.ActiveClasses = getL2BSTypeList() + getEFBSTypeList()

            if TriggerFlags.doHLT():
                from TrigEDMConfig.TriggerEDM import getHLTBSTypeList 
                TrigSerToolTP.ActiveClasses = getHLTBSTypeList()

        from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
        result = HLTTriggerResultGetter()
        return True


    def postConfigure(self, menu):
        log = logging.getLogger("HLTTriggergetter.py")
        log.info("postConfigure: update steering configuration from the menu")

        from TriggerMenu.menu.GenerateMenu import GenerateMenu
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()            
        if not hasattr( topSequence, 'TrigSteer_HLT' ):
            log.error("TrigSteer not found, postConfigure is not executed")
            return False
        
        # define the chains that can add streamtags in Rerun:
        topSequence.TrigSteer_HLT.EventInfoAccessTool.ListOfChainsAddingStreamTag=menu.GetStreamTagForRerunChains()

        return True

         
