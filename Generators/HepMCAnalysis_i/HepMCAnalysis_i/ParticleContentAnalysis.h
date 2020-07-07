/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLE_CONTENT_ANALYSIS_H_ 
#define PARTICLE_CONTENT_ANALYSIS_H_ 

#include "baseAnalysis.h"

class TH1D;

/*

 Init() is used to initialise histograms                               
 Process() is used to process the events and do the analysis    
                                                                       
 The ouput (histograms) will be saved in a root file                                                                                                                  
*/

class ParticleContentAnalysis: public baseAnalysis
{

 public:
  ParticleContentAnalysis();
  virtual ~ParticleContentAnalysis();
  
  virtual int Init(double tr_max_eta, double tr_min_pt);
  virtual int Process(HepMC::GenEvent* hepmcevt);



  static const int GeV=1000;

 private:

  void InitPlots();

  inline double getCosTheta(const HepMC::GenParticle* charged_pion, const HepMC::GenParticle*  tau);
  inline double getChargedEnergyFraction(const HepMC::GenParticle* charged_pion, const HepMC::GenParticle* uncharged_pion);

  TH1D *m_evtnr;
  TH1D *m_evtweight;
  TH1D *m_evtweight_zoom;

  //all particles in the event
  TH1D *m_initial_particle_pid;
  TH1D *m_intermediate_particle_pid;
  TH1D *m_final_particle_pid;

  TH1D *m_initial_particle_pid_zoom;
  TH1D *m_intermediate_particle_pid_zoom;
  TH1D *m_final_particle_pid_zoom;

  TH1D *m_particle_status;
  
  //B hadron plots
  TH1D *m_BottomHadron_pt;
  TH1D *m_BottomHadron_eta;
  TH1D *m_BottomHadron_phi;
  TH1D *m_BottomHadron_flightlength; 
  TH1D *m_BottomHadron_status;
  TH1D *m_BottomHadron_decay_multiplicity;
  TH1D *m_BottomHadron_decay_charged_multiplicity;

  TH1D *m_BottomHadron_tight_pt;
  TH1D *m_BottomHadron_tight_eta;
  TH1D *m_BottomHadron_tight_phi;
  TH1D *m_BottomHadron_tight_flightlength; 
  TH1D *m_BottomHadron_tight_status;
  TH1D *m_BottomHadron_tight_decay_multiplicity;
  TH1D *m_BottomHadron_tight_decay_charged_multiplicity;
  
  TH1D *m_BottomMeson_number;
  TH1D *m_BottomBaryon_number;

  //D hadron plots
  TH1D *m_CharmHadron_pt;
  TH1D *m_CharmHadron_eta;
  TH1D *m_CharmHadron_phi;
  TH1D *m_CharmHadron_flightlength; 
  TH1D *m_CharmHadron_status;
  TH1D *m_CharmHadron_decay_multiplicity;
  TH1D *m_CharmHadron_decay_charged_multiplicity;

  TH1D *m_CharmMeson_number;
  TH1D *m_CharmBaryon_number;

  //S hadron plots
  TH1D *m_StrangeHadron_pt;
  TH1D *m_StrangeHadron_unstable_pt;
  TH1D *m_StrangeHadron_eta;
  TH1D *m_StrangeHadron_phi;
  TH1D *m_StrangeHadron_flightlength; 
  TH1D *m_StrangeHadron_status;
  TH1D *m_StrangeHadron_decay_multiplicity;
  TH1D *m_StrangeHadron_decay_charged_multiplicity;

  TH1D *m_StrangeMeson_number;
  TH1D *m_StrangeBaryon_number;

  //tau plots
  TH1D *m_Tau_decay_multiplicity;
  TH1D *m_Tau_decay_charged_multiplicity;
  TH1D *m_Tau_cos_theta;  
  TH1D *m_Tau_charged_energy_fraction;
  TH1D *m_Tau_flightlength;


  TH1D *m_uquark_mass;
  TH1D *m_dquark_mass;
  TH1D *m_cquark_mass;
  TH1D *m_squark_mass;  
  TH1D *m_tquark_mass;
  TH1D *m_bquark_mass;
  TH1D *m_gluon_mass;
  TH1D *m_photon_mass;
  TH1D *m_Zboson_mass;
  TH1D *m_Wboson_mass;

};

#endif      // PARTICLE_CONT_H_ 
