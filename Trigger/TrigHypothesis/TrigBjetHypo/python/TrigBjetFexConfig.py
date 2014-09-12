# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetFex
from TrigBjetHypo.TrigBjetFexTuning import *

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import mm, GeV



def getBjetFexInstance( instance, version, algo ):
    if instance=="L1.5" :
        return BjetFex( instance=instance, version=version, algo=algo, name="L2BjetFex_FastJet_"+algo )
    elif instance=="L2" :
        return BjetFex( instance=instance, version=version, algo=algo, name="L2BjetFex_Jet_"+algo )
    elif instance=="EF" :
        return BjetFex( instance=instance, version=version, algo=algo, name="EFBjetFex_"+algo )


class BjetFex (TrigBjetFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BjetFex, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedInstances = ["L1.5","L2","EF"]
        AllowedVersions  = ["2012","2011"]
        AllowedAlgos     = ["SiTrack","IDScan","JetA","JetB","JetF","JetFR","EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        if instance=="L1.5" or instance=="L2" :        
            calibInstance = "L2"
        elif instance=="EF" :
            calibInstance = "EF"
        
        self.par_0_MC = getTuning_par_0_MC(calibInstance)
        self.par_1_MC = getTuning_par_1_MC(calibInstance)
        self.par_0_DT = getTuning_par_0_DT(calibInstance)
        self.par_1_DT = getTuning_par_1_DT(calibInstance)
            
        self.SizeIP1D = getTuning_SizeIP1D(calibInstance)
        self.bIP1D    = getTuning_bIP1D   (calibInstance)
        self.uIP1D    = getTuning_uIP1D   (calibInstance)
        self.SizeIP2D = getTuning_SizeIP2D(calibInstance)
        self.bIP2D    = getTuning_bIP2D   (calibInstance)
        self.uIP2D    = getTuning_uIP2D   (calibInstance)
        self.SizeIP3D = getTuning_SizeIP3D(calibInstance)
        self.bIP3D    = getTuning_bIP3D   (calibInstance)
        self.uIP3D    = getTuning_uIP3D   (calibInstance)
        
        self.SizeIP1D_lowSiHits = getTuning_SizeIP1D_lowSiHits(calibInstance)
        self.bIP1D_lowSiHits    = getTuning_bIP1D_lowSiHits   (calibInstance)
        self.uIP1D_lowSiHits    = getTuning_uIP1D_lowSiHits   (calibInstance)
        self.SizeIP2D_lowSiHits = getTuning_SizeIP2D_lowSiHits(calibInstance)
        self.bIP2D_lowSiHits    = getTuning_bIP2D_lowSiHits   (calibInstance)
        self.uIP2D_lowSiHits    = getTuning_uIP2D_lowSiHits   (calibInstance)
        self.SizeIP3D_lowSiHits = getTuning_SizeIP3D_lowSiHits(calibInstance)
        self.bIP3D_lowSiHits    = getTuning_bIP3D_lowSiHits   (calibInstance)
        self.uIP3D_lowSiHits    = getTuning_uIP3D_lowSiHits   (calibInstance)
        
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
        
        if instance=="L1.5" or instance=="L2" :
            if algo=="JetB" :
                self.AlgoId = 6
            elif algo=="JetA" :
                self.AlgoId = 5
            elif algo=="JetF" :
                self.AlgoId = 8
            elif algo=="JetFR" :
                self.AlgoId = 13                
            elif algo=="SiTrack" :
                self.AlgoId = 1
            elif algo=="IDScan" :
                self.AlgoId = 2
        elif instance=="EF" :
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
        elif version=="2011" :
            self. UseJetDirection = 2
            self.RetrieveHLTJets  = False
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; Disabled = 0; L1.5 jets = 1; L2 jets = 2; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="L1.5" :
                self.TagHLTJets = 1
            elif instance=="L2" :
                self.TagHLTJets = 2
            elif instance=="EF" :
                self.TagHLTJets = 3
        elif version=="2011" :
            self.TagHLTJets = 0
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="L1.5" or instance=="L2" :
            self.Instance = "L2"
        elif instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useLowSiHits       = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="JetB" or algo=="SiTrack" or algo=="JetF" or algo=="JetFR" :
            self.TrkSel_Chi2    = 0.001
            self.TrkSel_BLayer  = 1
            self.TrkSel_SiHits  = 4
            self.TrkSel_D0      = 1*mm
            self.TrkSel_Z0      = 2*mm
            self.TrkSel_Pt      = 1*GeV
        elif algo=="JetA" or algo=="IDScan" :
            self.TrkSel_Chi2    = 0.001
            self.TrkSel_BLayer  = 1
            self.TrkSel_SiHits  = 5
            self.TrkSel_D0      = 1*mm
            self.TrkSel_Z0      = 2*mm
            self.TrkSel_Pt      = 1*GeV
        elif algo=="EFID" :
            self.TrkSel_Chi2    = 0.0
            self.TrkSel_BLayer  = 1
            self.TrkSel_PixHits = 2
            self.TrkSel_SiHits  = 7
            self.TrkSel_D0      = 1*mm
            self.TrkSel_Z0      = 2*mm
            self.TrkSel_Pt      = 1*GeV

        if instance=="L1.5" or instance=="L2" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigL2BjetFexValidationMonitoring, TrigL2BjetFexOnlineMonitoring
            validation = TrigL2BjetFexValidationMonitoring()
            online     = TrigL2BjetFexOnlineMonitoring()
        if instance=="EF" :
            from TrigBjetHypo.TrigBjetFexMonitoring import TrigEFBjetFexValidationMonitoring, TrigEFBjetFexOnlineMonitoring
            validation = TrigEFBjetFexValidationMonitoring()
            online     = TrigEFBjetFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]
