# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# two implementation of the egammaMonTool base class
from AthenaCommon.SystemOfUnits import GeV

#import base class
from egammaPerformance.egammaMonToolBase import egammaMonToolBase

class egammaMonTool ( egammaMonToolBase ) :
        def __init__(self, name):
                egammaMonToolBase.__init__(self,name)

                # do the configuration
                self.ElectronContainer          ="ElectronCollection"
                self.PhotonContainer            ="PhotonCollection"
                self.EMTrackMatchContainer      ="egDetailContainer"
                self.EMShowerContainer          ="egDetailContainer"
                self.EMConvertContainer         ="egDetailContainer"
                self.TriggerDecisionTool = "TrigDec::TrigDecisionTool"
                self.UseTrigger = True
                self.Electron_Trigger_Items = ["EF_e105","EF_e10i","EF_e25_tight","EF_e10i","EF_2e15i"]
                self.Photon_Trigger_Items = ["EF_g105","EF_g25i_L32","EF_g25_L32"]

                self.EtaCuts = [-2.5,-2.3,-1.9,-1.8,-1.52,-1.37,-1.1,-.8,-.6,
                                0.,.6,.8,1.1,1.37,1.52,1.8,1.9,2.3,2.5]
                self.PhiCuts = [-3.2,-2.88,-2.56,-2.24,-1.92,-1.6,-1.28,-0.96,
                                -0.64,-0.32,0,0.32,0.64,0.96,1.28,1.6,1.92,
                                2.24,2.56,2.88,3.2]
                self.EtCut = 25 * GeV

class egammaAODMonTool ( egammaMonToolBase ) :
        def __init__(self, name):
                egammaMonToolBase.__init__(self,name)

                # do the configuration
                self.ElectronContainer          ="ElectronAODCollection"
                self.PhotonContainer            ="PhotonAODCollection"
                self.EMTrackMatchContainer      ="egDetailAOD"
                self.EMShowerContainer          ="egDetailAOD"
                self.EMConvertContainer         ="egDetailAOD"
                self.TriggerDecisionTool = "TrigDec::TrigDecisionTool"
                self.UseTrigger = True
                self.Electron_Trigger_Items = ["EF_e105","EF_e10i","EF_e25_tight","EF_e10i","EF_2e15i"]
                self.Photon_Trigger_Items = ["EF_g105","EF_g25i_L32","EF_g25_L32"]
