# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# TrigEgammaPidTools
# Add all pid selectors to ToolSvc
# Using versioned configuration -- over rides the configuration defined in mapping
# Conf files should be moved to $CALIBPATH and seperated into directories
#
# Trigger specific interface methods have been removed 
# Behavior now set via properties
# Results is multiple instances of tools to apply calo-only and trigger threshold
###############################################################

from PATCore.HelperUtils import *
from AthenaCommon import CfgMgr
import sys
import PyCintex

PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
from ROOT import egammaPID

# Include electron menus for LH and Cut-based
from ElectronPhotonSelectorTools.ElectronLikelihoodToolMapping import electronLHmenu
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
from ElectronPhotonSelectorTools.PhotonIsEMSelectorMapping import photonPIDmenu
from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefPhoton

# Path for versioned configuration
ConfigFilePath = "ElectronPhotonSelectorTools/trigger/dc14b_20150121/"

# Dictionaries for ToolNames
ElectronToolName = {'vloose':'AsgElectronIsEMVLooseSelector',
    'loose':'AsgElectronIsEMLooseSelector',
    'medium':'AsgElectronIsEMMediumSelector',
    'tight':'AsgElectronIsEMTightSelector',
    'loose1':'AsgElectronIsEMLoose1Selector',
    'medium1':'AsgElectronIsEMMedium1Selector',
    'tight1':'AsgElectronIsEMTight1Selector',
    'lhvloose':'AsgElectronLHVLooseSelector',
    'lhloose':'AsgElectronLHLooseSelector',
    'lhmedium':'AsgElectronLHMediumSelector',
    'lhtight':'AsgElectronLHTightSelector',}

ElectronHypoToolName = {'vloose':'AsgElectronIsEMVLooseHypoSelector',
    'loose':'AsgElectronIsEMLooseHypoSelector',
    'medium':'AsgElectronIsEMMediumHypoSelector',
    'tight':'AsgElectronIsEMTightHypoSelector',
    'loose1':'AsgElectronIsEMLoose1HypoSelector',
    'medium1':'AsgElectronIsEMMedium1HypoSelector',
    'tight1':'AsgElectronIsEMTight1HypoSelector',
    'lhvloose':'AsgElectronLHVLooseSelector',
    'lhloose':'AsgElectronLHLooseSelector',
    'lhmedium':'AsgElectronLHMediumSelector',
    'lhtight':'AsgElectronLHTightSelector',}

ElectronCaloToolName = {'vloose':'AsgElectronIsEMVLooseCaloSelector',
    'loose':'AsgElectronIsEMLooseCaloSelector',
    'medium':'AsgElectronIsEMMediumCaloSelector',
    'tight':'AsgElectronIsEMTightCaloSelector',
    'loose1':'AsgElectronIsEMLoose1CaloSelector',
    'medium1':'AsgElectronIsEMMedium1CaloSelector',
    'tight1':'AsgElectronIsEMTight1CaloSelector',
    'lhvloose':'AsgElectronLHVLooseCaloSelector',
    'lhloose':'AsgElectronLHLooseCaloSelector',
    'lhmedium':'AsgElectronLHMediumCaloSelector',
    'lhtight':'AsgElectronLHTightCaloSelector',}

ElectronCaloHypoToolName = {'vloose':'AsgElectronIsEMVLooseCaloHypoSelector',
    'loose':'AsgElectronIsEMLooseCaloHypoSelector',
    'medium':'AsgElectronIsEMMediumCaloHypoSelector',
    'tight':'AsgElectronIsEMTightCaloHypoSelector',
    'loose1':'AsgElectronIsEMLoose1CaloHypoSelector',
    'medium1':'AsgElectronIsEMMedium1CaloHypoSelector',
    'tight1':'AsgElectronIsEMTight1CaloHypoSelector',
    'lhvloose':'AsgElectronLHVLooseCaloHypoSelector',
    'lhloose':'AsgElectronLHLooseCaloHypoSelector',
    'lhmedium':'AsgElectronLHMediumCaloHypoSelector',
    'lhtight':'AsgElectronLHTightCaloHypoSelector',}

# Electron LH tools for alignment / commisioning
ElectronLHToolName = {'cutd0dphi':'AsgElectronLHMediumCutD0DphiSelector',
        'nod0':'AsgElectronLHMediumNoD0Selector',
        'nodphi':'AsgElectronLHMediumNoDphiSelector',
        'nodphires':'AsgElectronLHMediumNoDphiResSelector',}

ElectronLHToolConfigFile = {'cutd0dphi':'ElectronLikelihoodMediumTriggerConfig2015_CutD0DphiDeta.conf',
        'nod0':'ElectronLikelihoodMediumTriggerConfig2015_NoD0.conf',
        'nodphi':'ElectronLikelihoodMediumTriggerConfig2015_NoDeta.conf',
        'nodphires':'ElectronLikelihoodMediumTriggerConfig2015_NoDphiRes.conf',}

ElectronToolConfigFile = {'vloose':'ElectronIsEMVLooseSelectorCutDefs.conf', 
    'loose':'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':'ElectronIsEMTightSelectorCutDefs.conf',
    'lhvloose':'ElectronLikelihoodVeryLooseTriggerConfig2015.conf',
    'lhloose':'ElectronLikelihoodLooseTriggerConfig2015.conf',
    'lhmedium':'ElectronLikelihoodMediumTriggerConfig2015.conf',
    'lhtight':'ElectronLikelihoodTightTriggerConfig2015.conf',}

ElectronCaloToolConfigFile = {'vloose':'ElectronIsEMVLooseSelectorCutDefs.conf', 
    'loose':'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':'ElectronIsEMTightSelectorCutDefs.conf',
    'lhvloose':'ElectronLikelihoodEFCaloOnlyVeryLooseConfig2015.conf',
    'lhloose':'ElectronLikelihoodEFCaloOnlyLooseConfig2015.conf',
    'lhmedium':'ElectronLikelihoodEFCaloOnlyMediumConfig2015.conf',
    'lhtight':'ElectronLikelihoodEFCaloOnlyTightConfig2015.conf',}

ElectronIsEMBits = {'vloose':SelectionDefElectron.ElectronLooseHLT,
    'loose':SelectionDefElectron.ElectronLooseHLT,
    'medium':SelectionDefElectron.ElectronMediumHLT,
    'tight':SelectionDefElectron.ElectronTightHLT,
    'loose1':SelectionDefElectron.ElectronLoose1,
    'medium1':SelectionDefElectron.ElectronMedium1,
    'tight1':SelectionDefElectron.ElectronTight1,
    'lhvloose':SelectionDefElectron.ElectronLooseHLT,
    'lhloose':SelectionDefElectron.ElectronLooseHLT,
    'lhmedium':SelectionDefElectron.ElectronLooseHLT,
    'lhtight':SelectionDefElectron.ElectronLooseHLT,}

ElectronPidName = {'vloose':egammaPID.ElectronIDLooseHLT,
    'loose':egammaPID.ElectronIDLooseHLT,
    'medium':egammaPID.ElectronIDMediumHLT,
    'tight':egammaPID.ElectronIDTightHLT,
    'loose1':egammaPID.ElectronIDLoose1,
    'medium1':egammaPID.ElectronIDMedium1,
    'tight1':egammaPID.ElectronIDTight1,}

# Dictionaries for ToolNames
PhotonToolName = {'loose':'AsgPhotonIsEMSelector/AsgPhotonIsEMLooseSelector',
    'medium':'AsgPhotonIsEMSelector/AsgPhotonIsEMMediumSelector',
    'tight':'AsgPhotonIsEMSelector/AsgPhotonIsEMTightSelector',
    'loose1':'AsgElectronIsEMSelector/AsgPhotonIsEMLoose1Selector',
    'medium1':'AsgElectronIsEMSelector/AsgPhotonIsEMMedium1Selector',
    'tight1':'AsgPhotonIsEMSelector/AsgPhotonIsEMTight1Selector',}

PhotonToolConfigFile = {'loose':"PhotonIsEMLooseSelectorCutDefs.conf",
    'medium':"PhotonIsEMMediumSelectorCutDefs.conf",
    'tight':"PhotonIsEMTightSelectorCutDefs.conf",}

PhotonIsEMBits = {'loose':SelectionDefPhoton.PhotonLooseEF,
    'medium':SelectionDefPhoton.PhotonMediumEF,
    'tight':SelectionDefPhoton.PhotonTight,
    'loose1':SelectionDefPhoton.PhotonLooseEF,
    'medium1':SelectionDefPhoton.PhotonMediumEF,
    'tight1':SelectionDefPhoton.PhotonTight,}

def ElectronPidTools():
    from AthenaCommon.AppMgr import ToolSvc
    # Run1 selectors -- added directly to ToolSvc from default config via mapping
    #print '=========== Run1 PID ============'
    for key in ElectronToolName:
        if not ('lh' in key):
            if ('1' in key):
                #print ElectronToolName[key]
                tool=ConfiguredAsgElectronIsEMSelector(ElectronToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                ToolSvc += tool
    
    # Trigger specific Run1 selectors for high pt triggers
    #print '=========== Run1 PID trigger specific ============'
    for key in ElectronHypoToolName:
        if not ('lh' in key):
            if ('1' in key):
                #print ElectronHypoToolName[key]
                tool=ConfiguredAsgElectronIsEMSelector(ElectronHypoToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                tool.trigEtTh = 20000
                ToolSvc += tool
    
    # Calo only Run1 selectors
    #print '=========== Run1 PID Calo============'
    for key in ElectronCaloToolName:
        if not ('lh' in key):
            if ('1' in key):
                #print ElectronCaloToolName[key]
                tool=ConfiguredAsgElectronIsEMSelector(ElectronCaloToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                tool.caloOnly = True
                ToolSvc += tool
    
    # Calo-only Trigger specific with trigEtTh property set  
    #print '=========== Run1 PID Calo trigger specific ============'
    for key in ElectronCaloHypoToolName:
        if not ('lh' in key):
            if ('1' in key):
                #print ElectronCaloHypoToolName[key]
                tool=ConfiguredAsgElectronIsEMSelector(ElectronCaloHypoToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                tool.caloOnly = True
                tool.trigEtTh = 20000
                ToolSvc += tool
    
    # Versioned selectors for Run2
    #print '=========== Run2 PID ============'
    for key in ElectronToolName:
        if not('lh' in key):
            if not('1' in key):
                #print ElectronToolName[key]
                #print ElectronToolConfigFile[key]
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                ToolSvc += tool
    
    # Trigger specific Run2 selectors
    #print '=========== Run2 PID trigger specific ============'
    for key in ElectronHypoToolName:
        if not('lh' in key):
            if not('1' in key):
                #print ElectronHypoToolName[key]
                #print ElectronToolConfigFile[key]
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronHypoToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                tool.trigEtTh = 20000
                ToolSvc += tool
    
    # Calo-only selectors
    #print '=========== Run2 PID Calo============'
    for key in ElectronCaloToolName:
        if not('lh' in key):
            if not('1' in key):
                #print ElectronCaloToolName[key]
                #print ElectronCaloToolConfigFile[key]
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronCaloToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                tool.caloOnly = True
                ToolSvc += tool
    
    # Calo-only trigger specific selectors
    #print '=========== Run1 PID Calo trigger specific ============'
    for key in ElectronCaloHypoToolName:
        if not('lh' in key):
            if not('1' in key):
                #print ElectronCaloHypoToolName[key]
                #print ElectronCaloToolConfigFile[key]
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronCaloHypoToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                tool.caloOnly = True
                tool.trigEtTh = 20000
                ToolSvc += tool
   
    # LH selectors
    #print '======== LH selectors ==========='
    for key in ElectronToolName:
        if('lh' in key):
            #print ElectronToolName[key]
            #print ElectronToolConfigFile[key]
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
            tool.inputPDFFileName = ConfigFilePath + "DC14OnlinePDFs.root"
            #tool.inputPDFFileName = "ElectronPhotonSelectorTools/trigger/dc14b_20141031/DC14OnlinePDFs.root"
            tool.usePVContainer = False
            ToolSvc += tool
   
    # Special LH triggers for alignment / commisioning
    for key in ElectronLHToolName:
        #print ElectronLHToolName[key]
        #print ElectronLHToolConfigFile[key]
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHToolConfigFile[key]
        tool.inputPDFFileName = ConfigFilePath + "DC14OnlinePDFs.root"
        tool.usePVContainer = False
        ToolSvc += tool

    # Calo-only LH selectors
    #print '======== LH Calo selectors ==========='
    for key in ElectronCaloToolName:
        if('lh' in key):
            #print ElectronCaloToolName[key]
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronCaloToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
            tool.inputPDFFileName = ConfigFilePath + "DC14OnlinePDFs.root"
            tool.usePVContainer = False
            tool.caloOnly = True
            ToolSvc += tool
    
def PhotonPidTools():
    from AthenaCommon.AppMgr import ToolSvc
    PhotonLooseSel=ConfiguredAsgPhotonIsEMSelector("AsgPhotonIsEMLooseSelector",egammaPID.PhotonIDLooseEF,photonPIDmenu.menuTrigDC14)
    PhotonLooseSel.ConfigFile = ConfigFilePath + PhotonToolConfigFile['loose']
    PhotonLooseSel.ForceConvertedPhotonPID = True
    PhotonMediumSel=ConfiguredAsgPhotonIsEMSelector("AsgPhotonIsEMMediumSelector",egammaPID.PhotonIDLooseEF,photonPIDmenu.menuTrigDC14)                   
    PhotonMediumSel.ConfigFile =ConfigFilePath + PhotonToolConfigFile['medium'] 
    PhotonMediumSel.ForceConvertedPhotonPID = True
    PhotonLoose1Sel=ConfiguredAsgElectronIsEMSelector("AsgPhotonIsEMLoose1Selector",egammaPID.PhotonIDLooseEF,electronPIDmenu.menuTrig2012)
    PhotonLoose1Sel.caloOnly = True
    PhotonMedium1Sel=ConfiguredAsgElectronIsEMSelector("AsgPhotonIsEMMedium1Selector",egammaPID.PhotonIDMediumEF,electronPIDmenu.menuTrig2012)                   
    PhotonMedium1Sel.caloOnly = True
    PhotonTight1Sel=ConfiguredAsgPhotonIsEMSelector("AsgPhotonIsEMTight1Selector",egammaPID.PhotonIDTight)                    
    # Conf file fixed since this is Run1 tune
    PhotonTight1Sel.ConfigFile = 'ElectronPhotonSelectorTools/dc14b_20141031/PhotonIsEMTight8TeVSelectorCutDefs.conf'   
    PhotonTight1Sel.ForceConvertedPhotonPID = True
    
    PhotonTightSel=ConfiguredAsgPhotonIsEMSelector("AsgPhotonIsEMTightSelector",egammaPID.PhotonIDTight,photonPIDmenu.menuTrigDC14)                    
    PhotonTightSel.ConfigFile = ConfigFilePath + PhotonToolConfigFile['tight'] 
    PhotonTightSel.ForceConvertedPhotonPID = True
    
    ToolSvc += PhotonTightSel
    ToolSvc += PhotonLooseSel
    ToolSvc += PhotonMediumSel
    ToolSvc += PhotonLoose1Sel
    ToolSvc += PhotonMedium1Sel
    ToolSvc += PhotonTight1Sel
                       
                
                
                
