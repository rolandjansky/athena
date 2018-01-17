# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -------------------------------------------------------------
# L1 configuration
# -------------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import jobproperties
from CaloRec.CaloCellFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging  # loads logger
from AthenaCommon.Include import include  # to include old style job options
from AthenaCommon.AppMgr import theApp
        
from RecExConfig.Configured import Configured 
from RecExConfig.ObjKeyStore import objKeyStore


def getLvl1OutputLevel():
    """ Default OutputLevel used by tests run from TriggerTest
    set an output level in topOptions via
    Lvl1OutputLevel = DEBUG
    """
    import __main__
    if 'Lvl1OutputLevel' in dir(__main__):
        return __main__.Lvl1OutputLevel
    return WARNING

def LVL1MonitoringTools():
    log = logging.getLogger( "LVL1MonitoringTools" )

    log.debug("Setting up offline THistSvc")
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, 'THistSvc'):        
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()

    if hasattr(ServiceMgr.THistSvc, "Output"): # this is offline THistSvc for which we want to setup files 
        if 1 not in [ o.count('EXPERT') for o in  ServiceMgr.THistSvc.Output ]:
            ServiceMgr.THistSvc.Output += [ "EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]

def keyInInputFile(sgKey):
    """check, if the given StoreGate key is in the current input file
    """
    from RecExConfig.InputFilePeeker import inputFileSummary

    return sgKey in inputFileSummary['eventdata_itemsDic']



class Lvl1SimulationGetter (Configured):

    def configure(self):
        log = logging.getLogger( "Lvl1SimulationGetter" )

        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        from AthenaCommon.AppMgr import ServiceMgr
        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()


        if (not TriggerFlags.fakeLVL1()) and TriggerFlags.doLVL1():

            if TriggerFlags.doCalo():
                if TriggerFlags.useCaloTTL() and not jobproperties.CaloCellFlags.doFastCaloSim():
                    import re
                    if re.search("pp_v[5-9]|HI_v[3-9]|LS1_v[1-9]|DC14", TriggerFlags.triggerMenuSetup() ):
                        log.info("Using L1Calo Run 2 simulation")
                        include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_Run2.py" )
                    else:
                        if TriggerFlags.doCosmicSim():
                            include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1_cosmic.py" )
                        elif TriggerFlags.useL1CaloCalibration():
                            include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1.py")
                        elif 'pp_v4' in TriggerFlags.triggerMenuSetup(): #temporary should be steered by run configuration
                            include ( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1_2012.py") 
                        else:
                            include ( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1_NoCalib.py")
                else:
                    include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_Cell.py")
                    
                log.info("adding MBTS simulation to the topSequence")
                from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
                topSequence += LVL1__TrigT1MBTS()


            # schedule simulation
            if TriggerFlags.doMuon() and (not DetFlags.readRIOPool.LVL1_on() ):
                include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )
                import TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig
                import TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig
                import TrigT1RPCsteering.TrigT1RPCsteeringConfig
                import TrigT1TGC.TrigT1TGCConfig
                from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi                
                topSequence += L1Muctpi()

            if TriggerFlags.doBcm():
                from TrigT1BCM.TrigT1BCMConf import LVL1__TrigT1BCM
                alg = LVL1__TrigT1BCM()
                
                try:
                    sgKey = alg.BcmL1ContainerName
                except AttributeError:
                    sgKey = alg.getDefaultProperty('BcmL1ContainerName')

                if keyInInputFile(sgKey):
                    log.info("adding BCM simulation to the topSequence")
                    topSequence += alg
                else:
                    log.warning("%s input (%s) missing, not adding to the topSequence" % (alg.getName(), sgKey))
                    TriggerFlags.doBcm.set_Value(False)

            if TriggerFlags.doTrt():
                from TrigT1TRT.TrigT1TRTConf import LVL1__TrigT1TRT
                alg = LVL1__TrigT1TRT()
                
                try:
                    sgKey = alg.TRTL1ContainerName
                except AttributeError:
                    sgKey = alg.getDefaultProperty('TRTL1ContainerName')

                if keyInInputFile(sgKey):
                    log.info("adding TRTfast simulation to the topSequence")
                    topSequence += alg
                else:
                    log.warning("%s input (%s) missing, not adding to the topSequence" % (alg.getName(), sgKey))
                    TriggerFlags.doTrt.set_Value(False)

            if TriggerFlags.doZdc():
                from TrigT1ZDC.TrigT1ZDCConf import LVL1__TrigT1ZDC
                alg = LVL1__TrigT1ZDC()

                # ZDC simulation is using truth information at the moment, so do not do any fancy configuration
                
                log.info("adding ZDC simulation to the topSequence")
                topSequence += alg

            if TriggerFlags.doLucid():
                from TrigT1Lucid.TrigT1LucidConf import LVL1__TrigT1Lucid
                alg = LVL1__TrigT1Lucid()

                try:
                    sgKey = alg.LVL1T1ContainerName
                except AttributeError:
                    sgKey = alg.getDefaultProperty('LVL1T1ContainerName')
                
                if keyInInputFile(sgKey):
                    log.info("adding Lucid simulation to the topSequence")
                    topSequence += alg
                else:
                    log.warning("%s input (%s) missing, not adding to the topSequence" % (alg.getName(), sgKey))
                    TriggerFlags.doLucid.set_Value(False)
                    

            if TriggerFlags.doL1Topo():
                log.info("Enabling L1Topo simulation")
                from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
                topSequence += L1TopoSimulation()

                try: # this is temporary until TrigT1Muctpi-00-06-29 is in the release
                    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
                    from AthenaCommon.AppMgr import ToolSvc
                    ToolSvc += L1MuctpiTool()
                    topSequence.L1TopoSimulation.MuonInputProvider.MuctpiSimTool = L1MuctpiTool()
                except ImportError:
                    pass

                # enable the reduced (coarse) granularity topo simulation
                # currently only for MC
                from AthenaCommon.GlobalFlags  import globalflags
                if globalflags.DataSource()!='data':
                    log.info("Muon eta/phi encoding with reduced granularity for MC (L1 Simulation)")
                    topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
                else:
                    log.info("Muon eta/phi encoding with full granularity for data (L1 Simulation) - should be faced out")
                    topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 0


            log.info("adding ctp simulation to the topSequence")
            from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
            topSequence += CTPSimulationInReco("CTPSimulation")
            
            log.info("adding ROIB simulation to the topSequence")
            from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
            topSequence += RoIBuilder("RoIBuilder")

            # monitoring
            LVL1MonitoringTools()

            #jtb if not TriggerFlags.fakeLVL1():
                # LVL1
            #jtb     theApp.Dlls += [ "TrigT1ResultByteStream" ]
   

        if not (TriggerFlags.fakeLVL1() or TriggerFlags.doLVL1()):
            DetFlags.readRIOPool.LVL1_setOn()

            # need to following when reading LVL1 to o/p to AOD
            # RoI reconstruction svc

            #        if ( doWriteESD or readESD) or ( doWriteAOD or readAOD) or doWriteRDO :
            #            include("TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py")
            
            #        if (doWriteESD or doWriteAOD):
            # Pool convertors for LVL1
            #            include("AnalysisTriggerEventAthenaPool/AnalysisTriggerEventAthenaPool_joboptions.py")
 


        # --------------------------------------------------------------
        # digitize the muon detector first if needed
        # --------------------------------------------------------------
        if TriggerFlags.doMuon() and DetFlags.digitize.Muon_on():
    
            # configure Pileup
            theApp.Dlls += [ "PileUpTools" ]
            # RPC Digitization 
            include( "RPC_Digitization/RPC_Digitization_jobOptions.py" )
            # MDT Digitization 
            include( "MDT_Digitization/MDT_Digitization_jobOptions.py" )
            # CSC Digitization 
            include( "CSC_Digitization/CSC_Digitization_jobOptions.py" )
            # TGC Digitization 
            include( "TGC_Digitization/TGC_Digitization_jobOptions.py" )
    	
            include( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )

#        from TriggerJobOpts.Lvl1ResultBuilderGetter import Lvl1ResultBuilderGetter
#        result = Lvl1ResultBuilderGetter()


        # ***Temp : needed to enable required combination of thresholds
        #           mu4,mu6,mu10,mu11,mu20,mu40
        # Will be replaced by config. from Lvl1ConfigSvc
        if hasattr(ServiceMgr,'RPCcablingSimSvc'):
            ServiceMgr.RPCcablingSimSvc.HackFor1031 = True
            
        if  jobproperties.Global.InputFormat() != 'bytestream':
            if TriggerFlags.doCalo() and DetFlags.writeBS.Calo_on():

                from AthenaCommon.AppMgr import ToolSvc
                from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
                ToolSvc+=LArRawDataContByteStreamToolConfig(InitializeForWriting=True)                 

#        if jobproperties.Global.InputFormat() == 'bytestream':   
#            theApp.Dlls += [ "TrigT1Calo" ]   
#            include("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py") 

#        from RecExConfig.RecFlags  import rec
#        if (rec.doESD() or rec.doAOD()) and (not(rec.readAOD() or rec.readESD())): #fix for bug 64345
#            from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToAOD
#            topSequence += RoIBResultToAOD("RoIBResultToAOD")

             
#        from TrigEDMConfig.TriggerEDM import getLvl1ESDList
#        objKeyStore.addManyTypesStreamESD(getLvl1ESDList())
#        from TrigEDMConfig.TriggerEDM import getLvl1AODList
#        objKeyStore.addManyTypesStreamAOD(getLvl1AODList())
        
        return True
