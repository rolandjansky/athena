# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFex
#from TrigBjetHypo.TrigBtagFexTuning import *

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import mm, GeV

from AthenaCommon.AppMgr import ToolSvc
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration import getConfiguration
BTagConfig = getConfiguration("Trig")
BTagConfig.PrefixxAODBaseName(False)
BTagConfig.PrefixVertexFinderxAODBaseName(False)
BTagConfigSetupStatus = BTagConfig.setupJetBTaggerTool(ToolSvc, "AntiKt4EMTopo", SetupScheme="Trig", TaggerList=BTaggingFlags.TriggerTaggers)


def getBtagFexInstance( instance, version, algo ):
    if instance=="EF" :
        return BtagFex( instance=instance, version=version, algo=algo, name="EFBtagFex_"+algo )

def getBtagFexSplitInstance( instance, version, algo):
    return BtagFexSplit( instance=instance, version=version, algo=algo, name=instance+"BtagFexSplit_"+algo )

def getBtagFexFTKInstance( instance, version, algo):
    return BtagFexFTK( instance=instance, version=version, algo=algo, name=instance+"BtagFexFTK_"+algo )

def getBtagFexFTKRefitInstance( instance, version, algo):
    return BtagFexFTKRefit( instance=instance, version=version, algo=algo, name=instance+"BtagFexFTKRefit_"+algo )

def getBtagFexFTKVtxInstance( instance, version, algo):
    return BtagFexFTKVtx( instance=instance, version=version, algo=algo, name=instance+"BtagFexFTKVtx_"+algo )

class BtagFex (TrigBtagFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFex, self ).__init__( name )
        
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
            if BTagConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")

        # MONITORING
        from TrigBjetHypo.TrigBtagFexMonitoring import TrigEFBtagFexValidationMonitoring, TrigEFBtagFexOnlineMonitoring
        validation = TrigEFBtagFexValidationMonitoring()
        online     = TrigEFBtagFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]


###########################################
# For split configuration
###########################################

class BtagFexSplit (TrigBtagFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFexSplit, self ).__init__( name )
        
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
        self.UsePriVtxKeyBackup = True
        self.PriVtxKeyBackup = "EFHistoPrmVtx"
        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
        
        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            if BTagConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        from TrigBjetHypo.TrigBtagFexMonitoring import TrigEFBtagFexValidationMonitoring, TrigEFBtagFexOnlineMonitoring
        validation = TrigEFBtagFexValidationMonitoring()
        online     = TrigEFBtagFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]


###########################################
# For FTK configuration
###########################################

class BtagFexFTK (TrigBtagFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFexFTK, self ).__init__( name )
        
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
            if BTagConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        from TrigBjetHypo.TrigBtagFexMonitoring import TrigEFBtagFexValidationMonitoring, TrigEFBtagFexOnlineMonitoring
        validation = TrigEFBtagFexValidationMonitoring()
        online     = TrigEFBtagFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]


###########################################
# For FTKRefit configuration
###########################################

class BtagFexFTKRefit (TrigBtagFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFexFTKRefit, self ).__init__( name )
        
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
            if BTagConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        from TrigBjetHypo.TrigBtagFexMonitoring import TrigEFBtagFexValidationMonitoring, TrigEFBtagFexOnlineMonitoring
        validation = TrigEFBtagFexValidationMonitoring()
        online     = TrigEFBtagFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]


###########################################
# For FTKVtx configuration
###########################################

class BtagFexFTKVtx (TrigBtagFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFexFTKVtx, self ).__init__( name )
        
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
            if BTagConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")
                
        # MONITORING
        from TrigBjetHypo.TrigBtagFexMonitoring import TrigEFBtagFexValidationMonitoring, TrigEFBtagFexOnlineMonitoring
        validation = TrigEFBtagFexValidationMonitoring()
        online     = TrigEFBtagFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]
