# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigGSCFexMT
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

def getGSCFexInstance( theName, instance, version, algo ):
    if instance=="EF" :
        return GSCFex( instance=instance, version=version, algo=algo, name=theName )

def getGSCFexSplitInstance( theName,instance, version, algo):
    return GSCFexSplit( instance=instance, version=version, algo=algo, name=theName )


class GSCFex (TrigGSCFexMT):
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
        
        self.JetKey = "EFJet"       
        self.PriVtxKey = "EFHistoPrmVtx"
        self.TrackKey  = "InDetTrigTrackingxAODCnv_Bjet_EFID"

        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = True
        if self.setupOfflineTools :
            if JetConfigSetupStatus == None :
                self.setupOfflineTools = False
            else :
                self.JetGSCCalibrationTool = jrcf.find("AntiKt", 0.4, "EMTopo", "ajg", "reco", "Kt4")
                print self.JetGSCCalibrationTool


###########################################
# For split configuration
###########################################

class GSCFexSplit (TrigGSCFexMT):
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
                self.JetGSCCalibrationTool = myGSCTool
                print "Printing GSCCalibrationTool"
                print self.JetGSCCalibrationTool
                #JetCalibrationTool("myJCTool_trigger",
                #   IsData=True,
                #   ConfigFile="JES_2015dataset_recommendation_Feb2016.config",
                #   CalibSequence="JetArea_EtaJES_GSC",
                #   JetCollection="AntiKt4EMTopo")

