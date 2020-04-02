# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFexMT

from AthenaCommon.Logging import logging

from AthenaCommon.AppMgr import ToolSvc
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration import getConfiguration

#from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

BTagConfig = getConfiguration("Trig")
BTagConfig.PrefixxAODBaseName(False)
BTagConfig.PrefixVertexFinderxAODBaseName(False)
BTagConfigSetupStatus = BTagConfig.setupJetBTaggerTool(ToolSvc, "AntiKt4EMTopo", SetupScheme="Trig", TaggerList=BTaggingFlags.TriggerTaggers)


def getBtagFexInstance( instance, version, algo ):
    if instance=="EF" :
        return BtagFex( instance=instance, version=version, algo=algo, name="EFBtagFex_"+algo )

def getBtagFexSplitInstance( instance, version, algo):
    return BtagFexSplit( instance=instance, version=version, algo=algo, name=instance+"BtagFexSplit_"+algo )


class TrigBtagFexMT_OnlineMonitoring(GenericMonitoringTool):
    def __init__ (self, name="TrigBtagFexMT_OnlineMonitoring"):
        super(TrigBtagFexMT_OnlineMonitoring, self).__init__(name)
        self.name = "TrigBtagFexMT_OnlineMonitoring"
        self.HistPath = self.name
        default_bin_count = 20
        #self.defineTarget("Online")

        # Event Histograms
        self.defineHistogram('jet_count', path='EXPERT', type='TH1F', title="BtagFexMT Number of Jets", xbins = 20, xmin=0, xmax=20) 
        self.defineHistogram('track_count', path='EXPERT', type='TH1F', title="BtagFexMT Number of Tracks", xbins = 50, xmin=0, xmax=50) 
        self.defineHistogram('vertex_count', path='EXPERT', type='TH1F', title="BtagFexMT Number of Vertices", xbins = 50, xmin=0, xmax=50) 

        # Jet Histograms
        self.defineHistogram('jet_pt', path='EXPERT', type='TH1F', title="BtagFexMT Jet PT", xbins = default_bin_count, xmin=0, xmax=800000) 
        self.defineHistogram('jet_eta', path='EXPERT', type='TH1F', title="BtagFexMT Jet Eta", xbins = default_bin_count, xmin=-5, xmax=5) 

        # Track Histograms
        self.defineHistogram('track_Et', path='EXPERT', type='TH1F', title="BtagFexMT Track Transverse Energy", xbins = default_bin_count, xmin=0, xmax=800000) 
        self.defineHistogram('track_eta', path='EXPERT', type='TH1F', title="BtagFexMT Track Eta", xbins = default_bin_count, xmin=-5, xmax=5) 
        self.defineHistogram('track_phi', path='EXPERT', type='TH1F', title="BtagFexMT Track Phi", xbins = default_bin_count, xmin=-4, xmax=4) 
        self.defineHistogram('track_eta,track_phi', path='EXPERT', type='TH2F', title="BtagFexMT Track Eta vs Phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4) 
        self.defineHistogram('track_d0', path='EXPERT', type='TH1F', title="BtagFexMT Track D0", xbins = default_bin_count, xmin=0, xmax=10) 
        self.defineHistogram('track_d0err', path='EXPERT', type='TH1F', title="BtagFexMT Track D0 Error", xbins = default_bin_count, xmin=0, xmax=10) 
        self.defineHistogram('track_d0sig', path='EXPERT', type='TH1F', title="BtagFexMT Track D0 Significance", xbins = default_bin_count, xmin=0, xmax=1000) 
        self.defineHistogram('track_z0', path='EXPERT', type='TH1F', title="BtagFexMT Track Z0", xbins = default_bin_count, xmin=0, xmax=100) 
        self.defineHistogram('track_z0err', path='EXPERT', type='TH1F', title="BtagFexMT Track Z0 Error", xbins = default_bin_count, xmin=0, xmax=10) 
        self.defineHistogram('track_z0sig', path='EXPERT', type='TH1F', title="BtagFexMT Track Z0 Significance", xbins = default_bin_count, xmin=0, xmax=10000) 


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


