# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from TrigBphysHypo.TrigBphysHypoConf import TrigL2BMuMuXFex

from TrigVertexFitter.TrigVertexFitterConf import TrigVertexFitter 
from TrigVertexFitter.TrigVertexFitterConf import TrigL2VertexFitter
from TrigVertexFitter.TrigVertexFitterConf import TrigVertexingTool

VFitTool = TrigVertexFitter()
L2VFitTool = TrigL2VertexFitter()
VertexTool = TrigVertexingTool()

from AthenaCommon.AppMgr import ToolSvc

ToolSvc += L2VFitTool
ToolSvc += VFitTool
ToolSvc += VertexTool

class L2BMuMuXFex_1 (TrigL2BMuMuXFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_1"):
        super( TrigL2BMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # Timeout protection: maximum number of track combinations to try
        self.MaxNcombinations = 10000
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        ##self.CutMuonTightness = 0.01  # how tight dR of muon and ID tracks have to be to eliminate it
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        self.LowerMuVtxMassCut = 100.
        self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4400.
        self.UpperKMuMuMassCut = 5900.
        self.LowerB_KMuMuMassCutVtxOff = 4400.
        self.UpperB_KMuMuMassCutVtxOff = 5900.        
        self.LowerBVtxMassCut = 4400.
        self.UpperBVtxMassCut = 5900.
        self.BVtxChi2Cut = 60.
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 650.
        self.UpperKstar_KaonMassCut = 1150.
        self.LowerBd_KstarMuMuMassCut = 4500.
        self.UpperBd_KstarMuMuMassCut = 6000.
        self.LowerBd_KstarMuMuMassCutVtxOff = 4500.
        self.UpperBd_KstarMuMuMassCutVtxOff = 6000.
        self.LowerKstarVtxMassCut = 700.
        self.UpperKstarVtxMassCut = 1100.
        self.LowerBdVtxMassCut = 4500.
        self.UpperBdVtxMassCut = 6000.
        self.KstarVtxChi2Cut = 400.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 900.#990
        self.UpperPhi1020_KaonMassCut = 1140.#1050
        self.LowerBs_Phi1020MuMuMassCut = 4700.
        self.UpperBs_Phi1020MuMuMassCut = 6000.
        self.LowerBs_Phi1020MuMuMassCutVtxOff = 4700.
        self.UpperBs_Phi1020MuMuMassCutVtxOff = 6000.
        self.LowerPhi1020VtxMassCut = 900.#990
        self.UpperPhi1020VtxMassCut = 1140.#1050
        self.LowerBsVtxMassCut = 4700.
        self.UpperBsVtxMassCut = 6000.
        self.Phi1020VtxChi2Cut = 400.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1000.
        self.UpperLambda_PrPiMassCut = 1240.
        self.LowerLb_LambdaMuMuMassCut = 5000.
        self.UpperLb_LambdaMuMuMassCut = 6200.
        self.LowerLambdaVtxMassCut = 1000.
        self.UpperLambdaVtxMassCut = 1240.
        self.LowerLbVtxMassCut = 5000.
        self.UpperLbVtxMassCut = 6200.
        self.LambdaVtxChi2Cut = 400.
        self.LbVtxChi2Cut = 60.
        #self.LbVtxDistanceCut = 0.
        #self.PiImpactCut = 0.
        #self.PrImpactCut = 0.
        # vertexing off
        self.LowerLb_LambdaMuMuMassCutVtxOff = 5000.
        self.UpperLb_LambdaMuMuMassCutVtxOff = 6200.
        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = False
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1550.
        self.UpperDs_MassCut = 2450.
        self.LowerBc_DsMuMuMassCut = 1750.
        self.UpperBc_DsMuMuMassCut = 7100.
        self.LowerBc_DsMuMuMassCutVtxOff = 1800.
        self.UpperBc_DsMuMuMassCutVtxOff = 7100.
        self.LowerDsVtxMassCut = 1600.
        self.UpperDsVtxMassCut = 2400.
        self.LowerBcVtxMassCut = 1800.
        self.UpperBcVtxMassCut = 7050.
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.
        # vertexing
        #self.DoVertexFit                      = True
        #self.DoMuMuVertexFit                  = True
        #self.DoB_KMuMuVertexFit               = True
        #self.DoKstar_KaonPionVertexFit        = True
        #self.DoBd_KstarMuMuVertexFit          = True
        #self.DoPhi1020_KaonKaonVertexFit      = True
        #self.DoBs_Phi1020MuMuVertexFit        = True
        #self.DoLambda_ProtonPionVertexFit     = True
        #self.DoLb_LambdaMuMuVertexFit         = True
        #self.DoDs_PhiPionVertexFit            = True
        #self.DoBc_DsMuMuVertexFit             = True

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexValidationMonitoring
        validation = TrigL2BMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexOnlineMonitoring
        online = TrigL2BMuMuXFexOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [validation, online, time]
        

class L2BMuMuXFex_EF (TrigL2BMuMuXFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_EF"):
        super( TrigL2BMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # Timeout protection: maximum number of track combinations to try
        self.MaxNcombinations = 10000
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        ##self.CutMuonTightness = 0.01  # how tight dR of muon and ID tracks have to be to eliminate it
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        self.LowerMuVtxMassCut = 100.
        self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4400.
        self.UpperKMuMuMassCut = 5800.
        self.LowerB_KMuMuMassCutVtxOff = 4400.
        self.UpperB_KMuMuMassCutVtxOff = 5800.        
        self.LowerBVtxMassCut = 4400.
        self.UpperBVtxMassCut = 5800.
        self.BVtxChi2Cut = 60.
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 650.
        self.UpperKstar_KaonMassCut = 1150.
        self.LowerBd_KstarMuMuMassCut = 4600.
        self.UpperBd_KstarMuMuMassCut = 5900.
        self.LowerBd_KstarMuMuMassCutVtxOff = 4600.
        self.UpperBd_KstarMuMuMassCutVtxOff = 5900.
        self.LowerKstarVtxMassCut = 700.
        self.UpperKstarVtxMassCut = 1100.
        self.LowerBdVtxMassCut = 4600.
        self.UpperBdVtxMassCut = 5900.
        self.KstarVtxChi2Cut = 400.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 940.#990
        self.UpperPhi1020_KaonMassCut = 1100.#1050
        self.LowerBs_Phi1020MuMuMassCut = 4700.
        self.UpperBs_Phi1020MuMuMassCut = 5800.
        self.LowerBs_Phi1020MuMuMassCutVtxOff = 4700.
        self.UpperBs_Phi1020MuMuMassCutVtxOff = 5800.
        self.LowerPhi1020VtxMassCut = 940.#990
        self.UpperPhi1020VtxMassCut = 1100.#1050
        self.LowerBsVtxMassCut = 4700.
        self.UpperBsVtxMassCut = 5800.
        self.Phi1020VtxChi2Cut = 400.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040.
        self.UpperLambda_PrPiMassCut = 1200.
        self.LowerLb_LambdaMuMuMassCut = 5100.
        self.UpperLb_LambdaMuMuMassCut = 6100.
        self.LowerLambdaVtxMassCut = 1040.
        self.UpperLambdaVtxMassCut = 1200.
        self.LowerLbVtxMassCut = 5100.
        self.UpperLbVtxMassCut = 6100.
        self.LambdaVtxChi2Cut = 400.
        self.LbVtxChi2Cut = 60.
        #self.LbVtxDistanceCut = 0.
        #self.PiImpactCut = 0.
        #self.PrImpactCut = 0.
        # vertexing off
        self.LowerLb_LambdaMuMuMassCutVtxOff = 5000.
        self.UpperLb_LambdaMuMuMassCutVtxOff = 6200.
        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = False
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1600.
        self.UpperDs_MassCut = 2400.
        self.LowerBc_DsMuMuMassCut = 1800.
        self.UpperBc_DsMuMuMassCut = 7050.
        self.LowerBc_DsMuMuMassCutVtxOff = 1800.
        self.UpperBc_DsMuMuMassCutVtxOff = 7050.
        self.LowerDsVtxMassCut = 1600.
        self.UpperDsVtxMassCut = 2400.
        self.LowerBcVtxMassCut = 1800.
        self.UpperBcVtxMassCut = 7050.
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.
        # vertexing
        self.DoVertexFit                      = True
        self.DoMuMuVertexFit                  = True
        self.DoB_KMuMuVertexFit               = True
        self.DoKstar_KaonPionVertexFit        = True
        self.DoBd_KstarMuMuVertexFit          = True
        self.DoPhi1020_KaonKaonVertexFit      = True
        self.DoBs_Phi1020MuMuVertexFit        = True
        self.DoLambda_ProtonPionVertexFit     = True
        self.DoLb_LambdaMuMuVertexFit         = False
        self.DoDs_PhiPionVertexFit            = True
        self.DoBc_DsMuMuVertexFit             = True

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexValidationMonitoring
        validation = TrigL2BMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexOnlineMonitoring
        online = TrigL2BMuMuXFexOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [validation, online, time]
        

class L2BMuMuXFex_allModes (L2BMuMuXFex_1):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_allModes"):
        L2BMuMuXFex_1.__init__(self, name )

        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = True


class L2BMuMuXFex_BplusMuMuKplus (L2BMuMuXFex_1):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_BplusMuMuKplus"):
        L2BMuMuXFex_1.__init__(self, name )
        
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = False
        

class L2BMuMuXFex_allModes_tightBc (L2BMuMuXFex_allModes):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_allModes_tightBc"):
        L2BMuMuXFex_allModes.__init__(self, name )

        # Bc -> Ds* Mu Mu
        self.LowerBc_DsMuMuMassCut = 5350.
        self.LowerBc_DsMuMuMassCutVtxOff = 5400.
        self.LowerBcVtxMassCut = 5400.
         
         
class L2BMuMuXFex_BcMuMuDs (TrigL2BMuMuXFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_BcMuMuDs"):
      super( TrigL2BMuMuXFex, self ).__init__( name )
      
      # AcceptAll flag: if true take events regardless of cuts
      self.AcceptAll = False
      # Timeout protection: maximum number of track combinations to try
      self.MaxNcombinations = 10000
      # muon part
      self.OppositeSign = True # if check opposite sign of muons
      ##self.CutMuonTightness = 0.01  # how tight dR of muon and ID tracks have to be to eliminate it
      self.LowerMuMuMassCut = 100.
      self.UpperMuMuMassCut = 5500.
      self.LowerMuVtxMassCut = 100.
      self.UpperMuVtxMassCut = 5500.
      self.MuVtxChi2Cut = 40.
      # B{+/-} -> K{+/-} Mu Mu
      self.DoB_KMuMuDecay = False
      # Bd -> K*(892) Mu Mu
      self.DoBd_KstarMuMuDecay = False
      # Bs -> Phi(1020) Mu Mu
      self.DoBs_Phi1020MuMuDecay = False
      # Lb -> L Mu Mu     
      self.DoLb_LambdaMuMuDecay = False
      # Bc -> Ds* Mu Mu
      self.DoBc_DsMuMuDecay = True
      self.LowerPhiDs_MassCut = 980.
      self.UpperPhiDs_MassCut = 1080.
      self.LowerDs_MassCut = 1550.
      self.UpperDs_MassCut = 2450.
      self.LowerBc_DsMuMuMassCut = 1750.
      self.UpperBc_DsMuMuMassCut = 7100.
      self.LowerBc_DsMuMuMassCutVtxOff = 1800.
      self.UpperBc_DsMuMuMassCutVtxOff = 7100.
      self.LowerDsVtxMassCut = 1600.
      self.UpperDsVtxMassCut = 2400.
      self.LowerBcVtxMassCut = 1800.
      self.UpperBcVtxMassCut = 7050.
      self.DsVtxChi2Cut = 90.
      self.BcVtxChi2Cut = 120.
      # vertexing
      #self.DoVertexFit                      = True
      #self.DoMuMuVertexFit                  = True
      #self.DoB_KMuMuVertexFit               = True
      #self.DoKstar_KaonPionVertexFit        = True
      #self.DoBd_KstarMuMuVertexFit          = True
      #self.DoPhi1020_KaonKaonVertexFit      = True
      #self.DoBs_Phi1020MuMuVertexFit        = True
      #self.DoLambda_ProtonPionVertexFit     = True
      #self.DoLb_LambdaMuMuVertexFit         = True
      #self.DoDs_PhiPionVertexFit            = True
      #self.DoBc_DsMuMuVertexFit             = True
      
      from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexValidationMonitoring
      validation = TrigL2BMuMuXFexValidationMonitoring()
      
      from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexOnlineMonitoring
      online = TrigL2BMuMuXFexOnlineMonitoring()
      
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      time = TrigTimeHistToolConfig("Time")
      
      self.AthenaMonTools = [validation, online, time]
      

class L2BMuMuXFex_BcMuMuDs_tight (L2BMuMuXFex_BcMuMuDs):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_BcMuMuDs_tight"):
      L2BMuMuXFex_BcMuMuDs.__init__(self, name )
      
      # Bc -> Ds* Mu Mu
      self.LowerBc_DsMuMuMassCut = 5350.
      self.LowerBc_DsMuMuMassCutVtxOff = 5400.
      self.LowerBcVtxMassCut = 5400.
      
      
#        self.TrigVertexFitter   = VFitTool
class L2BMuMuXFex_noVtx (TrigL2BMuMuXFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_noVtx"):
        super( TrigL2BMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        ##self.CutMuonTightness = 0.01  # how tight dR of muon and ID tracks have to be to eliminate it
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        self.LowerMuVtxMassCut = 100.
        self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4500.
        self.UpperKMuMuMassCut = 5900.
        self.LowerKMuMuMassCutVtxOff = 4500.
        self.UpperKMuMuMassCutVtxOff = 5900.
        self.LowerBVtxMassCut = 4500.
        self.UpperBVtxMassCut = 5900.
        self.BVtxChi2Cut = 300.
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 600.
        self.UpperKstar_KaonMassCut = 1500.
        self.LowerBd_KstarMuMuMassCut = 4600.
        self.UpperBd_KstarMuMuMassCut = 6300.
        self.LowerBd_KstarMuMuMassCutVertexingOff = 4600.
        self.UpperBd_KstarMuMuMassCutVertexingOff = 6300.
        self.LowerKstarVtxMassCut = 600.
        self.UpperKstarVtxMassCut = 1200.
        self.LowerBdVtxMassCut = 4600.
        self.UpperBdVtxMassCut = 6000.
        self.KstarVtxChi2Cut = 40.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 990.
        self.UpperPhi1020_KaonMassCut = 1050.
        self.LowerBs_Phi1020MuMuMassCut = 5000.
        self.UpperBs_Phi1020MuMuMassCut = 5800.
        self.LowerBs_Phi1020MuMuMassCutVertexingOff = 5000.
        self.UpperBs_Phi1020MuMuMassCutVertexingOff = 5800.
        self.LowerPhi1020VtxMassCut = 990.
        self.UpperPhi1020VtxMassCut = 1060.
        self.LowerBsVtxMassCut = 4600.
        self.UpperBsVtxMassCut = 6000.
        self.Phi1020VtxChi2Cut = 40.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040.
        self.UpperLambda_PrPiMassCut = 1220.
        self.LowerLb_LambdaMuMuMassCut = 5090.
        self.UpperLb_LambdaMuMuMassCut = 6270.
        self.LowerLambdaVtxMassCut = 1070.
        self.UpperLambdaVtxMassCut = 1160.
        self.LowerLbVtxMassCut = 4800.
        self.UpperLbVtxMassCut = 6100.
        self.LambdaVtxChi2Cut = 100.
        self.LbVtxChi2Cut = 100.
        #self.LbVtxDistanceCut = 0.
        #self.PiImpactCut = 0.
        #self.PrImpactCut = 0.
        # vertexing off
        self.LowerLb_LambdaMuMuMassCutVtxOff = 2000.
        self.UpperLb_LambdaMuMuMassCutVtxOff = 8000.
        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = False
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1550.
        self.UpperDs_MassCut = 2450.
        self.LowerBc_DsMuMuMassCut = 1750.
        self.UpperBc_DsMuMuMassCut = 7100.
        self.LowerBc_DsMuMuMassCutVtxOff = 1800.
        self.UpperBc_DsMuMuMassCutVtxOff = 7100.
        self.LowerDsVtxMassCut = 1600.
        self.UpperDsVtxMassCut = 2400.
        self.LowerBcVtxMassCut = 1800.
        self.UpperBcVtxMassCut = 7050.
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.
        #vertexing
        #self.DoVertexFit                      = False
        #self.DoMuMuVertexFit                  = False
        #self.DoB_KMuMuVertexFit               = False
        #self.DoKstar_KaonPionVertexFit        = False
        #self.DoBd_KstarMuMuVertexFit          = False
        #self.DoPhi1020_KaonKaonVertexFit      = False
        #self.DoBs_Phi1020MuMuVertexFit        = False
        #self.DoLambda_ProtonPionVertexFit     = False
        #self.DoLb_LambdaMuMuVertexFit         = False
        #self.DoDs_PhiPionVertexFit            = False
        #self.DoBc_DsMuMuVertexFit             = False
        
        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexValidationMonitoring_noVtx
        validation = TrigL2BMuMuXFexValidationMonitoring_noVtx()

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexOnlineMonitoring_noVtx
        online = TrigL2BMuMuXFexOnlineMonitoring_noVtx()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [validation, online, time]
         
         
#        self.TrigVertexFitter   = VFitTool


class L2BMuMuXFex_noVtx_noOS (TrigL2BMuMuXFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_noVtx_noOS"):
        super( TrigL2BMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = False # if check opposite sign of muons
        #self.CutMuonTightness = 0.01  # how tight dR of muon and ID tracks have to be to eliminate it
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        self.LowerMuVtxMassCut = 100.
        self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4500.
        self.UpperKMuMuMassCut = 5900.
        self.LowerB_KMuMuMassCutVtxOff = 4500.
        self.UpperB_KMuMuMassCutVtxOff = 5900.        
        self.LowerBVtxMassCut = 4500.
        self.UpperBVtxMassCut = 5900.
        self.BVtxChi2Cut = 300.
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 600.
        self.UpperKstar_KaonMassCut = 1500.
        self.LowerBd_KstarMuMuMassCut = 4600.
        self.UpperBd_KstarMuMuMassCut = 6300.
        self.LowerBd_KstarMuMuMassCutVtxOff = 4600.
        self.UpperBd_KstarMuMuMassCutVtxOff = 6300.
        self.LowerKstarVtxMassCut = 600.
        self.UpperKstarVtxMassCut = 1200.
        self.LowerBdVtxMassCut = 4600.
        self.UpperBdVtxMassCut = 6000.
        self.KstarVtxChi2Cut = 40.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 990.
        self.UpperPhi1020_KaonMassCut = 1050.
        self.LowerBs_Phi1020MuMuMassCut = 5000.
        self.UpperBs_Phi1020MuMuMassCut = 5800.
        self.LowerBs_Phi1020MuMuMassCutVtxOff = 5000.
        self.UpperBs_Phi1020MuMuMassCutVtxOff = 5800.
        self.LowerPhi1020VtxMassCut = 990.
        self.UpperPhi1020VtxMassCut = 1060.
        self.LowerBsVtxMassCut = 4600.
        self.UpperBsVtxMassCut = 6000.
        self.Phi1020VtxChi2Cut = 40.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040.
        self.UpperLambda_PrPiMassCut = 1220.
        self.LowerLb_LambdaMuMuMassCut = 5090.
        self.UpperLb_LambdaMuMuMassCut = 6270.
        self.LowerLambdaVtxMassCut = 1070.
        self.UpperLambdaVtxMassCut = 1160.
        self.LowerLbVtxMassCut = 4800.
        self.UpperLbVtxMassCut = 6100.
        self.LambdaVtxChi2Cut = 100.
        self.LbVtxChi2Cut = 100.
        #self.LbVtxDistanceCut = 0.
        #self.PiImpactCut = 0.
        #self.PrImpactCut = 0.
        # vertexing off
        self.LowerLb_LambdaMuMuMassCutVtxOff = 2000.
        self.UpperLb_LambdaMuMuMassCutVtxOff = 8000.
        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = False
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1550.
        self.UpperDs_MassCut = 2450.
        self.LowerBc_DsMuMuMassCut = 1750.
        self.UpperBc_DsMuMuMassCut = 7100.
        self.LowerBc_DsMuMuMassCutVtxOff = 1800.
        self.UpperBc_DsMuMuMassCutVtxOff = 7100.
        self.LowerDsVtxMassCut = 1600.
        self.UpperDsVtxMassCut = 2400.
        self.LowerBcVtxMassCut = 1800.
        self.UpperBcVtxMassCut = 7050.
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.
        #vertexing
        #self.DoVertexFit                      = False
        #self.DoMuMuVertexFit                  = False
        #self.DoB_KMuMuVertexFit               = False
        #self.DoKstar_KaonPionVertexFit        = False
        #self.DoBd_KstarMuMuVertexFit          = False
        #self.DoPhi1020_KaonKaonVertexFit      = False
        #self.DoBs_Phi1020MuMuVertexFit        = False
        #self.DoLambda_ProtonPionVertexFit     = False
        #self.DoLb_LambdaMuMuVertexFit         = False
        #self.DoDs_PhiPionVertexFit            = False
        #self.DoBc_DsMuMuVertexFit             = False

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexValidationMonitoring_noVtx_noOS
        validation = TrigL2BMuMuXFexValidationMonitoring_noVtx_noOS()

        from TrigBphysHypo.TrigL2BMuMuXFexMonitoring import TrigL2BMuMuXFexOnlineMonitoring_noVtx_noOS
        online = TrigL2BMuMuXFexOnlineMonitoring_noVtx_noOS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [validation, online, time]
         
         
#        self.TrigVertexFitter   = VFitTool

class L2BMuMuXFex_EF_BcMuMuDs (L2BMuMuXFex_EF):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXFex_allModes"):
        L2BMuMuXFex_EF.__init__(self, name )
        
        self.DoB_KMuMuDecay = False
        self.DoBd_KstarMuMuDecay = False
        self.DoBs_Phi1020MuMuDecay = False
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> Ds* Mu Mu
        self.DoBc_DsMuMuDecay = True
        
