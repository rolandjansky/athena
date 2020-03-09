#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Author: Ryan Mackenzie White <ryan.white@cern.ch>
#
# TrigEgammaMonToolBuilder class
# Holds various properties that are needed to configure monitoring tools
# Provides a way to switch the configuration based on job Properties, e.g. beamType
# Following method taken from TrigEgammaMonToolConfig found in TrigEgammaAnalysisTools/TrigEgammaMonToolConfig.py
# Configuration should be updated and kept in this package for monitoring
# Provide all triggers to be monitored
# Provide list of categories and shifter plots

from __future__ import print_function
# Returns the name of tool in ToolSvc
def TrigEgammaMonitoringTool(**kwargs):
  raise RuntimeError('TrigEgammaMonitoringTool() is decrepted and not works in this package')
    






"""
# Commented code here
"""
from AthenaCommon.Logging import logging
log_trigeg = logging.getLogger( 'TrigEgammaMonitoringConfig' )

from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR

# Move the ProbeList into MonitCategory to keep all configuration in TrigEgammaMonitoring 
from TrigEgammaMonitoring.TrigEgammaMonitCategory import * 

if not 'DQMonFlags' in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags as dqflags


# Ideas taken from HLTMonTriggerList class
class TrigEgammaMonToolBuilder:
    _configured = False
    _get_monitoring_mode_success = False
    
    pp_mode = False
    pPb_mode = False
    HI_mode = False
    cosmic_mode = False
    mc_mode = False
    data_type = ''

    basePath = 'HLT/EgammaMon'
    debugLevel = INFO
    detailLevel = False
    
    # Add a flag to enable emulation
    __acceptable_keys_list=['derivation','emulation']
    emulation = False
    derivation = False

    tagItems = []
    JpsitagItems = []
    
    electronList = []
    photonList = []
    tpList = []
    jpsiList = []
    mam = {}
    
    anaTools=[]

    def __init__(self, monTool, monname, acc, runflag, **kwargs):
        log_trigeg.info('TrigEgammaMonToolBuilder.__init__()')

        # Mon tool is the main algortihm that will execute all ana tools
        # inside of the trigger e/g monitoring stack
        self.monTool = monTool
        # The name of the histogram group created by the mon algorithm tool
        self.monname = monname


        from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import  TrigEgammaAnalysisToolsCfg
        _acc, toolsDict = TrigEgammaAnalysisToolsCfg( runflag )
        # merge all public tools created inside of the e/g analysis config into the monitoring
        # componenet accumulator created by the TrigEgammaAlgorithmConfig. This component will
        # be merged later into the main acc
        acc.merge( _acc )
        # Use this to hold the "pointer" for each analysis tool
        self.__toolsDict = toolsDict
        self.__acc = acc # hold the componenet accumulator

        if not self._configured:
            for key,value in kwargs.items():
                if key in self.__acceptable_keys_list:
                    setattr(self,key,value)
            self.config()





    def config(self):
        log_trigeg.info("TrigEgammaMonToolBuilder.config()")
        self._get_monitoring_mode_success = self.get_monitoring_mode()
        #print (self._get_monitoring_mode_success)
        if self._get_monitoring_mode_success == False:
            log_trigeg.warning("HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used.")
        else:
            log_trigeg.info("Configuring for %s", self.data_type)
        self.setProperties()
        self.configureTools()
        self.configureMonTool()



    # Implementation of https://its.cern.ch/jira/browse/ATR-13200
    def get_monitoring_mode(self):
        log_trigeg.info("TrigEgammaMonToolBuilder.get_monitoring_mode()")
        self.data_type = dqflags.monManDataType()
        if self.data_type == 'monteCarlo': 
            self.mc_mode = True
            return True
        elif self.data_type == 'collisions': 
            self.pp_mode = True
            return True
        elif self.data_type == 'heavyioncollisions':
            self.HI_mode = True
            self.pPb_mode = True
            return True
        elif self.data_type == 'cosmics':
            self.cosmic_mode = True
            return True
        else:
            return False


    def setProperties(self):
        log_trigeg.info("TrigEgammaMonToolBuilder.setProperties()")
        #self.basePath = 'HLT/EgammaMon'
        self.debug = INFO
        self.tagItems = monitoring_tags 
        self.JpsitagItems = monitoring_jpsitags 
        if self.pp_mode == True:
            self.setDefaultProperties()
        elif self.cosmic_mode == True:
            self.electronList = monitoring_electron_cosmic
            self.photonList = monitoring_photon_cosmic
            self.mam=monitoring_mam_cosmic
        elif self.HI_mode == True or self.pPb_mode == True:
            self.electronList = monitoring_electron_hi
            self.photonList = monitoring_photon_hi
            self.tpList=monitoring_electron_hi
            self.mam=monitoring_mam_hi
        elif self.mc_mode == True:
            self.electronList = validation_electron
            self.photonList = validation_photon
            self.tpList = validationTP_electron+monitoring_ele_idperf+monitoring_L1Calo
            self.jpsiList = validationTP_Jpsiee
            self.mam=validation_mam
        else:
            log_trigeg.info('No monitoring mode configured, use default')
            self.setDefaultProperties()
        log_trigeg.info('Configuring TP electron chains %s',self.tpList)
        log_trigeg.info('Configuring electron chains %s',self.electronList)
        log_trigeg.info('Configuring photon chains %s',self.photonList)


    def setDefaultProperties(self):
        log_trigeg.info("TrigEgammaMonToolBuilder.setDefaultProperties()")
        self.electronList = monitoring_electron
        self.photonList = monitoring_photon
        self.tpList = monitoringTP_electron+monitoring_ele_idperf+monitoring_L1Calo
        self.jpsiList = monitoringTP_Jpsiee
        self.mam=monitoring_mam



    def configureElectronMonTool(self):
        log_trigeg.info("TrigEgammaMonToolBuilder.configureElectronMonTool()")
        Tool = self.__toolsDict["Electron"]
        Tool.Analysis='Electrons' # special directory name inside of the basepath
        Tool.BasePath=self.basePath # basepath into the root file
        Tool.TriggerList=self.electronList
        Tool.ForceProbeIsolation=True
        Tool.DefaultProbeSelector="LHMedium"
        Tool.isEMResultNames=["Tight","Medium","Loose"]
        Tool.LHResultNames=["LHTight","LHMedium","LHLoose"]
        Tool.OutputLevel=self.debugLevel
        Tool.DetailedHistograms=self.detailLevel

        self.anaTools.append(Tool)


    def configurePhotonMonTool(self):
        log_trigeg.info("TrigEgammaMonToolBuilder.configurePhotonMonTool()")
        Tool = self.__toolsDict["Photon"]
        Tool.Analysis='Photons'
        Tool.BasePath=self.basePath # basepath into the root file
        Tool.TriggerList=self.photonList
        Tool.OutputLevel=self.debugLevel
        Tool.DetailedHistograms=self.detailLevel

        self.anaTools.append(Tool)

    def configureTPMonTool(self):    
        
        log_trigeg.info("TrigEgammaMonToolBuilder.configureTPMonTool()")
        Tool = self.__toolsDict["Zee"]
        if self.emulation == True:
            # TODO: We will disable the emulator for now
            #from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool
            #HLTEgammaEmulationTool=TrigEgammaEmulationTool.copy(name="TrigEgammaEmulationTool",TriggerList=self.tpList)
            Tool.Analysis='Zee'
            Tool.BasePath=self.basePath # basepath into the root file
            Tool.TriggerList=self.tpList
            Tool.DefaultProbeSelector="LHMedium"
            Tool.TagTriggerList=self.tagItems
            Tool.RemoveCrack=False
            Tool.OutputLevel=self.debugLevel
            Tool.DetailedHistograms=self.detailLevel
        else:
            Tool.Analysis='Zee'
            Tool.BasePath=self.basePath # basepath into the root file
            Tool.TriggerList=self.tpList
            Tool.DefaultProbeSelector="LHMedium"
            Tool.OfflineTagSelector='LHTight'
            Tool.TagTriggerList=self.tagItems
            Tool.RemoveCrack=False
            Tool.OutputLevel=self.debugLevel
            Tool.DetailedHistograms=self.detailLevel
  
        self.anaTools.append(Tool)



    def configureJpsiMonTool(self):        
        log_trigeg.info("TrigEgammaMonToolBuilder.configureJpsiMonTool()")
        Tool = self.__toolsDict["Jpsiee"]
        Tool.Analysis='Jpsiee'
        Tool.BasePath=self.basePath # basepath into the root file
        Tool.TriggerList=self.jpsiList
        Tool.OfflineTagSelector='LHTight'
        Tool.TagTriggerList= self.JpsitagItems
  
        self.anaTools.append(Tool)


    def configureAllMonTools(self):        
        self.configureElectronMonTool()
        self.configurePhotonMonTool()
        self.configureTPMonTool()
        self.configureJpsiMonTool()
   



    def configureTools(self):
        # Since we load the tools by name below 
        # Need to ensure the correct tools are configured 
        # for each monitoring mode
        if self.mc_mode == True or self.pp_mode == True:
            if(self.derivation == True or self.emulation == True):
                self.configureTPMonTool()
            else:
                self.configureAllMonTools()
        elif self.HI_mode == True or self.pPb_mode == True or self.cosmic_mode == True:
            self.configureElectronMonTool()
            self.configurePhotonMonTool()
        else:
            self.configureAllMonTools()
        


 
    def configureMonTool(self):

        if self.mc_mode == True or self.pp_mode == True:
            if(self.derivation == True or self.emulation==True):
                self.monTool.Tools=["TrigEgammaNavTPAnalysisTool/HLTEgammaTPAnalysis"]
            else:
                self.configureDefaultMonTool()
        elif self.HI_mode == True or self.pPb_mode == True:
            self.monTool.Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                            "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis"]
        elif self.cosmic_mode == True:
            self.monTool.Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                            "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis"]
        else:
            self.configureDefaultMonTool()
        
        print("Configure tools for e/g monitoring:")
        print(self.monTool.Tools)

 
    def configureDefaultMonTool(self):
        log_trigeg.info("Default configuration of HLTEgammaMon")
        self.monTool.Tools=[ 
                #"TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis",
                #"TrigEgammaNavTPAnalysisTool/HLTEgammaTPAnalysis",
                #"TrigEgammaNavTPAnalysisTool/HLTEgammaTPJpsieeAnalysis"
              ]




