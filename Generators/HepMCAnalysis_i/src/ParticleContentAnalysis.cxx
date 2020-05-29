/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>
#include <stdio.h>
#include "CLHEP/Vector/LorentzVector.h"

// ROOT headers
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TMath.h"

/** Here we state explicitely what is used */
#include "MCUtils/PIDUtils.h"
#include "MCUtils/HepMCEventUtils.h"
#include "MCUtils/HepMCParticleUtils.h"
#include "MCUtils/HepMCVectors.h"
#include "MCUtils/HepMCParticleClassifiers.h"

#include "../HepMCAnalysis_i/ParticleContentAnalysis.h"

namespace MC {
using namespace MCUtils::PID;
using MCUtils::get_weight;
using MCUtils::flightLength;
using MCUtils::isLastReplica;
}


// empty default constructor
ParticleContentAnalysis::ParticleContentAnalysis()
{
  InitPlots();
}

/// empty default destructor
ParticleContentAnalysis::~ParticleContentAnalysis()
{
}

void ParticleContentAnalysis::InitPlots()
{
  //declaration of histograms
  m_evtnr =         new TH1D( "EventNumber",   "Event number" , 100, 0., 1000. );
  m_evtweight = new TH1D("EventWeight",  "event weight",  1000, -5000000.0, 5000000.0);
  m_evtweight_zoom = new TH1D("EventWeight_Zoom",  "event weight",  41, -20.5, 20.5);

  //all particles in the event
  m_initial_particle_pid =              new TH1D(  "Initial_Particle_PID" ,           "PID of initial particles" ,          30001., -15000.5, 15000.5 );
  m_intermediate_particle_pid =         new TH1D(  "Intermediate_Particle_PID" ,           "PID of intermediate particles" ,          30001., -15000.5, 15000.5 );
  m_final_particle_pid =                new TH1D(  "Final_Particle_PID" ,           "PID of final particles" ,          30001., -15000.5, 15000.5 );

  m_initial_particle_pid_zoom =              new TH1D(  "Initial_Particle_PID_zoom" ,           "PID of initial particles" ,          61., -30.5, 30.5 );
  m_intermediate_particle_pid_zoom =         new TH1D(  "Intermediate_Particle_PID_zoom" ,           "PID of intermediate particles" ,          61., -30.5, 30.5 );
  m_final_particle_pid_zoom =                new TH1D(  "Final_Particle_PID_zoom" ,           "PID of final particles" ,          61., -30.5, 30.5 );

  m_particle_status =      new TH1D(  "Particle_Status" ,        "status of particles" ,       151., -0.5, 150.5 );
  //bottom hadron plots
  m_BottomHadron_pt =           new TH1D(  "BottomHadron_Pt" ,            "p_{T} of bottom hadron" ,          200., 0., 400. );
  m_BottomHadron_eta =          new TH1D(  "BottomHadron_Eta" ,           "#eta of bottom hadron" ,           60, -6., 6. );
  m_BottomHadron_phi =          new TH1D(  "BottomHadron_Phi" ,           "#Phi of bottom hadron" ,           48, -3.15, 3.15 );
  m_BottomHadron_flightlength =  new TH1D(  "BottomHadron_FlightLength" ,   "flight length of bottom hadron" ,   100., -0.05, 150.05 );
  m_BottomHadron_status =       new TH1D(  "BottomHadron_Status" ,        "status of bottom hadron" ,         150., -0.05, 150.05 );
  m_BottomHadron_decay_multiplicity =             new TH1D(  "BottomHadron_decay_multiplicity" ,            "multiplicity of bottom hadron decay" ,     11., -0.5, 10.5 );
  m_BottomHadron_decay_charged_multiplicity =     new TH1D(  "BottomHadron_decay_charged_multiplicity" ,    "charged multiplicity of bottom hadron decay" ,             11., -0.5, 10.5 );

  m_BottomHadron_tight_pt =           new TH1D(  "BottomHadron_tight_Pt" ,            "p_{T} of bottom hadron" ,          200., 0., 400. );
  m_BottomHadron_tight_eta =          new TH1D(  "BottomHadron_tight_Eta" ,           "#eta of bottom hadron" ,           60, -6., 6. );
  m_BottomHadron_tight_phi =          new TH1D(  "BottomHadron_tight_Phi" ,           "#Phi of bottom hadron" ,           48, -3.15, 3.15 );
  m_BottomHadron_tight_flightlength =  new TH1D(  "BottomHadron_tight_FlightLength" ,   "flight length of bottom hadron" ,   100., -0.05, 150.05 );
  m_BottomHadron_tight_status =       new TH1D(  "BottomHadron_tight_Status" ,        "status of bottom hadron" ,         150., -0.05, 150.05 );
  m_BottomHadron_tight_decay_multiplicity =             new TH1D(  "BottomHadron_tight_decay_multiplicity" ,            "multiplicity of bottom hadron decay" ,     11., -0.5, 10.5 );
  m_BottomHadron_tight_decay_charged_multiplicity =     new TH1D(  "BottomHadron_tight_decay_charged_multiplicity" ,    "charged multiplicity of bottom hadron decay" ,             11., -0.5, 10.5 );

  m_BottomMeson_number  = new TH1D("NumBottomMeson",     "number of bottom meson",  81, -0.5, 80.5);
  m_BottomBaryon_number = new TH1D("NumBottomBaryon",     "number of bottom baryon",  81, -0.5, 80.5);


  //charmed hadron plots
  m_CharmHadron_pt =           new TH1D(  "CharmHadron_Pt" ,            "p_{T} of charmed hadron" ,           200., 0., 400.);
  m_CharmHadron_eta =          new TH1D(  "CharmHadron_Eta" ,           "#eta of charmed hadron" ,            60, -6., 6.);
  m_CharmHadron_phi =          new TH1D(  "CharmHadron_Phi" ,           "#Phi of charmed hadron" ,            48, -3.15, 3.15);
  m_CharmHadron_flightlength =  new TH1D(  "CharmHadron_FlightLength" ,   "flight length of charmed hadron" ,    100., -0.05, 150.05 );
  m_CharmHadron_status =       new TH1D(  "CharmHadron_Status" ,        "status of charmed hadron" ,          150., -0.05, 150.05 );
  m_CharmHadron_decay_multiplicity =             new TH1D(  "CharmHadron_decay_multiplicity" ,            "multiplicity of charmed hadron decay" ,     11., -0.5, 10.5 );
  m_CharmHadron_decay_charged_multiplicity =     new TH1D(  "CharmHadron_decay_charged_multiplicity" ,    "charged multiplicity of charmed hadron decay" ,             11., -0.5, 10.5 );

  m_CharmMeson_number  = new TH1D("NumCharmMeson",     "number of charm meson",  81, -0.5, 80.5);
  m_CharmBaryon_number = new TH1D("NumCharmBaryon",     "number of charm baryon",  81, -0.5, 80.5);


  //strange hadron plots
  m_StrangeHadron_pt =           new TH1D(  "StrangeHadron_Pt" ,            "p_{T} of strange hadron" ,                      200., 0., 400. );
  m_StrangeHadron_unstable_pt =  new TH1D(  "StrangeHadron_Unstable_Pt" ,   "p_{T} of the last strange hadron (unstable)" ,  200., 0., 400. );
  m_StrangeHadron_eta =          new TH1D(  "StrangeHadron_Eta" ,           "#eta of strange hadron" ,            60, -6., 6.);
  m_StrangeHadron_phi =          new TH1D(  "StrangeHadron_Phi" ,           "#Phi of strange hadron" ,            48, -3.15, 3.15);
  m_StrangeHadron_flightlength =  new TH1D(  "StrangeHadron_FlightLength" ,   "flight length of strange hadron" ,    100., -0.05, 150.05 );
  m_StrangeHadron_status =       new TH1D(  "StrangeHadron_Status" ,        "status of strange hadron" ,          150., -0.05, 150.05 );
  m_StrangeHadron_decay_multiplicity =             new TH1D(  "StrangeHadron_decay_multiplicity" ,            "multiplicity of strange hadron decay" ,     11., -0.5, 10.5 );
  m_StrangeHadron_decay_charged_multiplicity =     new TH1D(  "StrangeHadron_decay_charged_multiplicity" ,    "charged multiplicity of strange hadron decay" ,             11., -0.5, 10.5 );

  m_StrangeMeson_number  = new TH1D("NumStrangeMeson",     "number of strange meson",  81, -0.5, 80.5);
  m_StrangeBaryon_number = new TH1D("NumStrangeBaryon",     "number of strange baryon",  81, -0.5, 80.5);


  //tau plots
  m_Tau_decay_multiplicity =             new TH1D(  "Tau_decay_multiplicity" ,            "multiplicity of tau decay" ,     11., -0.5, 10.5 );
  m_Tau_decay_charged_multiplicity =     new TH1D(  "Tau_charged_decay_multiplicity" ,            "charged multiplicity of tau decay" ,     11., -0.5, 10.5 );
  m_Tau_cos_theta =                      new TH1D(  "Tau_cos_theta" ,                     "cos(#theta)" ,      80., -1.0, 1.0   );
  m_Tau_charged_energy_fraction =        new TH1D(  "Tau_charged_energy_fraction" ,            "charged energy fraction" ,      40., -1.0, 1.0 );
  m_Tau_flightlength =                   new TH1D(  "Tau_FlightLength" ,   "flight length of tau" ,    100., -0.05, 150.05 );

  m_uquark_mass =  new TH1D(  "uquark_mass" ,      "mass of u-quark" ,     101., -0.25, 50.25 );
  m_dquark_mass =  new TH1D(  "dquark_mass" ,      "mass of d-quark" ,     101., -0.25, 50.25 );
  m_cquark_mass =  new TH1D(  "cquark_mass" ,      "mass of c-quark" ,     101., -0.25, 50.25 );
  m_squark_mass =  new TH1D(  "squark_mass" ,      "mass of s-quark" ,     101., -0.25, 50.25 );
  m_tquark_mass =  new TH1D(  "tquark_mass" ,      "mass of t-quark" ,     201., 99.5, 200.5 );
  m_bquark_mass =  new TH1D(  "bquark_mass" ,      "mass of b-quark" ,     101., -0.25, 50.25 );
  m_gluon_mass =   new TH1D(  "gluon_mass" ,       "mass of gluon" ,       101., -0.25, 50.25 );
  m_photon_mass =  new TH1D(  "photon_mass" ,      "mass of photon" ,      101., -0.25, 50.25 );
  m_Zboson_mass =  new TH1D(  "Zboson_mass" ,      "mass of Z boson" ,     100., 50., 150  );
  m_Wboson_mass =  new TH1D(  "Wboson_mass" ,      "mass of W boson" ,     100., 50., 150  );

  m_evtnr -> Sumw2();
  m_evtweight -> Sumw2();
  m_evtweight_zoom -> Sumw2();

  //all particles in the event
  m_initial_particle_pid -> Sumw2();
  m_intermediate_particle_pid -> Sumw2();
  m_final_particle_pid -> Sumw2();

  m_initial_particle_pid_zoom -> Sumw2();
  m_intermediate_particle_pid_zoom -> Sumw2();
  m_final_particle_pid_zoom -> Sumw2();

  m_particle_status -> Sumw2();

  m_BottomHadron_pt -> Sumw2();
  m_BottomHadron_eta -> Sumw2();
  m_BottomHadron_phi -> Sumw2();
  m_BottomHadron_flightlength -> Sumw2();
  m_BottomHadron_status -> Sumw2();
  m_BottomHadron_decay_multiplicity -> Sumw2();
  m_BottomHadron_decay_charged_multiplicity -> Sumw2();

  m_BottomHadron_tight_pt -> Sumw2();
  m_BottomHadron_tight_eta -> Sumw2();
  m_BottomHadron_tight_phi -> Sumw2();
  m_BottomHadron_tight_flightlength -> Sumw2();
  m_BottomHadron_tight_status -> Sumw2();
  m_BottomHadron_tight_decay_multiplicity -> Sumw2();
  m_BottomHadron_tight_decay_charged_multiplicity -> Sumw2();

  m_BottomMeson_number -> Sumw2();
  m_BottomBaryon_number -> Sumw2();

  m_CharmHadron_pt -> Sumw2();
  m_CharmHadron_eta -> Sumw2();
  m_CharmHadron_phi -> Sumw2();
  m_CharmHadron_flightlength -> Sumw2();
  m_CharmHadron_status -> Sumw2();
  m_CharmHadron_decay_multiplicity -> Sumw2();
  m_CharmHadron_decay_charged_multiplicity -> Sumw2();

  m_CharmMeson_number -> Sumw2();
  m_CharmBaryon_number -> Sumw2();

  m_StrangeHadron_pt -> Sumw2();
  m_StrangeHadron_unstable_pt -> Sumw2();
  m_StrangeHadron_eta -> Sumw2();
  m_StrangeHadron_phi -> Sumw2();
  m_StrangeHadron_flightlength -> Sumw2();
  m_StrangeHadron_status -> Sumw2();
  m_StrangeHadron_decay_multiplicity -> Sumw2();
  m_StrangeHadron_decay_charged_multiplicity -> Sumw2();

  m_StrangeMeson_number -> Sumw2();
  m_StrangeBaryon_number -> Sumw2();

  m_Tau_decay_multiplicity -> Sumw2();
  m_Tau_decay_charged_multiplicity -> Sumw2();
  m_Tau_cos_theta  -> Sumw2();
  m_Tau_charged_energy_fraction -> Sumw2();
  m_Tau_flightlength -> Sumw2();

  m_uquark_mass -> Sumw2();
  m_dquark_mass -> Sumw2();
  m_cquark_mass -> Sumw2();
  m_squark_mass -> Sumw2();
  m_tquark_mass -> Sumw2();
  m_bquark_mass -> Sumw2();
  m_gluon_mass -> Sumw2();
  m_photon_mass -> Sumw2();
  m_Zboson_mass -> Sumw2();
  m_Wboson_mass -> Sumw2();
}

int ParticleContentAnalysis::Init(double tr_max_eta, double tr_min_pt)
{
  // specify default eta cut
  m_max_eta = tr_max_eta;

  // specify default pt cut
  m_min_pt = tr_min_pt;

  // default Output file name
  m_outputFileName = "ParticleContentAnalysis.root";
  m_outputRootDir = "PartCont";

  //declaration of histograms
  m_evtnr =         initHist( std::string( "EventNumber" ), std::string( "Event number" ), std::string("Eventnumber"), 100, 0., 1000. );
  m_evtweight =     initHist( std::string( "EventWeight"),  std::string( "event weight"), std::string("weight"),  1000, -5000000.0, 5000000.0);
  m_evtweight_zoom = initHist(std::string("EventWeight_Zoom"),  std::string("event weight"), std::string("weight"),  41, -20.5, 20.5);

  //all particles in the event

  m_initial_particle_pid =              initHist(  std::string( "Initial_Particle_PID" ) ,           std::string( "PID of initial particles") ,   std::string(" pid" ),       30001., -15000.5, 15000.5 );
  m_intermediate_particle_pid =         initHist(  std::string( "Intermediate_Particle_PID" ) ,           std::string( "PID of intermediate particles" ),   std::string(" pid" ),       30001., -15000.5, 15000.5 );
  m_final_particle_pid =                initHist(  std::string( "Final_Particle_PID") ,           std::string( "PID of final particles" ),    std::string(" pid" ),      30001., -15000.5, 15000.5 );

  m_initial_particle_pid_zoom =              initHist(  std::string( "Initial_Particle_PID_zoom" ) ,           std::string( "PID of initial particles") ,   std::string(" pid" ),   61., -30.5, 30.5 );
  m_intermediate_particle_pid_zoom =         initHist(  std::string( "Intermediate_Particle_PID_zoom" ) ,           std::string( "PID of intermediate particles" ),   std::string(" pid" ),  61., -30.5, 30.5 );
  m_final_particle_pid_zoom =                initHist(  std::string( "Final_Particle_PID_zoom") ,           std::string( "PID of final particles" ),    std::string(" pid" ),    61., -30.5, 30.5  );

  m_particle_status =      initHist( std::string( "Particle_Status" ),        std::string( "status of particles" ),   std::string("status" ),     151., -0.5, 150.5 );

  //bottom hadron plots
  m_BottomHadron_pt =           initHist( std::string( "BottomHadron_Pt" ),           std::string( "p_{T} of bottom hadron" ),        std::string( "p_{T} [GeV]" ),   200., 0., 400. );
  m_BottomHadron_eta =          initHist( std::string( "BottomHadron_Eta" ),          std::string( "#eta of bottom hadron" ),         std::string(" #eta" ),          60, -6., 6. );
  m_BottomHadron_phi =          initHist( std::string( "BottomHadron_Phi" ),          std::string( "#Phi of bottom hadron" ),         std::string( "#Phi" ),          48, -3.15, 3.15 );
  m_BottomHadron_flightlength =  initHist( std::string( "BottomHadron_FlightLength" ),  std::string( "flight length of bottom hadron" ), std::string( "length" ),         100., -0.05, 150.05 );
  m_BottomHadron_status =       initHist( std::string( "BottomHadron_Status" ),        std::string( "status of bottom hadron" ),      std::string("status" ),      151., -0.5, 150.5  );
  m_BottomHadron_decay_multiplicity =             initHist( std::string( "BottomHadron_decay_multiplicity" ),            std::string( "multiplicity of bottom hadron decay" ),   std::string("N" ),   11., -0.5, 10.5 );
  m_BottomHadron_decay_charged_multiplicity =     initHist( std::string( "BottomHadron_decay_charged_multiplicity" ),    std::string( "charged multiplicity of bottom hadron decay" ),           std::string("N" ),      11., -0.5, 10.5 );

  m_BottomHadron_tight_pt =           initHist( std::string( "BottomHadron_tight_Pt" ),           std::string( "p_{T} of bottom hadron" ),        std::string( "p_{T} [GeV]" ),   200., 0., 400. );
  m_BottomHadron_tight_eta =          initHist( std::string( "BottomHadron_tight_Eta" ),          std::string( "#eta of bottom hadron" ),         std::string(" #eta" ),          60, -6., 6. );
  m_BottomHadron_tight_phi =          initHist( std::string( "BottomHadron_tight_Phi" ),          std::string( "#Phi of bottom hadron" ),         std::string( "#Phi" ),          48, -3.15, 3.15 );
  m_BottomHadron_tight_flightlength =  initHist( std::string( "BottomHadron_tight_FlightLength" ),  std::string( "flight length of bottom hadron" ), std::string( "length" ),         100., -0.05, 150.05 );
  m_BottomHadron_tight_status =       initHist( std::string( "BottomHadron_tight_Status" ),        std::string( "status of bottom hadron" ),      std::string("status" ),      151., -0.5, 150.5  );
  m_BottomHadron_tight_decay_multiplicity =             initHist( std::string( "BottomHadron_tight_decay_multiplicity" ),            std::string( "multiplicity of bottom hadron decay" ),   std::string("N" ),   11., -0.5, 10.5 );
  m_BottomHadron_tight_decay_charged_multiplicity =     initHist( std::string( "BottomHadron_tight_decay_charged_multiplicity" ),    std::string( "charged multiplicity of bottom hadron decay" ),           std::string("N" ),      11., -0.5, 10.5 );

  m_BottomMeson_number  = initHist( std::string( "NumBottomMeson"),     std::string( "number of bottom meson"), std::string( "number"), 81, -0.5, 80.5);
  m_BottomBaryon_number = initHist( std::string( "NumBottomBaryon"),     std::string( "number of bottom baryon"), std::string( "number"),  81, -0.5, 80.5);

  //charmed hadron plots
  m_CharmHadron_pt =           initHist( std::string( "CharmHadron_Pt" ),           std::string( "p_{T} of charmed hadron" ),        std::string( "p_{T} [GeV]" ),   200., 0., 400.);
  m_CharmHadron_eta =          initHist( std::string( "CharmHadron_Eta" ),          std::string( "#eta of charmed hadron" ),         std::string( "#eta" ),          60, -6., 6.);
  m_CharmHadron_phi =          initHist( std::string( "CharmHadron_Phi" ),          std::string( "#Phi of charmed hadron" ),         std::string( "#Phi" ),          48, -3.15, 3.15);
  m_CharmHadron_flightlength =  initHist( std::string( "CharmHadron_FlightLength" ),  std::string( "flight length of charmed hadron" ), std::string( "length" ),         100., -0.05, 150.05 );
  m_CharmHadron_status =       initHist( std::string( "CharmHadron_Status" ),        std::string( "status of charmed hadron" ),      std::string("status" ),      151., -0.5, 150.5  );
  m_CharmHadron_decay_multiplicity =             initHist( std::string( "CharmHadron_decay_multiplicity" ),            std::string( "multiplicity of charmed hadron decay" ),   std::string("N" ),   11., -0.5, 10.5 );
  m_CharmHadron_decay_charged_multiplicity =     initHist( std::string( "CharmHadron_decay_charged_multiplicity" ),    std::string( "charged multiplicity of charmed hadron decay" ),           std::string("N" ),      11., -0.5, 10.5 );
  m_CharmMeson_number  = initHist( std::string( "NumCharmMeson"),     std::string( "number of charm meson"), std::string( "number"), 81, -0.5, 80.5);
  m_CharmBaryon_number = initHist( std::string( "NumCharmBaryon"),     std::string( "number of charm baryon"), std::string( "number"),  81, -0.5, 80.5);

  //strange hadron plots
  m_StrangeHadron_pt =           initHist( std::string( "StrangeHadron_Pt" ),           std::string( "p_{T} of strange hadron" ),        std::string( "p_{T} [GeV]" ),             200., 0., 400. );
  m_StrangeHadron_unstable_pt =  initHist( std::string( "StrangeHadron_Unstable_Pt" ),  std::string("p_{T} of the last strange hadron (unstable)" ), std::string( "p_{T} [GeV]" ), 200.,0.,400. );
  m_StrangeHadron_eta =          initHist( std::string( "StrangeHadron_Eta" ),          std::string( "#eta of strange hadron" ),         std::string( "#eta" ),          60, -6., 6.);
  m_StrangeHadron_phi =          initHist( std::string( "StrangeHadron_Phi" ),          std::string( "#Phi of strange hadron" ),         std::string( "#Phi" ),          48, -3.15, 3.15);
  m_StrangeHadron_flightlength =  initHist( std::string( "StrangeHadron_FlightLength" ),  std::string( "flight length of strange hadron" ), std::string( "length" ),         100., -0.05, 150.05 );
  m_StrangeHadron_status =       initHist( std::string( "StrangeHadron_Status" ),        std::string( "status of strange hadron" ),      std::string("status" ),      151., -0.5, 150.5  );
  m_StrangeHadron_decay_multiplicity =             initHist( std::string( "StrangeHadron_decay_multiplicity" ),            std::string( "multiplicity of strange hadron decay" ),   std::string("N" ),   11., -0.5, 10.5 );
  m_StrangeHadron_decay_charged_multiplicity =     initHist( std::string( "StrangeHadron_decay_charged_multiplicity" ),    std::string( "charged multiplicity of strange hadron decay" ),           std::string("N" ),      11., -0.5, 10.5 );

  m_StrangeMeson_number  = initHist( std::string( "NumStrangeMeson"),     std::string( "number of strange meson"), std::string( "number"), 81, -0.5, 80.5);
  m_StrangeBaryon_number = initHist( std::string( "NumStrangeBaryon"),     std::string( "number of strange baryon"), std::string( "number"),  81, -0.5, 80.5);

  //tau plots
  m_Tau_decay_multiplicity =             initHist(  std::string( "Tau_decay_multiplicity") ,            std::string("multiplicity of tau decay") ,   std::string("N" ),    11., -0.5, 10.5 );
  m_Tau_decay_charged_multiplicity =     initHist(  std::string( "Tau_charged_decay_multiplicity") ,    std::string("charged multiplicity of tau decay") ,   std::string("N" ),    11., -0.5, 10.5 );
  m_Tau_cos_theta =                      initHist(  std::string( "Tau_cos_theta") ,                     std::string("cos(#theta)") ,            std::string("cos(#theta)") ,          80., -1.0, 1.0  );
  m_Tau_charged_energy_fraction =        initHist(  std::string( "Tau_charged_energy_fraction") ,       std::string("charged energy fraction") , std::string("E_{charged}/E") ,      40., -1.0, 1.0 );
  m_Tau_flightlength =                   initHist(  std::string( "Tau_FlightLength") ,   std::string( "flight length of tau") ,  std::string( "length" ),    100., -0.05, 150.05 );

  m_uquark_mass =  initHist( std::string( "uquark_mass" ),      std::string( "mass of u-quark" ),  std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_dquark_mass =  initHist( std::string( "dquark_mass" ),      std::string( "mass of d-quark" ),  std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_cquark_mass =  initHist( std::string( "cquark_mass" ),      std::string( "mass of c-quark" ),  std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_squark_mass =  initHist( std::string( "squark_mass" ),      std::string( "mass of s-quark" ),  std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_tquark_mass =  initHist( std::string( "tquark_mass" ),      std::string( "mass of t-quark" ),  std::string( "mass [GeV]" ),   201., 99.5, 200.5  );
  m_bquark_mass =  initHist( std::string( "bquark_mass" ),      std::string( "mass of b-quark" ),  std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_gluon_mass =   initHist( std::string( "gluon_mass" ),       std::string( "mass of gluon" ),    std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_photon_mass =  initHist( std::string( "photon_mass" ),      std::string( "mass of photon" ),   std::string( "mass [GeV]" ),   101., -0.25, 50.25 );
  m_Zboson_mass =  initHist( std::string( "Zboson_mass" ),      std::string( "mass of Z boson" ),  std::string( "mass [GeV]" ),   100., 50., 150 );
  m_Wboson_mass =  initHist( std::string( "Wboson_mass" ),      std::string( "mass of W boson" ),  std::string( "mass [GeV]" ),   100., 50., 150 );


  m_evtnr -> Sumw2();
  m_evtweight -> Sumw2();
  m_evtweight_zoom -> Sumw2();

  //all particles in the event
  m_initial_particle_pid -> Sumw2();
  m_intermediate_particle_pid -> Sumw2();
  m_final_particle_pid -> Sumw2();

  m_initial_particle_pid_zoom -> Sumw2();
  m_intermediate_particle_pid_zoom -> Sumw2();
  m_final_particle_pid_zoom -> Sumw2();

  m_particle_status -> Sumw2();

  m_BottomHadron_pt -> Sumw2();
  m_BottomHadron_eta -> Sumw2();
  m_BottomHadron_phi -> Sumw2();
  m_BottomHadron_flightlength -> Sumw2();
  m_BottomHadron_status -> Sumw2();
  m_BottomHadron_decay_multiplicity -> Sumw2();
  m_BottomHadron_decay_charged_multiplicity -> Sumw2();

  m_BottomHadron_tight_pt -> Sumw2();
  m_BottomHadron_tight_eta -> Sumw2();
  m_BottomHadron_tight_phi -> Sumw2();
  m_BottomHadron_tight_flightlength -> Sumw2();
  m_BottomHadron_tight_status -> Sumw2();
  m_BottomHadron_tight_decay_multiplicity -> Sumw2();
  m_BottomHadron_tight_decay_charged_multiplicity -> Sumw2();

  m_BottomMeson_number -> Sumw2();
  m_BottomBaryon_number -> Sumw2();

  m_CharmHadron_pt -> Sumw2();
  m_CharmHadron_eta -> Sumw2();
  m_CharmHadron_phi -> Sumw2();
  m_CharmHadron_flightlength -> Sumw2();
  m_CharmHadron_status -> Sumw2();
  m_CharmHadron_decay_multiplicity -> Sumw2();
  m_CharmHadron_decay_charged_multiplicity -> Sumw2();

  m_CharmMeson_number -> Sumw2();
  m_CharmBaryon_number -> Sumw2();

  m_StrangeHadron_pt -> Sumw2();
  m_StrangeHadron_unstable_pt -> Sumw2();
  m_StrangeHadron_eta -> Sumw2();
  m_StrangeHadron_phi -> Sumw2();
  m_StrangeHadron_flightlength -> Sumw2();
  m_StrangeHadron_status -> Sumw2();
  m_StrangeHadron_decay_multiplicity -> Sumw2();
  m_StrangeHadron_decay_charged_multiplicity -> Sumw2();

  m_StrangeMeson_number -> Sumw2();
  m_StrangeBaryon_number -> Sumw2();

  m_Tau_decay_multiplicity -> Sumw2();
  m_Tau_decay_charged_multiplicity -> Sumw2();
  m_Tau_cos_theta  -> Sumw2();
  m_Tau_charged_energy_fraction -> Sumw2();
  m_Tau_flightlength -> Sumw2();

  m_uquark_mass -> Sumw2();
  m_dquark_mass -> Sumw2();
  m_cquark_mass -> Sumw2();
  m_squark_mass -> Sumw2();
  m_tquark_mass -> Sumw2();
  m_bquark_mass -> Sumw2();
  m_gluon_mass -> Sumw2();
  m_photon_mass -> Sumw2();
  m_Zboson_mass -> Sumw2();
  m_Wboson_mass -> Sumw2();

  return true;
}

int ParticleContentAnalysis::Process(HepMC::GenEvent* hepmcevt)
{

  double weight;

  HepMC::GenVertex* decay_vertex;

  HepMC::GenParticle* tau = nullptr;
  HepMC::GenParticle* charged_pion = nullptr;
  HepMC::GenParticle* uncharged_pion = nullptr;
  HepMC::GenParticle* tau_neutrino = nullptr;

  //tight b-hadron cuts
  double tight_bottom_pt = 5;
  double tight_bottom_eta = 2.5;

  //counter variable
  int cmult=0;

  int NumStrangeMeson = 0, NumStrangeBaryon = 0;
  int NumCharmMeson = 0, NumCharmBaryon = 0;
  int NumBottomMeson = 0, NumBottomBaryon = 0;

  // Fill the eventnumber
  m_evtnr->Fill(hepmcevt->event_number());

  weight = MC::get_weight(hepmcevt, 0 );
  m_evtweight -> Fill(weight);
  m_evtweight_zoom -> Fill(weight);

  bool isTightBottomHadron = false;

  // loop over the particles and select pid and pt
  for ( HepMC::GenEvent::particle_const_iterator p = hepmcevt->particles_begin(); p != hepmcevt->particles_end(); ++p ){

    if (((*p) -> status()) == 3) m_initial_particle_pid ->Fill((*p) -> pdg_id(), weight);
    if (((*p) -> status()) == 2) m_intermediate_particle_pid ->Fill((*p) -> pdg_id(), weight);
    if (((*p) -> status()) == 1) m_final_particle_pid ->Fill((*p) -> pdg_id(), weight);

    if (((*p) -> status()) == 3) m_initial_particle_pid_zoom ->Fill((*p) -> pdg_id(), weight);
    if (((*p) -> status()) == 2) m_intermediate_particle_pid_zoom ->Fill((*p) -> pdg_id(), weight);
    if (((*p) -> status()) == 1) m_final_particle_pid_zoom ->Fill((*p) -> pdg_id(), weight);

    m_particle_status -> Fill((*p) -> status(), weight);

    if ((MC::isBottomHadron((*p)->pdg_id())) && MC::isLastReplica(*p))
      {
	m_BottomHadron_pt -> Fill((*p) -> momentum().perp(), weight);
	m_BottomHadron_eta -> Fill((*p) -> momentum().eta(), weight);
	m_BottomHadron_phi -> Fill((*p) -> momentum().phi(), weight);
	double bflightlength = MC::flightLength(*p);
	m_BottomHadron_flightlength -> Fill(bflightlength, weight);
	m_BottomHadron_status -> Fill((*p) -> status(), weight);

	if((*p) -> momentum().perp() > tight_bottom_pt && std::abs((*p) -> momentum().eta())< tight_bottom_eta) isTightBottomHadron = true;
	if(isTightBottomHadron){
	  m_BottomHadron_tight_pt -> Fill((*p) -> momentum().perp(), weight);
	  m_BottomHadron_tight_eta -> Fill((*p) -> momentum().eta(), weight);
	  m_BottomHadron_tight_phi -> Fill((*p) -> momentum().phi(), weight);
	  m_BottomHadron_tight_flightlength -> Fill(bflightlength, weight);
	  m_BottomHadron_tight_status -> Fill((*p) -> status(), weight);
	}



	decay_vertex = (*p) -> end_vertex();
	Int_t NBD = 0;
	Int_t NBCD = 0;
	if (decay_vertex){
	for (HepMC::GenVertex::particle_iterator current_daughter = decay_vertex -> particles_begin(HepMC::children);
	     current_daughter != decay_vertex -> particles_end(HepMC::children); ++current_daughter)
	  {
	    NBD++;
	    if (MC::isCharged((*current_daughter)->pdg_id()))NBCD++;
	  }
	m_BottomHadron_decay_multiplicity -> Fill(NBD, weight);
	m_BottomHadron_decay_charged_multiplicity -> Fill(NBCD, weight);
	if(isTightBottomHadron) m_BottomHadron_tight_decay_multiplicity -> Fill(NBD, weight);
	if(isTightBottomHadron) m_BottomHadron_tight_decay_charged_multiplicity -> Fill(NBCD, weight);
	}
      }

    if ((MC::isBottomMeson((*p)->pdg_id()))  && MC::isLastReplica(*p))   NumBottomMeson++;
    if ((MC::isBottomBaryon((*p)->pdg_id()))  && MC::isLastReplica(*p))  NumBottomBaryon++;


    if ((MC::isCharmHadron((*p)->pdg_id())) && MC::isLastReplica(*p))
      {
	m_CharmHadron_pt -> Fill((*p) -> momentum().perp(), weight);
	m_CharmHadron_eta -> Fill((*p) -> momentum().eta(), weight);
	m_CharmHadron_phi -> Fill((*p) -> momentum().phi(), weight);
	double dflightlength = MC::flightLength(*p);
	m_CharmHadron_flightlength -> Fill(dflightlength, weight);
	m_CharmHadron_status -> Fill((*p) -> status(), weight);

	decay_vertex = (*p) -> end_vertex();

	Int_t NDD = 0;
	Int_t NDCD = 0;
	if (decay_vertex){
	for (HepMC::GenVertex::particle_iterator current_daughter = decay_vertex -> particles_begin(HepMC::children); current_daughter != decay_vertex -> particles_end(HepMC::children); ++current_daughter)
	  {
	    NDD++;
	    if (MC::isCharged((*current_daughter)->pdg_id()))NDCD++;
	  }
	m_CharmHadron_decay_multiplicity -> Fill(NDD, weight);
	m_CharmHadron_decay_charged_multiplicity -> Fill(NDCD, weight);
	}
      }

    if ((MC::isCharmMeson((*p)->pdg_id()))  && MC::isLastReplica(*p))   NumCharmMeson++;
    if ((MC::isCharmBaryon((*p)->pdg_id()))  && MC::isLastReplica(*p))  NumCharmBaryon++;


    if ((MC::isStrangeHadron((*p)->pdg_id()))  && MC::isLastReplica(*p))
      {
	if (!((*p)->status()==1)){
	  m_StrangeHadron_unstable_pt -> Fill((*p) -> momentum().perp(), weight);
	}
	m_StrangeHadron_pt -> Fill((*p) -> momentum().perp(), weight);
	m_StrangeHadron_eta -> Fill((*p) -> momentum().eta(), weight);
	m_StrangeHadron_phi -> Fill((*p) -> momentum().phi(), weight);
	double sflightlength = MC::flightLength(*p);
	m_StrangeHadron_flightlength -> Fill(sflightlength, weight);
	m_StrangeHadron_status -> Fill((*p) -> status(), weight);

	decay_vertex = (*p) -> end_vertex();

	Int_t NSD = 0;
	Int_t NSCD = 0;

	if (decay_vertex){
	for (HepMC::GenVertex::particle_iterator current_daughter = decay_vertex -> particles_begin(HepMC::children); current_daughter != decay_vertex -> particles_end(HepMC::children); ++current_daughter)
	  {
	    NSD++;
	    if (MC::isCharged((*current_daughter)->pdg_id()))NSCD++;
	  }
	m_StrangeHadron_decay_multiplicity -> Fill(NSD, weight);
	m_StrangeHadron_decay_charged_multiplicity -> Fill(NSCD, weight);
	}

      }

    if ((MC::isStrangeMeson((*p)->pdg_id()))  && MC::isLastReplica(*p))   NumStrangeMeson++;
    if ((MC::isStrangeBaryon((*p)->pdg_id()))  && MC::isLastReplica(*p))  NumStrangeBaryon++;


    if (  std::abs((*p) -> pdg_id()) == 15  && MC::isLastReplica(*p) )
    {
      Int_t NTauDecay = 0;
      Int_t NTauChargedDecay = 0;

      tau = (*p);

      double tauflightlength = MC::flightLength(*p);
      m_Tau_flightlength -> Fill(tauflightlength);

      decay_vertex = (*p) -> end_vertex();
      if (decay_vertex){
	for (HepMC::GenVertex::particle_iterator current_daughter = decay_vertex -> particles_begin(HepMC::children); current_daughter != decay_vertex -> particles_end(HepMC::children); ++current_daughter)
	  {
	    if (std::abs((*current_daughter) -> pdg_id()) == 16) tau_neutrino = (*current_daughter);
	    if (std::abs((*current_daughter) -> pdg_id()) == 211) charged_pion = (*current_daughter);
	    if (std::abs((*current_daughter) -> pdg_id()) == 111) uncharged_pion = (*current_daughter);

	    NTauDecay++;
	    if (MC::isCharged((*current_daughter)->pdg_id())) NTauChargedDecay++;
	  }

	m_Tau_decay_multiplicity -> Fill(NTauDecay, weight);
	m_Tau_decay_charged_multiplicity -> Fill(NTauChargedDecay, weight);



	if ((tau_neutrino) && (charged_pion) && (NTauDecay == 2)){
	  m_Tau_cos_theta -> Fill(getCosTheta(charged_pion, tau), weight);
	}

	if ((tau_neutrino) && (charged_pion) && (uncharged_pion) && (NTauDecay == 3)){
	  m_Tau_charged_energy_fraction -> Fill(getChargedEnergyFraction(charged_pion, uncharged_pion), weight);
	}
      }
    }

    if ( std::abs((*p) -> pdg_id()) == 2) m_uquark_mass -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 1) m_dquark_mass -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 4) m_cquark_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 3) m_squark_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 6) m_tquark_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 5) m_bquark_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( (std::abs((*p) -> pdg_id()) == 21) || (std::abs((*p) -> pdg_id()) == 9) ) m_gluon_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 22) m_photon_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 23) m_Zboson_mass  -> Fill((*p) -> momentum().m(), weight);
    if ( std::abs((*p) -> pdg_id()) == 24) m_Wboson_mass  -> Fill((*p) -> momentum().m(), weight);


    cmult++;
  } // end of loop over particles


  m_StrangeMeson_number -> Fill(NumStrangeMeson, weight);
  m_StrangeBaryon_number -> Fill(NumStrangeBaryon, weight);

  m_CharmMeson_number -> Fill(NumCharmMeson, weight);
  m_CharmBaryon_number -> Fill(NumCharmBaryon, weight);

  m_BottomMeson_number -> Fill(NumBottomMeson, weight);
  m_BottomBaryon_number -> Fill(NumBottomBaryon, weight);

  return true;
} //end Process


inline double ParticleContentAnalysis::getCosTheta(const HepMC::GenParticle*  charged_pion, const HepMC::GenParticle*   tau){

  //make a copy of charged_pion
  //(maybe you need charge_pion later in your code again, therefore you do not want to boost it)
  const HepMC::GenParticle*  copy_of_charged_pion = charged_pion;

  CLHEP::HepLorentzVector charged_pion_vector(0,0,0,0);
  CLHEP::HepLorentzVector tau_vector(0,0,0,0);

  charged_pion_vector.setPx(copy_of_charged_pion -> momentum().px());
  charged_pion_vector.setPy(copy_of_charged_pion -> momentum().py());
  charged_pion_vector.setPz(copy_of_charged_pion -> momentum().pz());
  charged_pion_vector.setE(copy_of_charged_pion -> momentum().e());

  tau_vector.setPx(tau -> momentum().px());
  tau_vector.setPy(tau -> momentum().py());
  tau_vector.setPz(tau -> momentum().pz());
  tau_vector.setE(tau -> momentum().e());

  //boost copy_of_charged_pion to the rest frame of the tau lepton
  charged_pion_vector.boost(-tau_vector.boostVector());

  //calculate theta defined as above
  double cos_theta;
  cos_theta = std::cos (charged_pion_vector.angle(tau_vector));

  return cos_theta;
}

inline double ParticleContentAnalysis::getChargedEnergyFraction(const HepMC::GenParticle*  charged_pion, const HepMC::GenParticle*  uncharged_pion){

  //calculate charged energy fraction
  double charged_energy_fraction;
  charged_energy_fraction = (charged_pion -> momentum().e() - uncharged_pion -> momentum().e() )/(charged_pion -> momentum().e() + uncharged_pion -> momentum().e() );

  return charged_energy_fraction;
}
