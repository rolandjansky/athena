#! /usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

# Provide function that is called by HLTMonitoring
# Returns the name of tool in ToolSvc
def TrigEgammaMonitoringTool(**kwargs):
    from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonToolBuilder
    builder = TrigEgammaMonToolBuilder(**kwargs)
    return builder.monTool

"""
# Commented code here
"""
from AthenaCommon.Logging import logging
log_trigeg = logging.getLogger( 'TrigEgammaMonitoringConfig' )

from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import EfficiencyTool, ResolutionTool, DistTool, TrigEgammaPlotTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool

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

    basePath = 'HLT/Egamma'
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
    
    monTool=[]

    def __init__(self,**kwargs):
        if not self._configured:
            for key,value in kwargs.items():
                print key,value
                if key in self.__acceptable_keys_list:
                    setattr(self,key,value)
            #print self.derivation,self.emulation

            self.config()
        
    def config(self):
        self._get_monitoring_mode_success = self.get_monitoring_mode()
        #print self._get_monitoring_mode_success
        if self._get_monitoring_mode_success == False:
            log_trigeg.warning("HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used.")
        else:
            log_trigeg.info("Configuring for %s", self.data_type)
        self.setProperties()
        self.configureTools()
        self.monTool=self.configureMonTool()

    # Implementation of https://its.cern.ch/jira/browse/ATR-13200
    def get_monitoring_mode(self):
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
        log_trigeg.info("Set Properties")
        self.basePath = 'HLT/Egamma'
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
        self.electronList = monitoring_electron
        self.photonList = monitoring_photon
        self.tpList = monitoringTP_electron+monitoring_ele_idperf+monitoring_L1Calo
        self.jpsiList = monitoringTP_Jpsiee
        self.mam=monitoring_mam

    def configureElectronMonTool(self,plotTool,toolList):
        ElectronAnalysis = TrigEgammaNavAnalysisTool(name='HLTEgammaElectronAnalysis',
                Analysis='Electrons',
                PlotTool=plotTool,
                Tools=toolList,
                TriggerList=self.electronList,
                ForceProbeIsolation=True,
                DefaultProbePid="LHMedium",
                File="",
                OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)

    def configurePhotonMonTool(self,plotTool,toolList):
        PhotonAnalysis = TrigEgammaNavAnalysisTool(name='HLTEgammaPhotonAnalysis',
            Analysis='Photons',
            PlotTool=plotTool,
            Tools=toolList,
            TriggerList=self.photonList,
            File="",
            OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)

    def configureTPMonTool(self,plotTool,toolList):    
        if self.emulation == True:
            from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool
            HLTEgammaEmulationTool=TrigEgammaEmulationTool.copy(name="TrigEgammaEmulationTool",TriggerList=self.tpList)
            TPAnalysis = TrigEgammaNavTPAnalysisTool(name='HLTEgammaTPAnalysis',
                Analysis='Zee',
                PlotTool=plotTool,
                EmulationTool=HLTEgammaEmulationTool,
                doEmulation=True,
                Tools=toolList,
                TriggerList=self.tpList,
                DefaultProbePid="LHMedium",
                File="",
                TagTriggerList=self.tagItems,
                RemoveCrack=False,
                OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)
        else:
            TPAnalysis = TrigEgammaNavTPAnalysisTool(name='HLTEgammaTPAnalysis',
                Analysis='Zee',
                PlotTool=plotTool,
                Tools=toolList,
                TriggerList=self.tpList,
                DefaultProbePid="LHMedium",
                OfflineTagSelector='LHTight',
                #OfflineProbeSelector='LHMedium',
                File="",
                TagTriggerList=self.tagItems,
                RemoveCrack=False,
                OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)
  
    def configureJpsiMonTool(self,plotTool,toolList):        
        JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='HLTEgammaTPJpsieeAnalysis',
                                                            Analysis='Jpsiee',
                                                            PlotTool=plotTool,
                                                            Tools=toolList,
                                                            TriggerList=self.jpsiList,
                                                            File="",
                                                            OfflineTagSelector='LHTight',
                                                            #OfflineProbeSelector='LHMedium',
                                                            TagTriggerList= self.JpsitagItems)
    def configureAllMonTools(self,plotTool,toolList):        
        self.configureElectronMonTool(plotTool,toolList)
        self.configurePhotonMonTool(plotTool,toolList)
        self.configureTPMonTool(plotTool,toolList)
        self.configureJpsiMonTool(plotTool,toolList)
    
    def configureTools(self):
        HLTEgammaPlotTool = TrigEgammaPlotTool.copy(name="HLTEgammaPlotTool",
                DirectoryPath=self.basePath,
                MaM=self.mam,
                Efficiency=plots_efficiency,
                Distribution=plots_distribution,
                Resolution=plots_resolution,
                OutputLevel=self.debugLevel)

        HLTEgammaEffTool = EfficiencyTool.copy(name="HLTEgammaEffTool",PlotTool=HLTEgammaPlotTool,OutputLevel=self.debugLevel)
        HLTEgammaResTool = ResolutionTool.copy(name="HLTEgammaResTool",PlotTool=HLTEgammaPlotTool,OutputLevel=self.debugLevel)
        HLTEgammaDistTool = DistTool.copy(name="HLTEgammaDistTool",PlotTool=HLTEgammaPlotTool,OutputLevel=self.debugLevel)
        
        toolList = [HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool]
        # For MaM, most important is the list of triggers to monitor
        # Use the MaM categories from those lists
        
        # Since we load the tools by name below 
        # Need to ensure the correct tools are configured 
        # for each monitoring mode
        if self.mc_mode == True or self.pp_mode == True:
            if(self.derivation == True or self.emulation == True):
                self.configureTPMonTool(HLTEgammaPlotTool,toolList)
            else:
                self.configureAllMonTools(HLTEgammaPlotTool,toolList)
        elif self.HI_mode == True or self.pPb_mode == True or self.cosmic_mode == True:
            self.configureElectronMonTool(HLTEgammaPlotTool,toolList)
            self.configurePhotonMonTool(HLTEgammaPlotTool,toolList)
        else:
            self.configureAllMonTools(HLTEgammaPlotTool,toolList)
        
  
    def configureDefaultMonTool(self):
        tool = TrigEgammaMonTool( name = "HLTEgammaMon", 
                histoPathBase=self.basePath,
                IgnoreTruncationCheck=True,
                Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                        "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis",
                        "TrigEgammaNavTPAnalysisTool/HLTEgammaTPAnalysis",
                        "TrigEgammaNavTPAnalysisTool/HLTEgammaTPJpsieeAnalysis"])
        return tool 
        
    def configureMonTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        toolList=['TrigEgammaMonTool/HLTEgammaMon'];
        if self.mc_mode == True or self.pp_mode == True:
            if(self.derivation == True or self.emulation==True):
                tool = TrigEgammaMonTool( name = "HLTEgammaMon", 
                        histoPathBase=self.basePath,
                        IgnoreTruncationCheck=True,
                        Tools=["TrigEgammaNavTPAnalysisTool/HLTEgammaTPAnalysis"])
                ToolSvc += tool        
            else:
                ToolSvc += self.configureDefaultMonTool()
        elif self.HI_mode == True or self.pPb_mode == True:
            tool = TrigEgammaMonTool( name = "HLTEgammaMon", 
                    histoPathBase=self.basePath,
                    IgnoreTruncationCheck=True,
                    Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                            "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis"])
            ToolSvc += tool        
        elif self.cosmic_mode == True:
            tool = TrigEgammaMonTool( name = "HLTEgammaMon", 
                    histoPathBase=self.basePath,
                    IgnoreTruncationCheck=True,
                    Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                            "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis"])
            ToolSvc += tool
        else:
            ToolSvc += self.configureDefaultMonTool()
        return toolList    
