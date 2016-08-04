#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Author: Ryan Mackenzie White <ryan.white@cern.ch>
#
# TrigEgammaMonToolBuilder class
# Holds various properties that are needed to configure monitoring tools
# Provides a way to switch the configuration based on job Properties, e.g. beamType
"""
# Commented code here
"""
from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import EfficiencyTool, ResolutionTool, DistTool, TrigEgammaPlotTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
# Move the ProbeList into MonitCategory to keep all configuration in TrigEgammaMonitoring 
from TrigEgammaMonitoring.TrigEgammaMonitCategory import * 

if not 'jobproperties' in dir():
    from AthenaCommon.BeamFlags import jobproperties
if not 'rec' in dir():
    from RecExConfig.RecFlags import rec

# Ideas taken from HLTMonTriggerList class
class TrigEgammaMonToolBuilder:
    _configured = False
    _get_monitoring_mode_success = False
    pp_mode = False
    pPb_mode = False
    HI_mode = False
    cosmic_mode = False

    basePath = 'HLT/Egamma'
    debugLevel = INFO
    detailLevel = False

    tagItems = []
    JpsitagItems = []
    
    electronList = []
    photonList = []
    tpList = []
    jpsiList = []
    mam = {}
    
    monTool=[]

    def __init__(self):
        if not self._configured:
            self.config()
        
    def config(self):
        self._get_monitoring_mode_success = self.get_monitoring_mode()
        #print self._get_monitoring_mode_success
        if self._get_monitoring_mode_success == False:
            print "HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used."
        self.setProperties()
        self.configureTools()
        self.monTool=self.configureMonTool()
    
    # Implementation of https://its.cern.ch/jira/browse/ATR-13200
    def get_monitoring_mode(self):
        # Set monitoring mode
        if jobproperties.Beam.beamType() == 'collisions':

            if rec.doHeavyIon == False and rec.doHIP == False:
                self.pp_mode = True
                return True

            if rec.doHeavyIon == True and rec.doHIP == False:
                self.HI_mode = True
                return True

            if rec.doHeavyIon == False and rec.doHIP == True:
                self.pPb_mode = True
                return True

        elif jobproperties.Beam.beamType() == 'cosmics':

            if rec.doHeavyIon == False and rec.doHIP == False:
                self.cosmic_mode = True
                return True

        elif jobproperties.Beam.beamType() == 'singlebeam':
            # what should be done in this case?
            pass

            return False

    def setProperties(self):
        self.basePath = 'HLT/Egamma'
        self.debug = INFO
        self.tagItems = ['HLT_e24_lhtight_nod0_ivarloose',
            'HLT_e26_lhtight_nod0_ivarloose']
    
        self.JpsitagItems = ['HLT_e5_lhtight_nod0_e4_etcut',
                        'HLT_e9_lhtight_nod0_e4_etcut',
                        'HLT_e14_lhtight_nod0_e4_etcut',
                        'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee',
                        'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee']
        if self.pp_mode == True:
            self.setDefaultProperties()
        elif self_cosmic_mode == True:
            self.electronList = monitoring_electron
            self.photonList = monitoring_photon
            self.mam=monitoring_mam_cosmic
        else:
            print 'No monitoring mode configured, use default'
            self.setDefaultProperties()


    def setDefaultProperties(self):
        self.electronList = monitoring_electron
        self.photonList = monitoring_photon
        self.tpList = monitoringTP_electron+monitoring_ele_idperf+monitoring_L1Calo
        self.jpsiList = monitoringTP_Jpsiee
        self.mam=monitoring_mam

    def configureTools(self):
        HLTEgammaPlotTool = TrigEgammaPlotTool(name="HLTEgammaPlotTool",
                DirectoryPath=self.basePath,
                MaM=self.mam,
                Efficiency=["eff_et","eff_eta","eff_mu"],
                Distribution=["et","eta","Reta","Rphi","Rhad","f1","f3","eratio","deta2","eprobHT","npixhits","nscthits","ptvarcone20"],
                Resolution=["res_et","res_Rphi","res_Reta","res_Rhad","res_ptvarcone20","res_deta2"],
                OutputLevel=self.debugLevel)

        HLTEgammaEffTool = EfficiencyTool(name="HLTEgammaEffTool",PlotTool=HLTEgammaPlotTool,OutputLevel=self.debugLevel)
        HLTEgammaResTool = ResolutionTool(name="HLTEgammaResTool",PlotTool=HLTEgammaPlotTool,OutputLevel=self.debugLevel)
        HLTEgammaDistTool = DistTool(name="HLTEgammaDistTool",PlotTool=HLTEgammaPlotTool,OutputLevel=self.debugLevel)


        # For MaM, most important is the list of triggers to monitor
        # Use the MaM categories from those lists
        ElectronAnalysis = TrigEgammaNavAnalysisTool(name='HLTEgammaElectronAnalysis',
                Analysis='Electrons',
                PlotTool=HLTEgammaPlotTool,
                Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
                TriggerList=self.electronList,
                ForceProbeIsolation=True,
                DefaultProbePid="LHMedium",
                File="",
                OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)
        PhotonAnalysis = TrigEgammaNavAnalysisTool(name='HLTEgammaPhotonAnalysis',
                Analysis='Photons',
                PlotTool=HLTEgammaPlotTool,
                Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
                TriggerList=self.photonList,
                File="",
                OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)
    
        TPAnalysis = TrigEgammaNavTPAnalysisTool(name='HLTEgammaTPAnalysis',
            Analysis='Zee',
            PlotTool=HLTEgammaPlotTool,
            Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
            TriggerList=self.tpList,
            DefaultProbePid="LHMedium",
            File="",
            TagTriggerList=self.tagItems,
            RemoveCrack=False,
            OutputLevel=self.debugLevel,DetailedHistograms=self.detailLevel)

        JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='HLTEgammaTPJpsieeAnalysis',
                                                            Analysis='Jpsiee',
                                                            PlotTool=HLTEgammaPlotTool,
                                                            Tools=[HLTEgammaEffTool,HLTEgammaResTool,HLTEgammaDistTool],
                                                            TriggerList=self.jpsiList,
                                                            File="",
                                                            TagTriggerList= self.JpsitagItems)
  
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
        if self.pp_mode == True:
            ToolSvc += self.configureDefaultMonTool()
        elif self_cosmic_mode == True:
            tool= TrigEgammaMonTool( name = "HLTEgammaMon", 
                    histoPathBase=self.basePath,
                    IgnoreTruncationCheck=True,
                    Tools=["TrigEgammaNavAnalysisTool/HLTEgammaPhotonAnalysis",
                            "TrigEgammaNavAnalysisTool/HLTEgammaElectronAnalysis"])
            ToolSvc += tool
        else:
            ToolSvc += self.configureDefaultMonTool()
        return toolList    


