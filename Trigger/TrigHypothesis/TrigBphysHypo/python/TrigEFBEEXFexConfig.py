# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# vim: tabstop=4:shiftwidth=4:expandtab
# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo.TrigBphysHypoConf       import TrigBphysHelperUtilsTool
from TrigBphysHypo.TrigBphysHypoConf import TrigEFBEEXFex
from TrigBphysHypo import TrigBphysVertexingConfig


from AthenaCommon.AppMgr import ToolSvc

class EFBEEXFex_1 (TrigEFBEEXFex):
    __slots__ = []
    def __init__(self, name = "EFBEEXFex_1"):
        super( TrigEFBEEXFex, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # Timeout protection: maximum number of track combinations to try
        self.MaxNcombinations = 40000 #JW arbitary reduction from 50000 in ncombinations for timeouts
        # muon part
        self.OppositeSign = True # if check opposite sign of muons
        self.LowerEEMassCut = 100.
        self.UpperEEMassCut = 5500.
        ##self.LowerMuVtxMassCut = 100.
        ##self.UpperMuVtxMassCut = 5500.
        self.VtxChi2Cut = 20.
        # B{+/-} -> K{+/-} Mu Mu
        self.DoB_KEEDecay = True
        self.LowerKEEMassCut = 4400.
        self.UpperKEEMassCut = 5800. #5900.
        ##self.LowerB_KEEMassCutVtxOff = 4500.
        ##self.UpperB_KEEMassCutVtxOff = 5900.        
        ##self.LowerBVtxMassCut = 4500.
        ##self.UpperBVtxMassCut = 5900.
        self.DoB_KEEVertexing = True
        self.BVtxChi2Cut = 60. # 300 in L2
        # Bd -> K*(892) Mu Mu
        self.DoBd_KstarEEDecay = True
        self.LowerKstar_KaonMassCut = 700.
        self.UpperKstar_KaonMassCut = 1100. #1500.
        self.LowerBd_KstarEEMassCut = 4600. #4600.
        self.UpperBd_KstarEEMassCut = 5900.
        ##self.LowerBd_KstarEEMassCutVtxOff = 4600.
        ##self.UpperBd_KstarEEMassCutVtxOff = 6300.
        ##self.LowerKstarVtxMassCut = 600.
        ##self.UpperKstarVtxMassCut = 1200.
        ##self.LowerBdVtxMassCut = 4600.
        ##self.UpperBdVtxMassCut = 6000.
        self.DoKstar_KPiVertexing = True
        self.DoBd_KstarEEVertexing = True
        self.KstarVtxChi2Cut = 400.
        self.BdVtxChi2Cut = 60.
        # Bs -> Phi(1020) Mu Mu
        self.DoBs_Phi1020EEDecay = True
        self.LowerPhi1020_KaonMassCut = 940. #990.
        self.UpperPhi1020_KaonMassCut = 1100. #1050.
        self.LowerBs_Phi1020EEMassCut = 4800. #5000.
        self.UpperBs_Phi1020EEMassCut = 5800. #5800.
        ##self.LowerBs_Phi1020EEMassCutVtxOff = 5000.
        ##self.UpperBs_Phi1020EEMassCutVtxOff = 5800.
        ##self.LowerPhi1020VtxMassCut = 990.
        ##self.UpperPhi1020VtxMassCut = 1060.
        ##self.LowerBsVtxMassCut = 4600.
        ##self.UpperBsVtxMassCut = 6000.
        self.DoPhi1020_KKVertexing = True
        self.DoBs_Phi1020EEVertexing = True
        self.Phi1020VtxChi2Cut = 400.
        self.BsVtxChi2Cut = 60.
        
        from TrigBphysHypo.TrigEFBEEXFexMonitoring import EFBEEXFexValidationMonitoring
        validation = EFBEEXFexValidationMonitoring()

        from TrigBphysHypo.TrigEFBEEXFexMonitoring import EFBEEXFexOnlineMonitoring
        online = EFBEEXFexOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]
        
        
