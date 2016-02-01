# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetFex
from TrigBjetHypo.TrigBjetFexTuning import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP1D import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP2D import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP3D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP1D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP2D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP3D import *
from TrigBjetHypo.TrigBjetFexTuning_EVTX import *
from TrigBjetHypo.TrigBjetFexTuning_MVTX import *
from TrigBjetHypo.TrigBjetFexTuning_NVTX import *


from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import mm, GeV

def getBjetFexInstance( instance, version, algo):
    if instance=="EF" :
        return BjetFex( instance=instance, version=version, algo=algo, name="EFBjetFex_"+algo )

def getBjetFexSplitInstance( instance, version, algo):
    return BjetFexSplit( instance=instance, version=version, algo=algo, name=instance+"BjetFexSplit_"+algo )

def getBjetFexFTKInstance( instance, version, algo):
    return BjetFexFTK( instance=instance, version=version, algo=algo, name=instance+"BjetFexFTK_"+algo )

def getBjetFexFTKRefitInstance( instance, version, algo):
    return BjetFexFTKRefit( instance=instance, version=version, algo=algo, name=instance+"BjetFexFTKRefit_"+algo )

def getBjetFexFTKVtxInstance( instance, version, algo):
    return BjetFexFTKVtx( instance=instance, version=version, algo=algo, name=instance+"BjetFexFTKVtx_"+algo )


class BjetFex (TrigBjetFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BjetFex, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedInstances = ["EF"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        if instance=="EF" :
            calibInstance = "EF"

        self.JetKey = ""
            
        self.PriVtxKey = "EFHistoPrmVtx"
        
        self.par_0_MC = getTuning_par_0_MC(calibInstance)
        self.par_1_MC = getTuning_par_1_MC(calibInstance)
        self.par_0_DT = getTuning_par_0_DT(calibInstance)
        self.par_1_DT = getTuning_par_1_DT(calibInstance)
            
        self.SizeIP1D_Grade1 = getTuning_SizeIP1D_Grade1(calibInstance)
        self.bIP1D_Grade1    = getTuning_bIP1D_Grade1   (calibInstance)
        self.uIP1D_Grade1    = getTuning_uIP1D_Grade1   (calibInstance)
        self.SizeIP2D_Grade1 = getTuning_SizeIP2D_Grade1(calibInstance)
        self.bIP2D_Grade1    = getTuning_bIP2D_Grade1   (calibInstance)
        self.uIP2D_Grade1    = getTuning_uIP2D_Grade1   (calibInstance)
        self.SizeIP3D_Grade1 = getTuning_SizeIP3D_Grade1(calibInstance)
        self.bIP3D_Grade1    = getTuning_bIP3D_Grade1   (calibInstance)
        self.uIP3D_Grade1    = getTuning_uIP3D_Grade1   (calibInstance)
        
        self.SizeIP1D_Grade2 = getTuning_SizeIP1D_Grade2(calibInstance)
        self.bIP1D_Grade2    = getTuning_bIP1D_Grade2   (calibInstance)
        self.uIP1D_Grade2    = getTuning_uIP1D_Grade2   (calibInstance)
        self.SizeIP2D_Grade2 = getTuning_SizeIP2D_Grade2(calibInstance)
        self.bIP2D_Grade2    = getTuning_bIP2D_Grade2   (calibInstance)
        self.uIP2D_Grade2    = getTuning_uIP2D_Grade2   (calibInstance)
        self.SizeIP3D_Grade2 = getTuning_SizeIP3D_Grade2(calibInstance)
        self.bIP3D_Grade2    = getTuning_bIP3D_Grade2   (calibInstance)
        self.uIP3D_Grade2    = getTuning_uIP3D_Grade2   (calibInstance)
        
        self.SizeSV   = getTuning_SizeSV  (calibInstance)
        self.bSV      = getTuning_bSV     (calibInstance)
        self.uSV      = getTuning_uSV     (calibInstance)
        self.SizeMVtx = getTuning_SizeMVtx(calibInstance)
        self.bMVtx    = getTuning_bMVtx   (calibInstance)
        self.uMVtx    = getTuning_uMVtx   (calibInstance)
        self.SizeEVtx = getTuning_SizeEVtx(calibInstance)
        self.bEVtx    = getTuning_bEVtx   (calibInstance)
        self.uEVtx    = getTuning_uEVtx   (calibInstance)
        self.SizeNVtx = getTuning_SizeNVtx(calibInstance)
        self.bNVtx    = getTuning_bNVtx   (calibInstance)
        self.uNVtx    = getTuning_uNVtx   (calibInstance)
        
        self.AlgoId = -1
        
        if instance=="EF" :
            if algo=="EFID" :
                self.AlgoId = 1
        
        if self.AlgoId==-1 :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        ## Unset = -1; HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3
        self.UseJetDirection = -1
        
        if version=="2012" :
            self.UseJetDirection = 1
            self.RetrieveHLTJets = True
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="EF" :
                self.TagHLTJets = 3
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useGrading         = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="EFID" :
            self.TrkSelGrade1_Chi2            = 0.0
            self.TrkSelGrade1_Innermost       = 1
            self.TrkSelGrade1_NextToInnermost = 0
            self.TrkSelGrade1_PixHits         = 2
            self.TrkSelGrade1_SiHits          = 7
            self.TrkSelGrade1_D0              = 1*mm
            self.TrkSelGrade1_Z0              = 1.5*mm
            self.TrkSelGrade1_Pt              = 1*GeV
            self.TrkSelGrade1_Eta             = 0.2
            self.TrkSelGrade1_Phi             = 0.2
            self.TrkSelGrade1_R               = 0.2
            self.TrkSelGrade2_Chi2            = 0.0
            self.TrkSelGrade2_Innermost       = 0
            self.TrkSelGrade2_NextToInnermost = 1
            self.TrkSelGrade2_PixHits         = 2
            self.TrkSelGrade2_SiHits          = 7
            self.TrkSelGrade2_D0              = 1*mm
            self.TrkSelGrade2_Z0              = 1.5*mm
            self.TrkSelGrade2_Pt              = 1*GeV
            self.TrkSelGrade2_Eta             = 0.2
            self.TrkSelGrade2_Phi             = 0.2
            self.TrkSelGrade2_R               = 0.2

        if instance=="EF" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigEFBjetFexValidationMonitoring, TrigEFBjetFexOnlineMonitoring
            validation = TrigEFBjetFexValidationMonitoring()
            online     = TrigEFBjetFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]


### Split

class BjetFexSplit (TrigBjetFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BjetFexSplit, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        # Hack to get muon-jet chains working properly
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"

        if instance=="EF" :
            calibInstance = "EF"

        self.PriVtxKey = "xPrimVx" #"EFHistoPrmVtx"
        self.TrackKey = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
        
        self.par_0_MC = getTuning_par_0_MC(calibInstance)
        self.par_1_MC = getTuning_par_1_MC(calibInstance)
        self.par_0_DT = getTuning_par_0_DT(calibInstance)
        self.par_1_DT = getTuning_par_1_DT(calibInstance)
            
        self.SizeIP1D_Grade1 = getTuning_SizeIP1D_Grade1(calibInstance)
        self.bIP1D_Grade1    = getTuning_bIP1D_Grade1   (calibInstance)
        self.uIP1D_Grade1    = getTuning_uIP1D_Grade1   (calibInstance)
        self.SizeIP2D_Grade1 = getTuning_SizeIP2D_Grade1(calibInstance)
        self.bIP2D_Grade1    = getTuning_bIP2D_Grade1   (calibInstance)
        self.uIP2D_Grade1    = getTuning_uIP2D_Grade1   (calibInstance)
        self.SizeIP3D_Grade1 = getTuning_SizeIP3D_Grade1(calibInstance)
        self.bIP3D_Grade1    = getTuning_bIP3D_Grade1   (calibInstance)
        self.uIP3D_Grade1    = getTuning_uIP3D_Grade1   (calibInstance)
        
        self.SizeIP1D_Grade2 = getTuning_SizeIP1D_Grade2(calibInstance)
        self.bIP1D_Grade2    = getTuning_bIP1D_Grade2   (calibInstance)
        self.uIP1D_Grade2    = getTuning_uIP1D_Grade2   (calibInstance)
        self.SizeIP2D_Grade2 = getTuning_SizeIP2D_Grade2(calibInstance)
        self.bIP2D_Grade2    = getTuning_bIP2D_Grade2   (calibInstance)
        self.uIP2D_Grade2    = getTuning_uIP2D_Grade2   (calibInstance)
        self.SizeIP3D_Grade2 = getTuning_SizeIP3D_Grade2(calibInstance)
        self.bIP3D_Grade2    = getTuning_bIP3D_Grade2   (calibInstance)
        self.uIP3D_Grade2    = getTuning_uIP3D_Grade2   (calibInstance)
        
        self.SizeSV   = getTuning_SizeSV  (calibInstance)
        self.bSV      = getTuning_bSV     (calibInstance)
        self.uSV      = getTuning_uSV     (calibInstance)
        self.SizeMVtx = getTuning_SizeMVtx(calibInstance)
        self.bMVtx    = getTuning_bMVtx   (calibInstance)
        self.uMVtx    = getTuning_uMVtx   (calibInstance)
        self.SizeEVtx = getTuning_SizeEVtx(calibInstance)
        self.bEVtx    = getTuning_bEVtx   (calibInstance)
        self.uEVtx    = getTuning_uEVtx   (calibInstance)
        self.SizeNVtx = getTuning_SizeNVtx(calibInstance)
        self.bNVtx    = getTuning_bNVtx   (calibInstance)
        self.uNVtx    = getTuning_uNVtx   (calibInstance)
        
        self.AlgoId = -1
        
        if instance=="EF" :
            if algo=="EFID" :
                self.AlgoId = 1
        
        if self.AlgoId==-1 :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        ## Unset = -1; HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3
        self.UseJetDirection = -1
        
        if version=="2012" :
            self.UseJetDirection = 1
            self.RetrieveHLTJets = True
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="EF" :
                self.TagHLTJets = 3
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useGrading         = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="EFID" :
            self.TrkSelGrade1_Chi2            = 0.0
            self.TrkSelGrade1_Innermost       = 1
            self.TrkSelGrade1_NextToInnermost = 0
            self.TrkSelGrade1_PixHits         = 2
            self.TrkSelGrade1_SiHits          = 7
            self.TrkSelGrade1_D0              = 1*mm
            self.TrkSelGrade1_Z0              = 1.5*mm
            self.TrkSelGrade1_Pt              = 1*GeV
            self.TrkSelGrade1_Eta             = 0.4
            self.TrkSelGrade1_Phi             = 0.4
            self.TrkSelGrade1_R               = 0.4
            self.TrkSelGrade2_Chi2            = 0.0
            self.TrkSelGrade2_Innermost       = 0
            self.TrkSelGrade2_NextToInnermost = 1
            self.TrkSelGrade2_PixHits         = 2
            self.TrkSelGrade2_SiHits          = 7
            self.TrkSelGrade2_D0              = 1*mm
            self.TrkSelGrade2_Z0              = 1.5*mm
            self.TrkSelGrade2_Pt              = 1*GeV
            self.TrkSelGrade2_Eta             = 0.4
            self.TrkSelGrade2_Phi             = 0.4
            self.TrkSelGrade2_R               = 0.4

        if instance=="EF" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigEFBjetFexValidationMonitoring, TrigEFBjetFexOnlineMonitoring
            validation = TrigEFBjetFexValidationMonitoring()
            online     = TrigEFBjetFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]
        
### FTK

class BjetFexFTK (TrigBjetFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BjetFexFTK, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        # Hack to get muon-jet chains working properly
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"

        if instance=="EF" :
            calibInstance = "EF"

        self.PriVtxKey = "HLT_PrimVertexFTK" #"EFHistoPrmVtx"
        self.TrackKey = "InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig"
        
        self.par_0_MC = getTuning_par_0_MC(calibInstance)
        self.par_1_MC = getTuning_par_1_MC(calibInstance)
        self.par_0_DT = getTuning_par_0_DT(calibInstance)
        self.par_1_DT = getTuning_par_1_DT(calibInstance)
            
        self.SizeIP1D_Grade1 = getTuning_SizeIP1D_Grade1(calibInstance)
        self.bIP1D_Grade1    = getTuning_bIP1D_Grade1   (calibInstance)
        self.uIP1D_Grade1    = getTuning_uIP1D_Grade1   (calibInstance)
        self.SizeIP2D_Grade1 = getTuning_SizeIP2D_Grade1(calibInstance)
        self.bIP2D_Grade1    = getTuning_bIP2D_Grade1   (calibInstance)
        self.uIP2D_Grade1    = getTuning_uIP2D_Grade1   (calibInstance)
        self.SizeIP3D_Grade1 = getTuning_SizeIP3D_Grade1(calibInstance)
        self.bIP3D_Grade1    = getTuning_bIP3D_Grade1   (calibInstance)
        self.uIP3D_Grade1    = getTuning_uIP3D_Grade1   (calibInstance)
        
        self.SizeIP1D_Grade2 = getTuning_SizeIP1D_Grade2(calibInstance)
        self.bIP1D_Grade2    = getTuning_bIP1D_Grade2   (calibInstance)
        self.uIP1D_Grade2    = getTuning_uIP1D_Grade2   (calibInstance)
        self.SizeIP2D_Grade2 = getTuning_SizeIP2D_Grade2(calibInstance)
        self.bIP2D_Grade2    = getTuning_bIP2D_Grade2   (calibInstance)
        self.uIP2D_Grade2    = getTuning_uIP2D_Grade2   (calibInstance)
        self.SizeIP3D_Grade2 = getTuning_SizeIP3D_Grade2(calibInstance)
        self.bIP3D_Grade2    = getTuning_bIP3D_Grade2   (calibInstance)
        self.uIP3D_Grade2    = getTuning_uIP3D_Grade2   (calibInstance)
        
        self.SizeSV   = getTuning_SizeSV  (calibInstance)
        self.bSV      = getTuning_bSV     (calibInstance)
        self.uSV      = getTuning_uSV     (calibInstance)
        self.SizeMVtx = getTuning_SizeMVtx(calibInstance)
        self.bMVtx    = getTuning_bMVtx   (calibInstance)
        self.uMVtx    = getTuning_uMVtx   (calibInstance)
        self.SizeEVtx = getTuning_SizeEVtx(calibInstance)
        self.bEVtx    = getTuning_bEVtx   (calibInstance)
        self.uEVtx    = getTuning_uEVtx   (calibInstance)
        self.SizeNVtx = getTuning_SizeNVtx(calibInstance)
        self.bNVtx    = getTuning_bNVtx   (calibInstance)
        self.uNVtx    = getTuning_uNVtx   (calibInstance)
        
        self.AlgoId = -1
        
        if instance=="EF" :
            if algo=="EFID" :
                self.AlgoId = 1
        
        if self.AlgoId==-1 :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        ## Unset = -1; HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3
        self.UseJetDirection = -1
        
        if version=="2012" :
            self.UseJetDirection = 1
            self.RetrieveHLTJets = True
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="EF" :
                self.TagHLTJets = 3
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useGrading         = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="EFID" :
            self.TrkSelGrade1_Chi2            = 0.0
            self.TrkSelGrade1_Innermost       = 1
            self.TrkSelGrade1_NextToInnermost = 0
            self.TrkSelGrade1_PixHits         = 2
            self.TrkSelGrade1_SiHits          = 7
            self.TrkSelGrade1_D0              = 1*mm
            self.TrkSelGrade1_Z0              = 1.5*mm
            self.TrkSelGrade1_Pt              = 1*GeV
            self.TrkSelGrade1_Eta             = 0.4
            self.TrkSelGrade1_Phi             = 0.4
            self.TrkSelGrade1_R               = 0.4
            self.TrkSelGrade2_Chi2            = 0.0
            self.TrkSelGrade2_Innermost       = 0
            self.TrkSelGrade2_NextToInnermost = 1
            self.TrkSelGrade2_PixHits         = 2
            self.TrkSelGrade2_SiHits          = 7
            self.TrkSelGrade2_D0              = 1*mm
            self.TrkSelGrade2_Z0              = 1.5*mm
            self.TrkSelGrade2_Pt              = 1*GeV
            self.TrkSelGrade2_Eta             = 0.4
            self.TrkSelGrade2_Phi             = 0.4
            self.TrkSelGrade2_R               = 0.4

        if instance=="EF" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigEFBjetFexValidationMonitoring, TrigEFBjetFexOnlineMonitoring
            validation = TrigEFBjetFexValidationMonitoring()
            online     = TrigEFBjetFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]
        
### FTK refit

class BjetFexFTKRefit (TrigBjetFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BjetFexFTKRefit, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        # Hack to get muon-jet chains working properly
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"

        if instance=="EF" :
            calibInstance = "EF"

        self.PriVtxKey = "HLT_PrimVertexFTK" #"EFHistoPrmVtx"
        self.TrackKey = "InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig"
        
        self.par_0_MC = getTuning_par_0_MC(calibInstance)
        self.par_1_MC = getTuning_par_1_MC(calibInstance)
        self.par_0_DT = getTuning_par_0_DT(calibInstance)
        self.par_1_DT = getTuning_par_1_DT(calibInstance)
            
        self.SizeIP1D_Grade1 = getTuning_SizeIP1D_Grade1(calibInstance)
        self.bIP1D_Grade1    = getTuning_bIP1D_Grade1   (calibInstance)
        self.uIP1D_Grade1    = getTuning_uIP1D_Grade1   (calibInstance)
        self.SizeIP2D_Grade1 = getTuning_SizeIP2D_Grade1(calibInstance)
        self.bIP2D_Grade1    = getTuning_bIP2D_Grade1   (calibInstance)
        self.uIP2D_Grade1    = getTuning_uIP2D_Grade1   (calibInstance)
        self.SizeIP3D_Grade1 = getTuning_SizeIP3D_Grade1(calibInstance)
        self.bIP3D_Grade1    = getTuning_bIP3D_Grade1   (calibInstance)
        self.uIP3D_Grade1    = getTuning_uIP3D_Grade1   (calibInstance)
        
        self.SizeIP1D_Grade2 = getTuning_SizeIP1D_Grade2(calibInstance)
        self.bIP1D_Grade2    = getTuning_bIP1D_Grade2   (calibInstance)
        self.uIP1D_Grade2    = getTuning_uIP1D_Grade2   (calibInstance)
        self.SizeIP2D_Grade2 = getTuning_SizeIP2D_Grade2(calibInstance)
        self.bIP2D_Grade2    = getTuning_bIP2D_Grade2   (calibInstance)
        self.uIP2D_Grade2    = getTuning_uIP2D_Grade2   (calibInstance)
        self.SizeIP3D_Grade2 = getTuning_SizeIP3D_Grade2(calibInstance)
        self.bIP3D_Grade2    = getTuning_bIP3D_Grade2   (calibInstance)
        self.uIP3D_Grade2    = getTuning_uIP3D_Grade2   (calibInstance)
        
        self.SizeSV   = getTuning_SizeSV  (calibInstance)
        self.bSV      = getTuning_bSV     (calibInstance)
        self.uSV      = getTuning_uSV     (calibInstance)
        self.SizeMVtx = getTuning_SizeMVtx(calibInstance)
        self.bMVtx    = getTuning_bMVtx   (calibInstance)
        self.uMVtx    = getTuning_uMVtx   (calibInstance)
        self.SizeEVtx = getTuning_SizeEVtx(calibInstance)
        self.bEVtx    = getTuning_bEVtx   (calibInstance)
        self.uEVtx    = getTuning_uEVtx   (calibInstance)
        self.SizeNVtx = getTuning_SizeNVtx(calibInstance)
        self.bNVtx    = getTuning_bNVtx   (calibInstance)
        self.uNVtx    = getTuning_uNVtx   (calibInstance)
        
        self.AlgoId = -1
        
        if instance=="EF" :
            if algo=="EFID" :
                self.AlgoId = 1
        
        if self.AlgoId==-1 :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        ## Unset = -1; HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3
        self.UseJetDirection = -1
        
        if version=="2012" :
            self.UseJetDirection = 1
            self.RetrieveHLTJets = True
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="EF" :
                self.TagHLTJets = 3
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useGrading         = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="EFID" :
            self.TrkSelGrade1_Chi2            = 0.0
            self.TrkSelGrade1_Innermost       = 1
            self.TrkSelGrade1_NextToInnermost = 0
            self.TrkSelGrade1_PixHits         = 2
            self.TrkSelGrade1_SiHits          = 7
            self.TrkSelGrade1_D0              = 1*mm
            self.TrkSelGrade1_Z0              = 1.5*mm
            self.TrkSelGrade1_Pt              = 1*GeV
            self.TrkSelGrade1_Eta             = 0.4
            self.TrkSelGrade1_Phi             = 0.4
            self.TrkSelGrade1_R               = 0.4
            self.TrkSelGrade2_Chi2            = 0.0
            self.TrkSelGrade2_Innermost       = 0
            self.TrkSelGrade2_NextToInnermost = 1
            self.TrkSelGrade2_PixHits         = 2
            self.TrkSelGrade2_SiHits          = 7
            self.TrkSelGrade2_D0              = 1*mm
            self.TrkSelGrade2_Z0              = 1.5*mm
            self.TrkSelGrade2_Pt              = 1*GeV
            self.TrkSelGrade2_Eta             = 0.4
            self.TrkSelGrade2_Phi             = 0.4
            self.TrkSelGrade2_R               = 0.4

        if instance=="EF" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigEFBjetFexValidationMonitoring, TrigEFBjetFexOnlineMonitoring
            validation = TrigEFBjetFexValidationMonitoring()
            online     = TrigEFBjetFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]

#FTKVtx

class BjetFexFTKVtx (TrigBjetFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BjetFexFTKVtx, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedInstances = ["EF", "MuJetChain"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        # Hack to get muon-jet chains working properly
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"

        if instance=="EF" :
            calibInstance = "EF"

        self.PriVtxKey = "HLT_PrimVertexFTK" #"EFHistoPrmVtx"
        self.TrackKey = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"
        
        self.par_0_MC = getTuning_par_0_MC(calibInstance)
        self.par_1_MC = getTuning_par_1_MC(calibInstance)
        self.par_0_DT = getTuning_par_0_DT(calibInstance)
        self.par_1_DT = getTuning_par_1_DT(calibInstance)
            
        self.SizeIP1D_Grade1 = getTuning_SizeIP1D_Grade1(calibInstance)
        self.bIP1D_Grade1    = getTuning_bIP1D_Grade1   (calibInstance)
        self.uIP1D_Grade1    = getTuning_uIP1D_Grade1   (calibInstance)
        self.SizeIP2D_Grade1 = getTuning_SizeIP2D_Grade1(calibInstance)
        self.bIP2D_Grade1    = getTuning_bIP2D_Grade1   (calibInstance)
        self.uIP2D_Grade1    = getTuning_uIP2D_Grade1   (calibInstance)
        self.SizeIP3D_Grade1 = getTuning_SizeIP3D_Grade1(calibInstance)
        self.bIP3D_Grade1    = getTuning_bIP3D_Grade1   (calibInstance)
        self.uIP3D_Grade1    = getTuning_uIP3D_Grade1   (calibInstance)
        
        self.SizeIP1D_Grade2 = getTuning_SizeIP1D_Grade2(calibInstance)
        self.bIP1D_Grade2    = getTuning_bIP1D_Grade2   (calibInstance)
        self.uIP1D_Grade2    = getTuning_uIP1D_Grade2   (calibInstance)
        self.SizeIP2D_Grade2 = getTuning_SizeIP2D_Grade2(calibInstance)
        self.bIP2D_Grade2    = getTuning_bIP2D_Grade2   (calibInstance)
        self.uIP2D_Grade2    = getTuning_uIP2D_Grade2   (calibInstance)
        self.SizeIP3D_Grade2 = getTuning_SizeIP3D_Grade2(calibInstance)
        self.bIP3D_Grade2    = getTuning_bIP3D_Grade2   (calibInstance)
        self.uIP3D_Grade2    = getTuning_uIP3D_Grade2   (calibInstance)
        
        self.SizeSV   = getTuning_SizeSV  (calibInstance)
        self.bSV      = getTuning_bSV     (calibInstance)
        self.uSV      = getTuning_uSV     (calibInstance)
        self.SizeMVtx = getTuning_SizeMVtx(calibInstance)
        self.bMVtx    = getTuning_bMVtx   (calibInstance)
        self.uMVtx    = getTuning_uMVtx   (calibInstance)
        self.SizeEVtx = getTuning_SizeEVtx(calibInstance)
        self.bEVtx    = getTuning_bEVtx   (calibInstance)
        self.uEVtx    = getTuning_uEVtx   (calibInstance)
        self.SizeNVtx = getTuning_SizeNVtx(calibInstance)
        self.bNVtx    = getTuning_bNVtx   (calibInstance)
        self.uNVtx    = getTuning_uNVtx   (calibInstance)
        
        self.AlgoId = -1
        
        if instance=="EF" :
            if algo=="EFID" :
                self.AlgoId = 1
        
        if self.AlgoId==-1 :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        ## Unset = -1; HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3
        self.UseJetDirection = -1
        
        if version=="2012" :
            self.UseJetDirection = 1
            self.RetrieveHLTJets = True
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="EF" :
                self.TagHLTJets = 3
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useGrading         = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="EFID" :
            self.TrkSelGrade1_Chi2            = 0.0
            self.TrkSelGrade1_Innermost       = 1
            self.TrkSelGrade1_NextToInnermost = 0
            self.TrkSelGrade1_PixHits         = 2
            self.TrkSelGrade1_SiHits          = 7
            self.TrkSelGrade1_D0              = 1*mm
            self.TrkSelGrade1_Z0              = 1.5*mm
            self.TrkSelGrade1_Pt              = 1*GeV
            self.TrkSelGrade1_Eta             = 0.4
            self.TrkSelGrade1_Phi             = 0.4
            self.TrkSelGrade1_R               = 0.4
            self.TrkSelGrade2_Chi2            = 0.0
            self.TrkSelGrade2_Innermost       = 0
            self.TrkSelGrade2_NextToInnermost = 1
            self.TrkSelGrade2_PixHits         = 2
            self.TrkSelGrade2_SiHits          = 7
            self.TrkSelGrade2_D0              = 1*mm
            self.TrkSelGrade2_Z0              = 1.5*mm
            self.TrkSelGrade2_Pt              = 1*GeV
            self.TrkSelGrade2_Eta             = 0.4
            self.TrkSelGrade2_Phi             = 0.4
            self.TrkSelGrade2_R               = 0.4

        if instance=="EF" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigEFBjetFexValidationMonitoring, TrigEFBjetFexOnlineMonitoring
            validation = TrigEFBjetFexValidationMonitoring()
            online     = TrigEFBjetFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]
        
