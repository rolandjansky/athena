# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
#ConfigFilePath = "ElectronPhotonSelectorTools/trigger/mc15_20160707/"
ConfigFilePath = "ElectronPhotonSelectorTools/trigger/rel21_20161101/"

# Dictionaries for ToolNames
ElectronToolName = {
    'vloose':   'AsgElectronIsEMVLooseSelector',
    'loose':    'AsgElectronIsEMLooseSelector',
    'medium':   'AsgElectronIsEMMediumSelector',
    'tight':    'AsgElectronIsEMTightSelector',
    'loose1':   'AsgElectronIsEMLoose1Selector',
    'medium1':  'AsgElectronIsEMMedium1Selector',
    'tight1':   'AsgElectronIsEMTight1Selector',
    'lhvloose': 'AsgElectronLHVLooseSelector',
    'lhloose':  'AsgElectronLHLooseSelector',
    'lhmedium': 'AsgElectronLHMediumSelector',
    'lhtight':  'AsgElectronLHTightSelector',}

ElectronHypoToolName = {
    'vloose':   'AsgElectronIsEMVLooseHypoSelector',
    'loose':    'AsgElectronIsEMLooseHypoSelector',
    'medium':   'AsgElectronIsEMMediumHypoSelector',
    'tight':    'AsgElectronIsEMTightHypoSelector',
    'loose1':   'AsgElectronIsEMLoose1HypoSelector',
    'medium1':  'AsgElectronIsEMMedium1HypoSelector',
    'tight1':   'AsgElectronIsEMTight1HypoSelector',
    'lhvloose': 'AsgElectronLHVLooseSelector',
    'lhloose':  'AsgElectronLHLooseSelector',
    'lhmedium': 'AsgElectronLHMediumSelector',
    'lhtight':  'AsgElectronLHTightSelector',}

ElectronCaloToolName = {
    'vloose':   'AsgElectronIsEMVLooseCaloSelector',
    'loose':    'AsgElectronIsEMLooseCaloSelector',
    'medium':   'AsgElectronIsEMMediumCaloSelector',
    'tight':    'AsgElectronIsEMTightCaloSelector',
    'loose1':   'AsgElectronIsEMLoose1CaloSelector',
    'medium1':  'AsgElectronIsEMMedium1CaloSelector',
    'tight1':   'AsgElectronIsEMTight1CaloSelector',
    'lhvloose': 'AsgElectronLHVLooseCaloSelector',
    'lhloose':  'AsgElectronLHLooseCaloSelector',
    'lhmedium': 'AsgElectronLHMediumCaloSelector',
    'lhtight':  'AsgElectronLHTightCaloSelector',}

ElectronCaloHypoToolName = {
    'vloose':   'AsgElectronIsEMVLooseCaloHypoSelector',
    'loose':    'AsgElectronIsEMLooseCaloHypoSelector',
    'medium':   'AsgElectronIsEMMediumCaloHypoSelector',
    'tight':    'AsgElectronIsEMTightCaloHypoSelector',
    'loose1':   'AsgElectronIsEMLoose1CaloHypoSelector',
    'medium1':  'AsgElectronIsEMMedium1CaloHypoSelector',
    'tight1':   'AsgElectronIsEMTight1CaloHypoSelector',
    'lhvloose': 'AsgElectronLHVLooseCaloHypoSelector',
    'lhloose':  'AsgElectronLHLooseCaloHypoSelector',
    'lhmedium': 'AsgElectronLHMediumCaloHypoSelector',
    'lhtight':  'AsgElectronLHTightCaloHypoSelector',}

# Electron LH tools for alignment / commisioning
ElectronLHVLooseToolName = {
    'cutd0dphideta': 'AsgElectronLHVeryLooseCutD0DphiDetaSelector',
    'nod0':          'AsgElectronLHVeryLooseNoD0Selector',
    'nodeta':        'AsgElectronLHVeryLooseNoDetaSelector',
    'nodphires':     'AsgElectronLHVeryLooseNoDphiResSelector',}

ElectronLHVLooseToolConfigFile = {
    'cutd0dphideta': 'ElectronLikelihoodVeryLooseTriggerConfig2016_CutD0DphiDeta_Smooth.conf',
    'nod0':          'ElectronLikelihoodVeryLooseTriggerConfig2015_NoD0_Smooth.conf',
    'nodeta':        'ElectronLikelihoodVeryLooseTriggerConfig2016_NoDeta_Smooth.conf',
    'nodphires':     'ElectronLikelihoodVeryLooseTriggerConfig2015_NoDphiRes.conf',}

ElectronLHLooseToolName = {
    'cutd0dphideta': 'AsgElectronLHLooseCutD0DphiDetaSelector',
    'nod0':          'AsgElectronLHLooseNoD0Selector',
    'nodeta':        'AsgElectronLHLooseNoDetaSelector',
    'nodphires':     'AsgElectronLHLooseNoDphiResSelector',}

ElectronLHLooseToolConfigFile = {
    'cutd0dphideta': 'ElectronLikelihoodLooseTriggerConfig2016_CutD0DphiDeta_Smooth.conf',
    'nod0':          'ElectronLikelihoodLooseTriggerConfig2015_NoD0_Smooth.conf',
    'nodeta':        'ElectronLikelihoodLooseTriggerConfig2016_NoDeta_Smooth.conf',
    'nodphires':     'ElectronLikelihoodLooseTriggerConfig2015_NoDphiRes.conf',}

ElectronLHMediumToolName = {
    'cutd0dphideta': 'AsgElectronLHMediumCutD0DphiDetaSelector',
    'nod0':          'AsgElectronLHMediumNoD0Selector',
    'nodeta':        'AsgElectronLHMediumNoDetaSelector',
    'nodphires':     'AsgElectronLHMediumNoDphiResSelector',}

ElectronLHMediumToolConfigFile = {
    'cutd0dphideta': 'ElectronLikelihoodMediumTriggerConfig2016_CutD0DphiDeta_Smooth.conf',
    'nod0':          'ElectronLikelihoodMediumTriggerConfig2015_NoD0_Smooth.conf',
    'nodeta':        'ElectronLikelihoodMediumTriggerConfig2016_NoDeta_Smooth.conf',
    'nodphires':     'ElectronLikelihoodMediumTriggerConfig2015_NoDphiRes.conf',}

ElectronLHTightToolName = {
    'cutd0dphideta': 'AsgElectronLHTightCutD0DphiDetaSelector',
    'nod0':          'AsgElectronLHTightNoD0Selector',
    'nodeta':        'AsgElectronLHTightNoDetaSelector',
    'nodphires':     'AsgElectronLHTightNoDphiResSelector',
    'smooth':        'AsgElectronLHTightSmoothSelector'}

ElectronLHTightToolConfigFile = {
    'cutd0dphideta': 'ElectronLikelihoodTightTriggerConfig2016_CutD0DphiDeta_Smooth.conf',
    'nod0':          'ElectronLikelihoodTightTriggerConfig2015_NoD0_Smooth.conf',
    'nodeta':        'ElectronLikelihoodTightTriggerConfig2016_NoDeta_Smooth.conf',
    'nodphires':     'ElectronLikelihoodTightTriggerConfig2015_NoDphiRes.conf',
    'smooth':        'ElectronLikelihoodTightTriggerConfig2016_Smooth.conf'}

ElectronToolConfigFile = {
    'vloose':   'ElectronIsEMVLooseSelectorCutDefs.conf',
    'loose':    'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':   'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':    'ElectronIsEMTightSelectorCutDefs.conf',
    'lhvloose': 'ElectronLikelihoodVeryLooseTriggerConfig2016_Smooth.conf',
    'lhloose':  'ElectronLikelihoodLooseTriggerConfig2016_Smooth.conf',
    'lhmedium': 'ElectronLikelihoodMediumTriggerConfig2016_Smooth.conf',
    'lhtight':  'ElectronLikelihoodTightTriggerConfig2016_Smooth.conf',}

ElectronCaloToolConfigFile = {
    'vloose':   'ElectronIsEMVLooseSelectorCutDefs.conf',
    'loose':    'ElectronIsEMLooseSelectorCutDefs.conf',
    'medium':   'ElectronIsEMMediumSelectorCutDefs.conf',
    'tight':    'ElectronIsEMTightSelectorCutDefs.conf',
    'lhvloose': 'ElectronLikelihoodVeryLooseTriggerConfig2016_CaloOnly_Smooth.conf',
    'lhloose':  'ElectronLikelihoodLooseTriggerConfig2016_CaloOnly_Smooth.conf',
    'lhmedium': 'ElectronLikelihoodMediumTriggerConfig2016_CaloOnly_Smooth.conf',
    'lhtight':  'ElectronLikelihoodTightTriggerConfig2016_CaloOnly_Smooth.conf',}

ElectronIsEMBits = {
    'vloose':   SelectionDefElectron.ElectronLooseHLT,
    'loose':    SelectionDefElectron.ElectronLooseHLT,
    'medium':   SelectionDefElectron.ElectronMediumHLT,
    'tight':    SelectionDefElectron.ElectronTightHLT,
    'loose1':   SelectionDefElectron.ElectronLoose1,
    'medium1':  SelectionDefElectron.ElectronMedium1,
    'tight1':   SelectionDefElectron.ElectronTight1,
    'lhvloose': SelectionDefElectron.ElectronLooseHLT,
    'lhloose':  SelectionDefElectron.ElectronLooseHLT,
    'lhmedium': SelectionDefElectron.ElectronLooseHLT,
    'lhtight':  SelectionDefElectron.ElectronLooseHLT,}

ElectronPidName = {
    'vloose':  egammaPID.ElectronIDLooseHLT,
    'loose':   egammaPID.ElectronIDLooseHLT,
    'medium':  egammaPID.ElectronIDMediumHLT,
    'tight':   egammaPID.ElectronIDTightHLT,
    'loose1':  egammaPID.ElectronIDLoose1,
    'medium1': egammaPID.ElectronIDMedium1,
    'tight1':  egammaPID.ElectronIDTight1,}

# Dictionaries for ToolNames
PhotonToolName = {
    'loose':   'AsgPhotonIsEMLooseSelector',
    'medium':  'AsgPhotonIsEMMediumSelector',
    'tight':   'AsgPhotonIsEMTightSelector',
    'loose1':  'AsgElectronIsEMSelector/AsgPhotonIsEMLoose1Selector',
    'medium1': 'AsgElectronIsEMSelector/AsgPhotonIsEMMedium1Selector',
    'tight1':  'AsgPhotonIsEMSelector/AsgPhotonIsEMTight1Selector',}

PhotonToolConfigFile = {
    'loose':  "PhotonIsEMLooseSelectorCutDefs.conf",
    'medium': "PhotonIsEMMediumSelectorCutDefs.conf",
    'tight':  "PhotonIsEMTightSelectorCutDefs.conf",}

PhotonIsEMBits = {
    'loose':   SelectionDefPhoton.PhotonLoose,
    'medium':  SelectionDefPhoton.PhotonMedium,
    'tight':   SelectionDefPhoton.PhotonTight,
    'loose1':  SelectionDefPhoton.PhotonLooseEF,
    'medium1': SelectionDefPhoton.PhotonMediumEF,
    'tight1':  SelectionDefPhoton.PhotonTight,}


def getAsgToolNames():
  return set( ElectronToolName.values()+\
              ElectronCaloToolName.values()+\
              ElectronHypoToolName.values()+\
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
    #from AthenaCommon.AppMgr import ToolSvc   
    # Run1 selectors -- added directly to ToolSvc from default config via mapping
    #print '=========== Run1 PID ============'
    for key in ElectronToolName:
        if not ('lh' in key):
            if ('1' in key):
                tool=ConfiguredAsgElectronIsEMSelector(ElectronToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                addToToolSvc( tool )
   
    # Trigger specific Run1 selectors for high pt triggers
    #print '=========== Run1 PID trigger specific ============'
    for key in ElectronHypoToolName:
        if not ('lh' in key):
            if ('1' in key):
                tool=ConfiguredAsgElectronIsEMSelector(ElectronHypoToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                tool.trigEtTh = 20000
                addToToolSvc( tool )
   
    # Calo only Run1 selectors
    #print '=========== Run1 PID Calo============'
    for key in ElectronCaloToolName:
        if not ('lh' in key):
            if ('1' in key):
                tool=ConfiguredAsgElectronIsEMSelector(ElectronCaloToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                tool.caloOnly = True
                addToToolSvc( tool )
   
    # Calo-only Trigger specific with trigEtTh property set 
    #print '=========== Run1 PID Calo trigger specific ============'
    for key in ElectronCaloHypoToolName:
        if not ('lh' in key):
            if ('1' in key):
                tool=ConfiguredAsgElectronIsEMSelector(ElectronCaloHypoToolName[key],ElectronPidName[key],electronPIDmenu.menuTrig2012)
                tool.caloOnly = True
                tool.trigEtTh = 20000
                addToToolSvc( tool )
   
    # Versioned selectors for Run2
    #print '=========== Run2 PID ============'
    for key in ElectronToolName:
        if not('lh' in key):
            if not('1' in key):
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                addToToolSvc( tool )
   
    # Trigger specific Run2 selectors
    #print '=========== Run2 PID trigger specific ============'
    for key in ElectronHypoToolName:
        if not('lh' in key):
            if not('1' in key):
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronHypoToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                tool.trigEtTh = 20000
                addToToolSvc( tool )
   
    # Calo-only selectors
    #print '=========== Run2 PID Calo============'
    for key in ElectronCaloToolName:
        if not('lh' in key):
            if not('1' in key):
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronCaloToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                tool.caloOnly = True
                addToToolSvc( tool )
   
    # Calo-only trigger specific selectors
    #print '=========== Run2 PID Calo trigger specific ============'
    for key in ElectronCaloHypoToolName:
        if not('lh' in key):
            if not('1' in key):
                tool=CfgMgr.AsgElectronIsEMSelector(ElectronCaloHypoToolName[key])
                tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
                tool.isEMMask = ElectronIsEMBits[key]
                tool.caloOnly = True
                tool.trigEtTh = 20000
                addToToolSvc( tool )
   
    # LH selectors
    #print '======== LH selectors ==========='
    for key in ElectronToolName:
        if('lh' in key):
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronToolConfigFile[key]
            tool.usePVContainer = False
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
       

    # Calo-only LH selectors
    #print '======== LH Calo selectors ==========='
    for key in ElectronCaloToolName:
        if('lh' in key):
            tool=CfgMgr.AsgElectronLikelihoodTool(ElectronCaloToolName[key])
            tool.ConfigFile = ConfigFilePath + ElectronCaloToolConfigFile[key]
            tool.usePVContainer = False
            tool.caloOnly = True
            addToToolSvc( tool )
           
   
def PhotonPidTools():
    from AthenaCommon.AppMgr import ToolSvc
    # Run2 Photon trigger
    for key in PhotonToolName:
        if not('1' in key):
            tool=CfgMgr.AsgPhotonIsEMSelector(PhotonToolName[key])
            tool.ConfigFile = ConfigFilePath + PhotonToolConfigFile[key]
            tool.isEMMask = PhotonIsEMBits[key]
            tool.ForceConvertedPhotonPID = True
            addToToolSvc( tool )
           
    # Run1 Photon triggers
    PhotonLoose1Sel=ConfiguredAsgElectronIsEMSelector("AsgPhotonIsEMLoose1Selector",egammaPID.PhotonIDLooseEF,electronPIDmenu.menuTrig2012)
    PhotonLoose1Sel.caloOnly = True
   
    PhotonMedium1Sel=ConfiguredAsgElectronIsEMSelector("AsgPhotonIsEMMedium1Selector",egammaPID.PhotonIDMediumEF,electronPIDmenu.menuTrig2012) 
    PhotonMedium1Sel.caloOnly = True

    PhotonTight1Sel=ConfiguredAsgPhotonIsEMSelector("AsgPhotonIsEMTight1Selector",egammaPID.PhotonIDTight)                   
    PhotonTight1Sel.ConfigFile = 'ElectronPhotonSelectorTools/dc14b_20141031/PhotonIsEMTight8TeVSelectorCutDefs.conf'   
    PhotonTight1Sel.ForceConvertedPhotonPID = True
   
    addToToolSvc( PhotonLoose1Sel )
    addToToolSvc( PhotonMedium1Sel )
    addToToolSvc( PhotonTight1Sel )

    print "====  PhotonLooseHLT:                              0x%08x              =====" % SelectionDefPhoton.PhotonLoose
    print "====  PhotonLoose:                                 0x%08x              =====" % egammaPID.PhotonLoose
    print "====  PhotonMediumHLT:                             0x%08x              =====" % SelectionDefPhoton.PhotonMedium
    print "====  PhotonMedium:                                0x%08x              =====" % egammaPID.PhotonMedium
    print "====  PhotonTightHLT:                              0x%08x              =====" % SelectionDefPhoton.PhotonTight
    print "====  PhotonTight:                                 0x%08x              =====" % egammaPID.PhotonTight

