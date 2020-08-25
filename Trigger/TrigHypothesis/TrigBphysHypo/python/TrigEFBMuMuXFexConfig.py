# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# vim: tabstop=4:shiftwidth=4:expandtab
# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo.TrigBphysHypoConf import TrigEFBMuMuXFex
from TrigBphysHypo import TrigBphysVertexingConfig  # noqa: F401


from AthenaCommon.AppMgr import ToolSvc

class EFBMuMuXFex_1 (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_1"):
        super( TrigEFBMuMuXFex, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # Timeout protection: maximum number of track combinations to try
        self.MaxNcombinations = 40000 #JW arbitary reduction from 50000 in ncombinations for timeouts
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 20.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4400.
        self.UpperKMuMuMassCut = 5800. #5900.
        ##self.LowerB_KMuMuMassCutVtxOff = 4500.
        ##self.UpperB_KMuMuMassCutVtxOff = 5900.        
        ##self.LowerBVtxMassCut = 4500.
        ##self.UpperBVtxMassCut = 5900.
        self.DoB_KMuMuVertexing = True
        self.BVtxChi2Cut = 60. # 300 in L2
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 700.
        self.UpperKstar_KaonMassCut = 1100. #1500.
        self.LowerBd_KstarMuMuMassCut = 4600. #4600.
        self.UpperBd_KstarMuMuMassCut = 5900.
        ##self.LowerBd_KstarMuMuMassCutVtxOff = 4600.
        ##self.UpperBd_KstarMuMuMassCutVtxOff = 6300.
        ##self.LowerKstarVtxMassCut = 600.
        ##self.UpperKstarVtxMassCut = 1200.
        ##self.LowerBdVtxMassCut = 4600.
        ##self.UpperBdVtxMassCut = 6000.
        self.DoKstar_KPiVertexing = True
        self.DoBd_KstarMuMuVertexing = True
        self.KstarVtxChi2Cut = 400.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 940. #990.
        self.UpperPhi1020_KaonMassCut = 1100. #1050.
        self.LowerBs_Phi1020MuMuMassCut = 4800. #5000.
        self.UpperBs_Phi1020MuMuMassCut = 5800. #5800.
        ##self.LowerBs_Phi1020MuMuMassCutVtxOff = 5000.
        ##self.UpperBs_Phi1020MuMuMassCutVtxOff = 5800.
        ##self.LowerPhi1020VtxMassCut = 990.
        ##self.UpperPhi1020VtxMassCut = 1060.
        ##self.LowerBsVtxMassCut = 4600.
        ##self.UpperBsVtxMassCut = 6000.
        self.DoPhi1020_KKVertexing = True
        self.DoBs_Phi1020MuMuVertexing = True
        self.Phi1020VtxChi2Cut = 400.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040. #1040.
        self.UpperLambda_PrPiMassCut = 1200. #1220.
        self.LowerLambda_PtCut = 0.
        self.LowerLb_LambdaMuMuMassCut = 5100. #5090.
        self.UpperLb_LambdaMuMuMassCut = 6100. #6270.
        self.LowerLb_PtCut = 0.
        self.UpperLambdaVtxMassCut = 1200.
        self.LowerLbVtxMassCut = 3850.
        self.UpperLbVtxMassCut = 6150.
        self.DoLambda_PPiVertexing = True
        self.DoLb_LambdaMuMuVertexing = False
        self.DoLb_LambdaMuMuCascade = False
        self.LambdaVtxChi2Cut = 400. #100.
        self.LbVtxChi2Cut = 60. #100.
        ##self.LbVtxDistanceCut = 0.
        ##self.PiImpactCut = 0.
        ##self.PrImpactCut = 0.
        # vertexing off
        ##self.LowerLb_LambdaMuMuMassCutVtxOff = 1080.
        ##self.UpperLb_LambdaMuMuMassCutVtxOff = 1180.
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1600.
        self.UpperDs_MassCut = 2400.
        self.LowerDs_PtCut = 2000.
        self.LowerBc_DsMuMuMassCut = 5450.
        self.UpperBc_DsMuMuMassCut = 7050.
        self.LowerBcDs_PtCut = 8000.
        ##self.LowerDs_LxyCut = 0.15
        self.LowerDs_LxyCut = -999.3
        self.DoDs_Vertexing = True
        self.DoBc_DsMuMuVertexing = True
        self.DoBc_DsMuMuCascade = True
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.

        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        ##self.LowerPhiDs_MassCut = 980.
        ##self.UpperPhiDs_MassCut = 1080.
        self.LowerDplus_MassCut = 1500.
        self.UpperDplus_MassCut = 2300.
        self.LowerDplus_PtCut = 2000.
        ##self.LowerBc_DsMuMuMassCut = 1800.
        self.LowerBc_DplusMuMuMassCut = 5450.
        self.UpperBc_DplusMuMuMassCut = 7050.
        self.LowerBcDplus_PtCut = 8000.
        ##self.LowerDplus_LxyCut = 0.3
        self.LowerDplus_LxyCut = 0.01
        self.DoDplus_Vertexing = True
        self.DoBc_DplusMuMuVertexing = True
        self.DoBc_DplusMuMuCascade = True
        self.DplusVtxChi2Cut = 90.
        self.BcDplusVtxChi2Cut = 180.

        # Bc -> Dstar Mu Mu
        self.DoBc_DstarMuMuDecay = False
        self.LowerD0Dstar_MassCut = 1500.
        self.UpperD0Dstar_MassCut = 2300.
        self.UpperDstar_DMCut = 180.
        self.LowerDstar_PtCut = 2000.
        self.LowerDstarKpi_PtCut = 1800.
        self.LowerBc_DstarMuMuMassCut = 5450.
        self.UpperBc_DstarMuMuMassCut = 7050.
        self.LowerBcDstar_PtCut = 8000.
        ##self.LowerBcD0Dstar_LxyCut = 0.1
        self.DoBc_DstarMuMuVertexing = True
        self.DoBc_DstarMuMuCascade = True
        self.D0DstarVtxChi2Cut = 90.
        self.BcDstarVtxChi2Cut = 120.

        # Bc -> D0 (Dstar with lost pi_s) Mu Mu
        self.DoBc_D0MuMuDecay = False
        self.LowerD0_MassCut = 1500.
        self.UpperD0_MassCut = 2300.
        self.LowerD0_PtCut = 2000.
        self.LowerBc_D0MuMuMassCut = 5250.
        self.UpperBc_D0MuMuMassCut = 6911.
        self.LowerBcD0_PtCut = 8000.
        self.LowerD0_LxyCut = 0.01
        self.DoD0_Vertexing = True
        self.DoBc_D0MuMuVertexing = True
        self.DoBc_D0MuMuCascade = True
        self.D0VtxChi2Cut = 90.
        self.BcD0VtxChi2Cut = 120.
        self.MaxBcD0ToStore = 1000
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_1_legacyVtx (EFBMuMuXFex_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_1_legacyVtx"):
        EFBMuMuXFex_1.__init__(self, name )
        
        # set lecacy helper tool with 'broken' vertex fitter
        self.TrigBphysHelperTool = ToolSvc.TrigBphysHelperUtilsToolLegacy
        
        
class EFBMuMuXFex_allModes (EFBMuMuXFex_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_allModes"):
        EFBMuMuXFex_1.__init__(self, name )
        
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = True
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = True
        # Bc -> Dstar Mu Mu
        self.DoBc_DstarMuMuDecay = True
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = True
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring_allModes
        online = EFBMuMuXFexOnlineMonitoring_allModes()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]



class EFBMuMuXFex_allModes_tightBc (EFBMuMuXFex_allModes):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_allModes_tightBc"):
        EFBMuMuXFex_allModes.__init__(self, name )
        
        # Bc -> D_s* Mu Mu
        self.LowerBc_DsMuMuMassCut = 5450.
        # Bc -> D+ Mu Mu
        self.LowerBc_DplusMuMuMassCut = 5450.
        # Bc -> D* Mu Mu
        self.LowerBc_DstarMuMuMassCut = 5450.
        # Bc -> D0 Mu Mu
        self.LowerBc_D0MuMuMassCut = 5250.
        

class EFBMuMuXFex_noVtx (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_noVtx"):
        super( TrigEFBMuMuXFex, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4500.
        self.UpperKMuMuMassCut = 5900.
        ##self.LowerB_KMuMuMassCutVtxOff = 4500.
        ##self.UpperB_KMuMuMassCutVtxOff = 5900.        
        ##self.LowerBVtxMassCut = 4500.
        ##self.UpperBVtxMassCut = 5900.
        self.DoB_KMuMuVertexing = False
        self.BVtxChi2Cut = 60. # 300 in L2
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 600.
        self.UpperKstar_KaonMassCut = 1500.
        self.LowerBd_KstarMuMuMassCut = 4600.
        self.UpperBd_KstarMuMuMassCut = 6300.
        ##self.LowerBd_KstarMuMuMassCutVtxOff = 4600.
        ##self.UpperBd_KstarMuMuMassCutVtxOff = 6300.
        ##self.LowerKstarVtxMassCut = 600.
        ##self.UpperKstarVtxMassCut = 1200.
        ##self.LowerBdVtxMassCut = 4600.
        ##self.UpperBdVtxMassCut = 6000.
        self.DoKstar_KPiVertexing = False
        self.DoBd_KstarMuMuVertexing = False
        self.KstarVtxChi2Cut = 40.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 990.
        self.UpperPhi1020_KaonMassCut = 1050.
        self.LowerBs_Phi1020MuMuMassCut = 5000.
        self.UpperBs_Phi1020MuMuMassCut = 5800.
        ##self.LowerBs_Phi1020MuMuMassCutVtxOff = 5000.
        ##self.UpperBs_Phi1020MuMuMassCutVtxOff = 5800.
        ##self.LowerPhi1020VtxMassCut = 990.
        ##self.UpperPhi1020VtxMassCut = 1060.
        ##self.LowerBsVtxMassCut = 4600.
        ##self.UpperBsVtxMassCut = 6000.
        self.DoPhi1020_KKVertexing = False
        self.DoBs_Phi1020MuMuVertexing = False
        self.Phi1020VtxChi2Cut = 40.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040. #1040.
        self.UpperLambda_PrPiMassCut = 1250. #1220.
        self.LowerLambda_PtCut = 1200.
        self.LowerLb_LambdaMuMuMassCut = 3700. #5090.
        self.UpperLb_LambdaMuMuMassCut = 6600. #6270.
        self.LowerLb_PtCut = 8000.
        self.UpperLambdaVtxMassCut = 1200.
        self.LowerLbVtxMassCut = 3850.
        self.UpperLbVtxMassCut = 6150.
        self.DoLambda_PPiVertexing = False
        self.DoLb_LambdaMuMuVertexing = False
        self.DoLb_LambdaMuMuCascade = False
        self.LambdaVtxChi2Cut = 90. #100.
        self.LbVtxChi2Cut = 180. #100.
        ##self.LbVtxDistanceCut = 0.
        ##self.PiImpactCut = 0.
        ##self.PrImpactCut = 0.
        # vertexing off
        ##self.LowerLb_LambdaMuMuMassCutVtxOff = 1080.
        ##self.UpperLb_LambdaMuMuMassCutVtxOff = 1180.
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = True
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1600.
        self.UpperDs_MassCut = 2400.
        self.LowerDs_PtCut = 2000.
        self.LowerBc_DsMuMuMassCut = 5450.
        self.UpperBc_DsMuMuMassCut = 7050.
        self.LowerBcDs_PtCut = 8000.
        ##self.LowerDs_LxyCut = 0.3
        self.LowerDs_LxyCut = -999.3
        self.DoDs_Vertexing = False
        self.DoBc_DsMuMuVertexing = False
        self.DoBc_DsMuMuCascade = False
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.

        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = True
        ##self.LowerPhiDs_MassCut = 980.
        ##self.UpperPhiDs_MassCut = 1080.
        self.LowerDplus_MassCut = 1500.
        self.UpperDplus_MassCut = 2300.
        self.LowerDplus_PtCut = 2000.
        ##self.LowerBc_DsMuMuMassCut = 1800.
        self.LowerBc_DplusMuMuMassCut = 5450.
        self.UpperBc_DplusMuMuMassCut = 7050.
        self.LowerBcDplus_PtCut = 8000.
        ##self.LowerDplus_LxyCut = 0.3
        self.LowerDplus_LxyCut = 0.01
        self.DoDplus_Vertexing = False
        self.DoBc_DplusMuMuVertexing = False
        self.DoBc_DplusMuMuCascade = False
        self.DplusVtxChi2Cut = 90.
        self.BcDplusVtxChi2Cut = 180.

        # Bc -> Dstar Mu Mu
        self.DoBc_DstarMuMuDecay = True
        self.LowerD0Dstar_MassCut = 1500.
        self.UpperD0Dstar_MassCut = 2300.
        self.UpperDstar_DMCut = 180.
        self.LowerDstar_PtCut = 2000.
        self.LowerDstarKpi_PtCut = 1800.
        self.LowerBc_DstarMuMuMassCut = 5450.
        self.UpperBc_DstarMuMuMassCut = 7050.
        self.LowerBcDstar_PtCut = 8000.
        self.DoD0Dstar_Vertexing = False
        self.DoBc_DstarMuMuVertexing = False
        self.DoBc_DstarMuMuCascade = False
        self.D0DstarVtxChi2Cut = 90.
        self.BcDstarVtxChi2Cut = 120.

        # Bc -> D0 (Dstar with lost pi_s) Mu Mu
        self.DoBc_D0MuMuDecay = True
        self.LowerD0_MassCut = 1500.
        self.UpperD0_MassCut = 2300.
        self.LowerD0_PtCut = 2000.
        self.LowerBc_D0MuMuMassCut = 5250.
        self.UpperBc_D0MuMuMassCut = 6911.
        self.LowerBcD0_PtCut = 8000.
        self.LowerD0_LxyCut = 0.01
        self.DoD0_Vertexing = False
        self.DoBc_D0MuMuVertexing = False
        self.DoBc_D0MuMuCascade = False
        self.D0VtxChi2Cut = 90.
        self.BcD0VtxChi2Cut = 120.
        self.MaxBcD0ToStore = 1000
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
class EFBMuMuXFex_BplusMuMuKplus (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BplusMuMuKplus"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 2500. # JW update to loose jpsi mass range
        self.UpperMuMuMassCut = 4200.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 20. # JW tighten vertex requirement
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4500.
        self.UpperKMuMuMassCut = 5900.
        #self.LowerB_KMuMuMassCutVtxOff = 4500.
        #self.UpperB_KMuMuMassCutVtxOff = 5900.        
        #self.LowerBVtxMassCut = 4500.
        #self.UpperBVtxMassCut = 5900.
        self.DoB_KMuMuVertexing = True
        self.BVtxChi2Cut = 200. # JW reduce from 
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring_BpMuMuKp
        online = EFBMuMuXFexOnlineMonitoring_BpMuMuKp()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
class EFBMuMuXFex_TauMuMuX (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_TauMuMuX"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 0.
        self.UpperMuMuMassCut = 2700.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 100.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 700.
        self.UpperKMuMuMassCut = 2700.
        #self.LowerB_KMuMuMassCutVtxOff = 4500.
        #self.UpperB_KMuMuMassCutVtxOff = 5900.        
        #self.LowerBVtxMassCut = 4500.
        #self.UpperBVtxMassCut = 5900.
        self.DoB_KMuMuVertexing = True
        self.BVtxChi2Cut = 100.
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        
        #from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        #validation = EFBMuMuXFexValidationMonitoring()

        #from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring_BpMuMuKp
        #online = EFBMuMuXFexOnlineMonitoring_BpMuMuKp()
        
        #from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        #time = TrigTimeHistToolConfig("Time")

        #self.AthenaMonTools = [ validation, online, time ]
        
class EFBMuMuXFex_BplusMuMuKplus_noVtx (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BplusMuMuKplus_noVtx"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 2500.
        self.UpperMuMuMassCut = 4200.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 20.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = True
        self.LowerKMuMuMassCut = 4500.
        self.UpperKMuMuMassCut = 5900.
        #self.LowerB_KMuMuMassCutVtxOff = 4500.
        #self.UpperB_KMuMuMassCutVtxOff = 5900.        
        #self.LowerBVtxMassCut = 4500.
        #self.UpperBVtxMassCut = 5900.
        self.DoB_KMuMuVertexing = False
        self.BVtxChi2Cut = 200.
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring_BpMuMuKp
        online = EFBMuMuXFexOnlineMonitoring_BpMuMuKp()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_BdMuMuKstar (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BdMuMuKstar"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 600.
        self.UpperKstar_KaonMassCut = 1500.
        self.LowerBd_KstarMuMuMassCut = 4600.
        self.UpperBd_KstarMuMuMassCut = 6300.
        ##self.LowerBd_KstarMuMuMassCutVtxOff = 4600.
        ##self.UpperBd_KstarMuMuMassCutVtxOff = 6300.
        ##self.LowerKstarVtxMassCut = 600.
        ##self.UpperKstarVtxMassCut = 1200.
        ##self.LowerBdVtxMassCut = 4600.
        ##self.UpperBdVtxMassCut = 6000.
        self.DoKstar_KPiVertexing = True
        self.DoBd_KstarMuMuVertexing = True
        self.KstarVtxChi2Cut = 40.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_BdMuMuKstar_noVtx (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BdMuMuKstar_noVtx"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = True
        self.LowerKstar_KaonMassCut = 600.
        self.UpperKstar_KaonMassCut = 1500.
        self.LowerBd_KstarMuMuMassCut = 4600.
        self.UpperBd_KstarMuMuMassCut = 6300.
        ##self.LowerBd_KstarMuMuMassCutVtxOff = 4600.
        ##self.UpperBd_KstarMuMuMassCutVtxOff = 6300.
        ##self.LowerKstarVtxMassCut = 600.
        ##self.UpperKstarVtxMassCut = 1200.
        ##self.LowerBdVtxMassCut = 4600.
        ##self.UpperBdVtxMassCut = 6000.
        self.DoKstar_KPiVertexing = False
        self.DoBd_KstarMuMuVertexing = False
        self.KstarVtxChi2Cut = 40.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_BsMuMuPhi (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BsMuMuPhi"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 980.
        self.UpperPhi1020_KaonMassCut = 1050.
        self.LowerBs_Phi1020MuMuMassCut = 5000.
        self.UpperBs_Phi1020MuMuMassCut = 5800.
        ##self.LowerBs_Phi1020MuMuMassCutVtxOff = 5000.
        ##self.UpperBs_Phi1020MuMuMassCutVtxOff = 5800.
        ##self.LowerPhi1020VtxMassCut = 990.
        ##self.UpperPhi1020VtxMassCut = 1060.
        ##self.LowerBsVtxMassCut = 4600.
        ##self.UpperBsVtxMassCut = 6000.
        self.DoPhi1020_KKVertexing = True
        self.DoBs_Phi1020MuMuVertexing = True
        self.Phi1020VtxChi2Cut = 40.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_BsJpsiPhi (EFBMuMuXFex_BsMuMuPhi):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BsJpsiPhi"):
        EFBMuMuXFex_BsMuMuPhi.__init__(self, name )
        
        # user narrower Jpsi mass cut
        self.LowerMuMuMassCut = 2500.
        self.UpperMuMuMassCut = 4200.
        
        
class EFBMuMuXFex_BsMuMuPhi_noVtx (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BsMuMuPhi_noVtx"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = True
        self.LowerPhi1020_KaonMassCut = 990.
        self.UpperPhi1020_KaonMassCut = 1050.
        self.LowerBs_Phi1020MuMuMassCut = 5000.
        self.UpperBs_Phi1020MuMuMassCut = 5800.
        ##self.LowerBs_Phi1020MuMuMassCutVtxOff = 5000.
        ##self.UpperBs_Phi1020MuMuMassCutVtxOff = 5800.
        ##self.LowerPhi1020VtxMassCut = 990.
        ##self.UpperPhi1020VtxMassCut = 1060.
        ##self.LowerBsVtxMassCut = 4600.
        ##self.UpperBsVtxMassCut = 6000.
        self.DoPhi1020_KKVertexing = False
        self.DoBs_Phi1020MuMuVertexing = False
        self.Phi1020VtxChi2Cut = 40.
        self.BsVtxChi2Cut = 60.
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_LbMuMuLambda (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_LbMuMuLambda"):
        super( TrigEFBMuMuXFex, self ).__init__( name )
        
        #self.VertexFitterTool = ToolSvc.TrigBphysFitter
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040. #1040.
        self.UpperLambda_PrPiMassCut = 1250. #1220.
        self.LowerLambda_PtCut = 1200.
        self.LowerLb_LambdaMuMuMassCut = 3700. #5090.
        self.UpperLb_LambdaMuMuMassCut = 6600. #6270.
        self.LowerLb_PtCut = 8000.
        self.UpperLambdaVtxMassCut = 1200.
        self.LowerLbVtxMassCut = 3850.
        self.UpperLbVtxMassCut = 6150.
        self.DoLambda_PPiVertexing = True
        self.DoLb_LambdaMuMuVertexing = True
        self.DoLb_LambdaMuMuCascade = True
        self.LambdaVtxChi2Cut = 90. #100.
        self.LbVtxChi2Cut = 180. #100.
        self.LowerLambda_LxyCut = 5.
        ##self.LbVtxDistanceCut = 0.
        ##self.PiImpactCut = 0.
        ##self.PrImpactCut = 0.
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
class EFBMuMuXFex_LbMuMuLambda_noVtx (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_LbMuMuLambda_noVtx"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = True
        self.LowerLambda_PrPiMassCut = 1040. #1040.
        self.UpperLambda_PrPiMassCut = 1250. #1220.
        self.LowerLambda_PtCut = 1200.
        self.LowerLb_LambdaMuMuMassCut = 3700. #5090.
        self.UpperLb_LambdaMuMuMassCut = 6600. #6270.
        self.LowerLb_PtCut = 8000.
        self.UpperLambdaVtxMassCut = 1200.
        self.LowerLbVtxMassCut = 3850.
        self.UpperLbVtxMassCut = 6150.
        self.DoLambda_PPiVertexing = False
        self.DoLb_LambdaMuMuVertexing = False
        self.DoLb_LambdaMuMuCascade = False
        self.LambdaVtxChi2Cut = 90. #100.
        self.LbVtxChi2Cut = 180. #100.
        ##self.LbVtxDistanceCut = 0.
        ##self.PiImpactCut = 0.
        ##self.PrImpactCut = 0.
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = False
        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        # Bc -> D* Mu Mu
        self.DoBc_DstarMuMuDecay = False
        # Bc -> D0 Mu Mu
        self.DoBc_D0MuMuDecay = False
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring
        online = EFBMuMuXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuXFex_BcMuMuDs (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BcMuMuDs"):
        super( TrigEFBMuMuXFex, self ).__init__( name )
        
        #self.VertexFitterTool = ToolSvc.TrigBphysFitter
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = True
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1600.
        self.UpperDs_MassCut = 2400.
        self.LowerBc_DsMuMuMassCut = 5450.
        self.UpperBc_DsMuMuMassCut = 7050.
        self.DoDs_Vertexing = True
        self.DoBc_DsMuMuVertexing = True
        self.DoBc_DsMuMuCascade = True
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.
        self.MaxBcToStore = 1000

        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = True
        self.LowerDplus_MassCut = 1500.
        self.UpperDplus_MassCut = 2300.
        self.LowerDplus_PtCut = 2000.
        self.LowerBc_DplusMuMuMassCut = 5450.
        self.UpperBc_DplusMuMuMassCut = 7050.
        self.LowerBcDplus_PtCut = 8000.
        ##self.LowerDplus_LxyCut = 0.3
        self.LowerDplus_LxyCut = 0.01
        self.DoDplus_Vertexing = True
        self.DoBc_DplusMuMuVertexing = True
        self.DoBc_DplusMuMuCascade = True
        self.DplusVtxChi2Cut = 90.
        self.BcDplusVtxChi2Cut = 180.
        self.MaxBcDplusToStore = 1000

        # Bc -> Dstar Mu Mu
        self.DoBc_DstarMuMuDecay = True
        self.LowerD0Dstar_MassCut = 1500.
        self.UpperD0Dstar_MassCut = 2300.
        self.UpperDstar_DMCut = 180.
        self.LowerDstar_PtCut = 2000.
        self.LowerDstarKpi_PtCut = 1800.
        self.LowerBc_DstarMuMuMassCut = 5450.
        self.UpperBc_DstarMuMuMassCut = 7050.
        self.LowerBcDstar_PtCut = 8000.
        self.LowerD0Dstar_LxyCut = -999.
        self.DoD0Dstar_Vertexing = True
        self.DoBc_DstarMuMuVertexing = True
        self.DoBc_DstarMuMuCascade = True
        self.D0DstarVtxChi2Cut = 90.
        self.BcDstarVtxChi2Cut = 120.
        self.MaxBcDstarToStore = 1000

        # Bc -> D0 (Dstar with lost pi_s) Mu Mu
        self.DoBc_D0MuMuDecay = True
        self.LowerD0_MassCut = 1500.
        self.UpperD0_MassCut = 2300.
        self.LowerD0_PtCut = 2000.
        self.LowerBc_D0MuMuMassCut = 5250.
        self.UpperBc_D0MuMuMassCut = 6911.
        self.LowerBcD0_PtCut = 8000.
        self.LowerD0_LxyCut = 0.01
        self.DoD0_Vertexing = True
        self.DoBc_D0MuMuVertexing = True
        self.DoBc_D0MuMuCascade = True
        self.D0VtxChi2Cut = 90.
        self.BcD0VtxChi2Cut = 120.
        self.MaxBcD0ToStore = 1000
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring_BcMuMuDs
        online = EFBMuMuXFexOnlineMonitoring_BcMuMuDs()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        

class EFBMuMuXFex_BcMuMuDs_tight (EFBMuMuXFex_BcMuMuDs):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BcMuMuDs_tight"):
        EFBMuMuXFex_BcMuMuDs.__init__(self, name )

        # Bc -> D_s* Mu Mu
        self.LowerBc_DsMuMuMassCut = 5450.
        # Bc -> D+ Mu Mu
        self.LowerBc_DplusMuMuMassCut = 5450.
        # Bc -> D* Mu Mu
        self.LowerBc_DstarMuMuMassCut = 5450.
        # Bc -> D0 Mu Mu
        self.LowerBc_D0MuMuMassCut = 5250.
        
        
class EFBMuMuXFex_BcMuMuDs_noVtx (TrigEFBMuMuXFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXFex_BcMuMuDs_noVtx"):
        super( TrigEFBMuMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerMuMuMassCut = 100.
        self.UpperMuMuMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.MuVtxChi2Cut = 40.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KMuMuDecay = False
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarMuMuDecay = False
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020MuMuDecay = False
        # Lb -> L Mu Mu     
        self.DoLb_LambdaMuMuDecay = False
        # Bc -> D_s* Mu Mu
        self.DoBc_DsMuMuDecay = True
        self.LowerPhiDs_MassCut = 980.
        self.UpperPhiDs_MassCut = 1080.
        self.LowerDs_MassCut = 1600.
        self.UpperDs_MassCut = 2400.
        self.LowerBc_DsMuMuMassCut = 5450.
        self.UpperBc_DsMuMuMassCut = 7050.
        self.DoDs_Vertexing = False
        self.DoBc_DsMuMuVertexing = False
        self.DoBc_DsMuMuCascade = False
        self.DsVtxChi2Cut = 90.
        self.BcVtxChi2Cut = 120.

        # Bc -> D+ Mu Mu
        self.DoBc_DplusMuMuDecay = False
        self.LowerDplus_MassCut = 1500.
        self.UpperDplus_MassCut = 2300.
        self.LowerDplus_PtCut = 2000.
        self.LowerBc_DplusMuMuMassCut = 5450.
        self.UpperBc_DplusMuMuMassCut = 7050.
        self.LowerBcDplus_PtCut = 8000.
        ##self.LowerDplus_LxyCut = 0.3
        self.LowerDplus_LxyCut = 0.01
        self.DoDplus_Vertexing = False
        self.DoBc_DplusMuMuVertexing = False
        self.DoBc_DplusMuMuCascade = False
        self.DplusVtxChi2Cut = 90.
        self.BcDplusVtxChi2Cut = 180.

        # Bc -> Dstar Mu Mu
        self.DoBc_DstarMuMuDecay = False
        self.LowerD0Dstar_MassCut = 1500.
        self.UpperD0Dstar_MassCut = 2300.
        self.UpperDstar_DMCut = 180.
        self.LowerDstar_PtCut = 2000.
        self.LowerDstarKpi_PtCut = 1800.
        self.LowerBc_DstarMuMuMassCut = 5450.
        self.UpperBc_DstarMuMuMassCut = 7050.
        self.LowerBcDstar_PtCut = 8000.
        self.DoBc_DstarMuMuVertexing = False
        self.DoBc_DstarMuMuCascade = False
        self.D0DstarVtxChi2Cut = 90.
        self.BcDstarVtxChi2Cut = 120.
        
        # Bc -> D0 (Dstar with lost pi_s) Mu Mu
        self.DoBc_D0MuMuDecay = False
        self.LowerD0_MassCut = 1500.
        self.UpperD0_MassCut = 2300.
        self.LowerD0_PtCut = 2000.
        self.LowerBc_D0MuMuMassCut = 5250.
        self.UpperBc_D0MuMuMassCut = 6911.
        self.LowerBcD0_PtCut = 8000.
        self.LowerD0_LxyCut = 0.01
        self.DoD0_Vertexing = False
        self.DoBc_D0MuMuVertexing = False
        self.DoBc_D0MuMuCascade = False
        self.D0VtxChi2Cut = 90.
        self.BcD0VtxChi2Cut = 120.
        self.MaxBcD0ToStore = 1000
        
        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexValidationMonitoring
        validation = EFBMuMuXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBMuMuXFexMonitoring import EFBMuMuXFexOnlineMonitoring_BcMuMuDs
        online = EFBMuMuXFexOnlineMonitoring_BcMuMuDs()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
