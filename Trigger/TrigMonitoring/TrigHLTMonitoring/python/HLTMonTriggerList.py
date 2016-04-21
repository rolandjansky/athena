# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrigBjetMonitoring.TrigBjetMonitCategory as bjet
import TrigBphysMonitoring.TrigBphysMonitCategory as bphys
import TrigEgammaMonitoring.TrigEgammaMonitCategory as egamma
import TrigJetMonitoring.TrigJetMonitCategory as jets
import TrigMETMonitoring.TrigMETMonitCategory as met
import TrigMinBiasMonitoring.TrigMinBiasMonitCategory as minbias
import TrigMuonMonitoring.TrigMuonMonitCategory as muon
import TrigTauMonitoring.TrigTauMonitCategory as tau

if not 'jobproperties' in dir():
    from AthenaCommon.BeamFlags import jobproperties

if not 'rec' in dir():
    from RecExConfig.RecFlags import rec

class HLTMonTriggerList:
    
    # HLTMonTriggerList config
    _configured = False
    
    # running mode config
    _get_monitoring_mode_success = False
    pp_mode = False
    pPb_mode = False
    HI_mode = False
    cosmic_mode = False
    
    # trigger lists
    monitoring_mujet = []
    monitoring_bjet = []
    monitoring_bphys = []
    primary_bphys = []
    monitoring_egamma = []
    primary_single_ele = []
    primary_single_ele_iso = []
    primary_single_ele_cutbased = []
    primary_single_ele_cutbased_iso = []
    primary_double_ele = []
    primary_double_ele_cutbased = []
    monitoring_ele_idperf = []
    monitoring_ele_idperf_cutbased = []
    monitoring_Zee = []
    monitoring_Jpsiee = []
    primary_single_pho = []
    primary_double_pho = []
    monitoring_l1jet = []
    monitoring_jet = []
    primary_l1jet = []
    primary_jet = []
    monitoring_met = []
    monitoring_minbias = []
    monitoring_muonNonIso = []
    monitoring_muonIso = []
    monitoring_MSonly = []
    monitoring_muonEFFS = []
    monitoring_muon_Support = []
    monitoring_tau = []
    monitoring_singleTau = []
        
        
    def __init__(self):
        if not self._configured:
            self.config()
    
    def config(self):
        
        self.set_HLTMonTrigList_default()
        
        self._get_monitoring_mode_success = self.get_monitoring_mode()
        
        #print self._get_monitoring_mode_success
        #self._get_monitoring_mode_success = False
        
        if self._get_monitoring_mode_success == False:
            # what should be done in this case?
            print "HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used."
        
        elif self.pp_mode == True: 
            print "HLTMonTriggerList: Setting up pp monitoring."
            self.set_HLTMonTrigList_pp()

        elif self.HI_mode == True:
            print "HLTMonTriggerList: Setting up HI monitoring."
            self.set_HLTMonTrigList_HI()

        elif self.pPb_mode == True: 
            print "HLTMonTriggerList: Setting up pPb monitoring."
            self.set_HLTMonTrigList_pPb()

        elif self.cosmic_mode == True: 
            print "HLTMonTriggerList: Setting up cosmic monitoring."
            self.set_HLTMonTrigList_cosmic()
        
        self._configured = True

    
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

                
    # Config of default/running mode independent monitoring lists
    def set_HLTMonTrigList_default(self):
        # Monitoring lists (defaults) which are independent of running mode:
        # These are the trigger lists that will be used unless they are overwritten based on the running mode. In this way the trigger lists can be updated based on running mode with no risk of a signature being left unmonitored
        
        # set the bjet and mujet triggers to the default values
        self.monitoring_bjet = bjet.monitoring_bjet
        self.monitoring_mujet = bjet.monitoring_mujet
        
        # set the bphys triggers to the default values
        self.monitoring_bphys = bphys.monitoring_bphys
        self.primary_bphys = bphys.primary_bphys
        
        # set the egamma triggers to the default values
        self.monitoring_egamma = egamma.monitoring_egamma
        
        self.primary_single_ele = egamma.primary_single_ele
        self.primary_single_ele_iso = egamma.primary_single_ele_iso
        self.primary_single_ele_cutbased = egamma.primary_single_ele_cutbased
        self.primary_single_ele_cutbased_iso = egamma.primary_single_ele_cutbased_iso
        self.primary_double_ele = egamma.primary_double_ele
        self.primary_double_ele_cutbased = egamma.primary_double_ele_cutbased
        self.monitoring_ele_idperf = egamma.monitoring_ele_idperf
        self.monitoring_ele_idperf_cutbased = egamma.monitoring_ele_idperf_cutbased
        self.monitoring_Zee = egamma.monitoring_Zee
        self.monitoring_Jpsiee = egamma.monitoring_Jpsiee
        self.primary_single_pho = egamma.primary_single_pho
        self.primary_double_pho = egamma.primary_double_pho
        
        # set the jet triggers to the default values
        self.monitoring_l1jet = jets.monitoring_l1jet
        self.monitoring_jet = jets.monitoring_jet
        self.primary_l1jet = jets.primary_l1jet
        self.primary_jet = jets.primary_jet
        
        # set the met triggers to the default values
        self.monitoring_met = met.monitoring_met
        
        # set the minbias triggers to the default values
        self.monitoring_minbias = minbias.monitoring_minbias
        
        # set the muon triggers to the default values
        self.monitoring_muonNonIso = muon.monitoring_muonNonIso
        self.monitoring_muonIso = muon.monitoring_muonIso
        self.monitoring_MSonly = muon.monitoring_MSonly
        self.monitoring_muonEFFS = muon.monitoring_muonEFFS
        self.monitoring_muon_Support = muon.monitoring_muon_Support
        
        # set the tau triggers to the default values
        self.monitoring_tau = tau.monitoring_tau
        self.monitoring_singleTau = tau.monitoring_singleTau
        

    # Config of monitoring lists which are dependent on running mode    
    def set_HLTMonTrigList_pp(self):
        self.primary_bphys = bphys.primary_bphys_pp

        self.monitoring_tau = tau.monitoring_tau_pp
        self.monitoring_met = met.monitoring_met_pp
        
        self.monitoring_l1jet = jets.monitoring_l1jet_pp
        self.monitoring_jet = jets.monitoring_jet_pp
        self.primary_l1jet = jets.primary_l1jet_pp
        self.primary_jet = jets.primary_jet_pp

        self.monitoring_muonNonIso = muon.monitoring_muonNonIso_pp
        self.monitoring_muonIso = muon.monitoring_muonIso_pp
        self.monitoring_MSonly = muon.monitoring_MSonly_pp 
        self.monitoring_muonEFFS = muon.monitoring_muonEFFS_pp
        self.monitoring_muon_Support = muon.monitoring_muon_Support_pp

    def set_HLTMonTrigList_HI(self):
        self.primary_bphys = bphys.primary_bphys_hi

        self.monitoring_tau = tau.monitoring_tau_pp
        self.monitoring_met = []

        self.monitoring_l1jet = jets.monitoring_l1jet_hi
        self.monitoring_jet = jets.monitoring_jet_hi
        self.primary_l1jet = jets.primary_l1jet_hi
        self.primary_jet = jets.primary_jet_hi

        self.monitoring_muonNonIso = muon.monitoring_muonNonIso_HI
        self.monitoring_muonIso = muon.monitoring_muonIso_HI
        self.monitoring_MSonly = muon.monitoring_MSonly_HI 
        self.monitoring_muonEFFS = muon.monitoring_muonEFFS_HI
        self.monitoring_muon_Support = muon.monitoring_muon_Support_HI
            
    def set_HLTMonTrigList_pPb(self):
        self.monitoring_l1jet = jets.monitoring_l1jet_hi
        self.monitoring_jet = jets.monitoring_jet_hi
        self.primary_l1jet = jets.primary_l1jet_hi
        self.primary_jet = jets.primary_jet_hi

        self.monitoring_muonNonIso = muon.monitoring_muonNonIso_HI
        self.monitoring_muonIso = muon.monitoring_muonIso_HI
        self.monitoring_MSonly = muon.monitoring_MSonly_HI 
        self.monitoring_muonEFFS = muon.monitoring_muonEFFS_HI
        self.monitoring_muon_Support = muon.monitoring_muon_Support_HI

    def set_HLTMonTrigList_cosmic (self):
        self.monitoring_tau = tau.monitoring_tau_cosmic
        self.monitoring_met = met.monitoring_met_cosmic

        self.monitoring_l1jet = jets.monitoring_l1jet_cosmic
        self.monitoring_jet = jets.monitoring_jet_cosmic
        self.primary_l1jet = jets.primary_l1jet_cosmic
        self.primary_jet = jets.primary_jet_cosmic
        

hltmonList = HLTMonTriggerList()
