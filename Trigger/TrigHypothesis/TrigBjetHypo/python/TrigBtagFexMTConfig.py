# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFexMT

from AthenaCommon.Logging import logging

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




class BtagFex (TrigBtagFexMT):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFex, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF"]
        AllowedVersions  = ["2012"]

        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        self.JetKey = "GSCJets"
        self.PriVtxKey = "EFHistoPrmVtx"
#        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_EFID"

#        # IMPORT OFFLINE TOOLS
#        self.setupOfflineTools = True
#        if self.setupOfflineTools :
#            if BTagConfigSetupStatus == None :
#                self.setupOfflineTools = False
#            else :
#                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
#                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
#                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
#                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")


###########################################
# For split configuration
###########################################

class BtagFexSplit (TrigBtagFexMT):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFexSplit, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]

        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "GSCJets"
        self.PriVtxKey = "EFHistoPrmVtx"
#        self.UsePriVtxKeyBackup = True
#        self.PriVtxKeyBackup = "EFHistoPrmVtx"
#        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
        
#        # IMPORT OFFLINE TOOLS
#        self.setupOfflineTools = True
#        if self.setupOfflineTools :
#            if BTagConfigSetupStatus == None :
#                self.setupOfflineTools = False
#            else :
#                self.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
#                self.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
#                self.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
#                self.TaggerBaseNames    = BTagConfig.getJetCollectionSecVertexingToolAttribute("SecVtxFinderxAODBaseNameList", "AntiKt4EMTopo")


