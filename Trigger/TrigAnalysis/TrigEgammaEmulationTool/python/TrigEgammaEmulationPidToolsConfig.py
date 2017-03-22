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
import logging
import sys
import cppyy

cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
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
ConfigFilePath = "ElectronPhotonSelectorTools/trigger/rel21_20170214//"


mlog = logging.getLogger ('TrigEgammaEmulationPidTools')
mlog.info("TrigEgammaPidTools version %s"%ConfigFilePath)

# Dictionaries for ToolNames
ElectronToolName = {'vloose':'AsgElectronIsEMVLooseSelector',
    'loose':'AsgElectronIsEMLooseSelector',
    'medium':'AsgElectronIsEMMediumSelector',
    'tight':'AsgElectronIsEMTightSelector',
    'mergedtight':'AsgElectronIsEMMergedTightSelector',
    'lhvloose':'AsgElectronLHVLooseSelector',
    'lhloose':'AsgElectronLHLooseSelector',
    'lhmedium':'AsgElectronLHMediumSelector',
    'lhtight':'AsgElectronLHTightSelector',}

ElectronCaloToolName = {'vloose':'AsgElectronIsEMVLooseCaloSelector',
    'loose':'AsgElectronIsEMLooseCaloSelector',
    'medium':'AsgElectronIsEMMediumCaloSelector',
    'tight':'AsgElectronIsEMTightCaloSelector',
    'mergedtight':'AsgElectronIsEMMergedTightCaloSelector',
    'lhvloose':'AsgElectronLHVLooseCaloSelector',
    'lhloose':'AsgElectronLHLooseCaloSelector',
    'lhmedium':'AsgElectronLHMediumCaloSelector',
    'lhtight':'AsgElectronLHTightCaloSelector',}

# Electron LH tools for alignment / commisioning
ElectronLHVLooseToolName = {'cutd0dphideta':'AsgElectronLHVeryLooseCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHVeryLooseNoD0Selector',
        'nodeta':'AsgElectronLHVeryLooseNoDetaSelector',
        'nodphires':'AsgElectronLHVeryLooseNoDphiResSelector',}

ElectronLHVLooseToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodVeryLooseTriggerConfig_CutDphiDeta.conf',
        'nod0':'ElectronLikelihoodVeryLooseTriggerConfig.conf',
        'nodeta':'ElectronLikelihoodVeryLooseTriggerConfig_NoDeta.conf',
        'nodphires':'ElectronLikelihoodVeryLooseTriggerConfig_NoDphi.conf',}

ElectronLHLooseToolName = {'cutd0dphideta':'AsgElectronLHLooseCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHLooseNoD0Selector',
        'nodeta':'AsgElectronLHLooseNoDetaSelector',
        'nodphires':'AsgElectronLHLooseNoDphiResSelector',}

ElectronLHLooseToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodLooseTriggerConfig_CutDphiDeta.conf',
        'nod0':'ElectronLikelihoodLooseTriggerConfig.conf',
        'nodeta':'ElectronLikelihoodLooseTriggerConfig_NoDeta.conf',
        'nodphires':'ElectronLikelihoodLooseTriggerConfig_NoDphi.conf',}

ElectronLHMediumToolName = {'cutd0dphideta':'AsgElectronLHMediumCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHMediumNoD0Selector',
        'nodeta':'AsgElectronLHMediumNoDetaSelector',
        'nodphires':'AsgElectronLHMediumNoDphiResSelector',}

ElectronLHMediumToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodMediumTriggerConfig_CutDphiDeta.conf',
        'nod0':'ElectronLikelihoodMediumTriggerConfig.conf',
        'nodeta':'ElectronLikelihoodMediumTriggerConfig_NoDeta.conf',
        'nodphires':'ElectronLikelihoodMediumTriggerConfig_NoDphi.conf',}

ElectronLHTightToolName = {'cutd0dphideta':'AsgElectronLHTightCutD0DphiDetaSelector',
        'nod0':'AsgElectronLHTightNoD0Selector',
        'nodeta':'AsgElectronLHTightNoDetaSelector',
        'nodphires':'AsgElectronLHTightNoDphiResSelector',
        'smooth':'AsgElectronLHTightSmoothSelector'}

ElectronLHTightToolConfigFile = {'cutd0dphideta':'ElectronLikelihoodTightTriggerConfig_CutDphiDeta.conf',
        'nod0':'ElectronLikelihoodTightTriggerConfig.conf',
        'nodeta':'ElectronLikelihoodTightTriggerConfig_NoDeta.conf',
        'nodphires':'ElectronLikelihoodTightTriggerConfig_NoDphi.conf',
        'smooth':'ElectronLikelihoodTightTriggerConfig_Smooth.conf'}

ElectronToolConfigFile = {'vloose':'ElectronIsEMVLooseSelectorCutDefs.conf', 
    'loose':'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':'ElectronIsEMTightSelectorCutDefs.conf',
    'mergedtight':'ElectronIsEMMergedTightSelectorCutDefs.conf',
    'lhvloose':'ElectronLikelihoodVeryLooseTriggerConfig2015.conf',
    'lhloose':'ElectronLikelihoodLooseTriggerConfig2015.conf',
    'lhmedium':'ElectronLikelihoodMediumTriggerConfig2015.conf',
    'lhtight':'ElectronLikelihoodTightTriggerConfig2015.conf',}

ElectronCaloToolConfigFile = {'vloose':'ElectronIsEMVLooseSelectorCutDefs.conf', 
    'loose':'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':'ElectronIsEMTightSelectorCutDefs.conf',
    'mergedtight':'ElectronIsEMMergedTightSelectorCutDefs.conf',
    'lhvloose':'ElectronLikelihoodVeryLooseTriggerConfig_CaloOnly.conf',
    'lhloose':'ElectronLikelihoodLooseTriggerConfig_CaloOnly.conf',
    'lhmedium':'ElectronLikelihoodMediumTriggerConfig_CaloOnly.conf',
    'lhtight':'ElectronLikelihoodTightTriggerConfig_CaloOnly.conf',}

ElectronIsEMBits = {'vloose':SelectionDefElectron.ElectronLooseHLT,
    'loose':SelectionDefElectron.ElectronLooseHLT,
    'medium':SelectionDefElectron.ElectronMediumHLT,
    'tight':SelectionDefElectron.ElectronTightHLT,
    'mergedtight':SelectionDefElectron.ElectronTightHLT, #Can define separate mask
    'lhvloose':SelectionDefElectron.ElectronLooseHLT,
    'lhloose':SelectionDefElectron.ElectronLooseHLT,
    'lhmedium':SelectionDefElectron.ElectronLooseHLT,
    'lhtight':SelectionDefElectron.ElectronLooseHLT,}

ElectronPidName = {'vloose':egammaPID.ElectronIDLooseHLT,
    'loose':egammaPID.ElectronIDLooseHLT,
    'medium':egammaPID.ElectronIDMediumHLT,
    'tight':egammaPID.ElectronIDTightHLT,
    'mergedtight':egammaPID.ElectronIDTightHLT,
    }

# Dictionaries for ToolNames
PhotonToolName = {'loose':'AsgPhotonIsEMLooseSelector',
    'medium':'AsgPhotonIsEMMediumSelector',
    'tight':'AsgPhotonIsEMTightSelector',}

PhotonToolConfigFile = {'loose':"PhotonIsEMLooseSelectorCutDefs.conf",
    'medium':"PhotonIsEMMediumSelectorCutDefs.conf",
    'tight':"PhotonIsEMTightSelectorCutDefs.conf",}

PhotonIsEMBits = {'loose':SelectionDefPhoton.PhotonLoose,
    'medium':SelectionDefPhoton.PhotonMedium,
    'tight':SelectionDefPhoton.PhotonTight,}

def getAsgToolNames():
  return set( ElectronToolName.values()+\
              ElectronCaloToolName.values()+\
              #ElectronHypoToolName.values()+\
              #ElectronCaloHypoToolName.values()+\
              ElectronLHVLooseToolName.values()+\
              ElectronLHLooseToolName.values()+\
              ElectronLHMediumToolName.values()+\
              ElectronLHTightToolName.values())

def clearAllAsgSelectors():
  from AthenaCommon.AppMgr import ToolSvc
  for name in getAsgToolNames():
    if hasattr(ToolSvc,name):
      print 'removing ',name
      ToolSvc.remove(name)



# Add function to add to ToolSvc, same as in Factories
def addToToolSvc( tool ):
    "addToToolSvc( tool ) --> add tool to ToolSvc"
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,tool.getName()):
        ToolSvc += tool
        #print tool
    return tool


def ElectronPidTools():
    # Versioned selectors for Run2
    #print '=========== Run2 PID ============'
    for key in ElectronToolName:
        if('lh' in key):
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
            tool.usePVContainer = False
            addToToolSvc( tool )
        else:
            tool=CfgMgr.AsgElectronIsEMSelector(ElectronToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
            tool.isEMMask = ElectronIsEMBits[key]
            addToToolSvc( tool )
   
    # Calo-only selectors
    #print '=========== Run2 PID Calo============'
    for key in ElectronCaloToolName:
        if('lh' in key):
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronCaloToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
            tool.usePVContainer = False
            tool.caloOnly = True
            addToToolSvc( tool )
        else: 
            tool=CfgMgr.AsgElectronIsEMSelector(ElectronCaloToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
            tool.isEMMask = ElectronIsEMBits[key]
            tool.caloOnly = True
            addToToolSvc( tool )
   
   
    # Special LH triggers for alignment / commisioning
    for key in ElectronLHVLooseToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHVLooseToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHVLooseToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
       
    for key in ElectronLHLooseToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHLooseToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHLooseToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
       
    for key in ElectronLHMediumToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHMediumToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHMediumToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
       
    for key in ElectronLHTightToolName:
        tool=CfgMgr.AsgElectronLikelihoodTool(ElectronLHTightToolName[key])
        tool.ConfigFile = ConfigFilePath + ElectronLHTightToolConfigFile[key]
        tool.usePVContainer = False
        addToToolSvc( tool )
   
def PhotonPidTools():
    from AthenaCommon.AppMgr import ToolSvc
    # Run2 Photon trigger
    for key in PhotonToolName:
        tool=CfgMgr.AsgPhotonIsEMSelector(PhotonToolName[key])
        tool.ConfigFile = ConfigFilePath + PhotonToolConfigFile[key]
        tool.isEMMask = PhotonIsEMBits[key]
        tool.ForceConvertedPhotonPID = True
        addToToolSvc( tool )
del mlog


