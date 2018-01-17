# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigGSCFex
#from TrigBjetHypo.TrigGSCFexTuning import *

from AthenaCommon.Logging import logging
#from AthenaCommon.SystemOfUnits import mm, GeV

from AthenaCommon.AppMgr import ToolSvc
#from BTagging.BTaggingFlags import BTaggingFlags
#from BTagging.BTaggingConfiguration import getConfiguration
#BTagConfig = getConfiguration("Trig")
#BTagConfig.PrefixxAODBaseName(False)
#BTagConfig.PrefixVertexFinderxAODBaseName(False)
#BTagConfigSetupStatus = BTagConfig.setupJetBTaggerTool(ToolSvc, "AntiKt4EMTopo", SetupScheme="Trig", TaggerList=BTaggingFlags.TriggerTaggers)
from JetRec.JetRecCalibrationFinder import jrcf
JetConfigSetupStatus = True

# johns hack
from JetCalibTools.JetCalibToolsConf import JetCalibrationTool

def getGSCFexInstance( instance, version, algo ):
    if instance=="EF" :
        return GSCFex( instance=instance, version=version, algo=algo, name="EFGSCFex_"+algo )

def getGSCFexSplitInstance( instance, version, algo):
    return GSCFexSplit( instance=instance, version=version, algo=algo, name=instance+"GSCFexSplit_"+algo )

def getGSCFexFTKInstance( instance, version, algo):
    return GSCFexFTK( instance=instance, version=version, algo=algo, name=instance+"GSCFexFTK_"+algo )

def getGSCFexFTKRefitInstance( instance, version, algo):
    return GSCFexFTKRefit( instance=instance, version=version, algo=algo, name=instance+"GSCFexFTKRefit_"+algo )

def getGSCFexFTKVtxInstance( instance, version, algo):
    return GSCFexFTKVtx( instance=instance, version=version, algo=algo, name=instance+"GSCFexFTKVtx_"+algo )

class GSCFex (TrigGSCFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( GSCFex, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        self.JetKey = ""       
        self.PriVtxKey = "EFHistoPrmVtx"
        self.TrackKey  = ""

        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            if JetConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.GSCCalibrationTool = jrcf.find("AntiKt", 0.4, "EMTopo", "ajg", "reco", "Kt4")
                print self.GSCCalibrationTool
        # MONITORING
        #from TrigBjetHypo.TrigGSCFexMonitoring import TrigEFGSCFexValidationMonitoring, TrigEFGSCFexOnlineMonitoring
        #validation = TrigEFGSCFexValidationMonitoring()
        #online     = TrigEFGSCFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        #self.AthenaMonTools = [ time, validation, online ]
        self.AthenaMonTools = [ time  ]


###########################################
# For split configuration
###########################################

class GSCFexSplit (TrigGSCFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( GSCFexSplit, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"
        
        self.PriVtxKey = "xPrimVx" #"EFHistoPrmVtx"
        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
        
        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            if JetConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                #self.GSCCalibrationTool = jrcf.find("AntiKt", 0.4, "EMTopo", "ajg", "reco", "Kt4")
                #print self.GSCCalibrationTool
                myGSCTool = JetCalibrationTool("myJCTool_trigger", 
                                               JetCollection="AntiKt4EMTopo", 
                                               ConfigFile="JES_data2016_data2015_Recommendation_Dec2016_rel21.config", 
                                               CalibSequence="JetArea_EtaJES_GSC_Insitu",
                                               RhoKey="HLTKt4EMTopoEventShape",
                                               IsData=True,
                                               #DoTrigger = True
                                               )
                from AthenaCommon.AppMgr import ToolSvc
                ToolSvc += myGSCTool
                self.GSCCalibrationTool = myGSCTool
                print "Printing GSCCalibrationTool"
                print self.GSCCalibrationTool                
                #JetCalibrationTool("myJCTool_trigger",
                #   IsData=True,
                #   ConfigFile="JES_2015dataset_recommendation_Feb2016.config",
                #   CalibSequence="JetArea_EtaJES_GSC",
                #   JetCollection="AntiKt4EMTopo")


        # MONITORING
        #from TrigBjetHypo.TrigGSCFexMonitoring import TrigEFGSCFexValidationMonitoring, TrigEFGSCFexOnlineMonitoring
        #validation = TrigEFGSCFexValidationMonitoring()
        #online     = TrigEFGSCFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        #self.AthenaMonTools = [ time, validation, online ]
        self.AthenaMonTools = [ time ]


###########################################
# For FTK configuration
###########################################

class GSCFexFTK (TrigGSCFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( GSCFexFTK, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"
        
        self.PriVtxKey = "PrimVertexFTK" #"EFHistoPrmVtx"
        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig"
        
        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            # Will set up GSC here
            pass
            #if BTagConfigSetupStatus == None :
            #    self.setupOfflineTools = False
            #else :
            #    self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
            #    self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
            #    self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
            #    self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        #from TrigBjetHypo.TrigGSCFexMonitoring import TrigEFGSCFexValidationMonitoring, TrigEFGSCFexOnlineMonitoring
        #validation = TrigEFGSCFexValidationMonitoring()
        #online     = TrigEFGSCFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        #self.AthenaMonTools = [ time, validation, online ]
        self.AthenaMonTools = [ time ]


###########################################
# For FTKRefit configuration
###########################################

class GSCFexFTKRefit (TrigGSCFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( GSCFexFTKRefit, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"
        
        self.PriVtxKey = "PrimVertexFTK" #"EFHistoPrmVtx"
        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig"
        
        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            # Will set up GSC here
            pass
            #if BTagConfigSetupStatus == None :
            #    self.setupOfflineTools = False
            #else :
            #    self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
            #    self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
            #    self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
            #    self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        #from TrigBjetHypo.TrigGSCFexMonitoring import TrigEFGSCFexValidationMonitoring, TrigEFGSCFexOnlineMonitoring
        #validation = TrigEFGSCFexValidationMonitoring()
        #online     = TrigEFGSCFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        #self.AthenaMonTools = [ time, validation, online ]
        self.AthenaMonTools = [ time ]


###########################################
# For FTKVtx configuration
###########################################

class GSCFexFTKVtx (TrigGSCFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( GSCFexFTKVtx, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"
        
        self.PriVtxKey = "PrimVertexFTK" #"EFHistoPrmVtx"
        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
        
        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            # Will set up GSC here
            pass
            #if BTagConfigSetupStatus == None :
            #    self.setupOfflineTools = False
            #else :
            #    self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
            #    self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
            #    self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
            #    self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        #from TrigBjetHypo.TrigGSCFexMonitoring import TrigEFGSCFexValidationMonitoring, TrigEFGSCFexOnlineMonitoring
        #validation = TrigEFGSCFexValidationMonitoring()
        #online     = TrigEFGSCFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        #self.AthenaMonTools = [ time, validation, online ]
        self.AthenaMonTools = [ time ]
