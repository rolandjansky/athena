/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/TestHepMC.h"
#include "GaudiKernel/DataSvc.h"
#include "TruthUtils/PIDHelpers.h"

// For find
#include <algorithm>

using namespace std;


TestHepMC::TestHepMC(const string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator),
    m_thistSvc("THistSvc", name)
{
  declareProperty("PdgToSearch",      m_pdg = 15); //< @todo This test is a bit weirdly specific to taus
  declareProperty("CmEnergy",         m_cm_energy = -1); // in MeV, -1 = get from event
  declareProperty("MaxTransVtxDisp",  m_max_dist_trans = 100.); // mm
  declareProperty("MaxVtxDisp",       m_max_dist = 1000.); // mm;
  declareProperty("EnergyDifference", m_energy_diff = 1000.); // MeV
  declareProperty("EnergyDifferenceError", m_max_energy_diff = 100000.); // MeV
  declareProperty("CmeDifference", m_cme_diff = 1.); // MeV
  declareProperty("DumpEvent",        m_dumpEvent = false);
  declareProperty("MinTau",           m_min_tau = 1/300.); // ns; corresponds to 1mm
  declareProperty("MaxNonG4Energy",   m_nonG4_energy_threshold = 100.); //MeV
  declareProperty("TauEffThreshold",  m_tau_eff_threshold = 0.1); // fraction 
  declareProperty("EffWarnThreshold", m_eff_warn_threshold=0.99); // fraction
  declareProperty("EffFailThreshold", m_eff_fail_threshold=0.98); // fraction
  declareProperty("AccuracyMargin",   m_accur_margin=0.); //MeV

  declareProperty("G4ExtraWhiteFile", m_paramFile       = "g4_extrawhite.param" );
  declareProperty("UnknownPDGIDFile", m_unknownPDGIDFile = "pdgid_extras.txt" );

  declareProperty("NoDecayVertexStatuses", m_vertexStatuses );

  declareProperty("BeamEnergyTest",           m_beamEnergyTest = true); //switching off inactive
  declareProperty("VtxNaNTest",               m_vtxNaNTest = true);
  declareProperty("VtxDisplacedTest",         m_vtxDisplacedTest = true);
  declareProperty("MomNaNTest",               m_momNaNTest = true);
  declareProperty("LifeTimeTest",             m_lifeTimeTest = true);
  declareProperty("EnergyG4Test",             m_energyG4Test = true);
  declareProperty("EnergyImbalanceTest",      m_energyImbalanceTest = true);
  declareProperty("MomImbalanceTest",         m_momImbalanceTest = true);
  declareProperty("NegativeEnergyTest",       m_negativeEnergyTest = true);
  declareProperty("TachyonsTest",             m_tachyonsTest = true);
  declareProperty("UnstableNoVtxTest",        m_unstableNoVtxTest = true);
  declareProperty("Pi0NoVtxTest",             m_pi0NoVtxTest = true);
  declareProperty("UndisplacedDaughtersTest", m_undisplacedDaughtersTest = true);
  declareProperty("UknownPDGIDTest",          m_unknownPDGIDTest = true);

  m_vertexStatuses.push_back( 1 );
  m_vertexStatuses.push_back( 3 );
  m_vertexStatuses.push_back( 4 );


  declareProperty("THistSvc", m_thistSvc);

  declareProperty("DoHist", m_doHist=true); //histograming yes/no true/false

  m_nPass = 0;
  m_nFail = 0;

  m_TotalTaus = 0;
  m_FastDecayedTau = 0;


  // Check counters
  m_invalidBeamParticlesCheckRate = 0;
  m_beamParticleswithStatusNotFourCheckRate = 0;
  m_beamEnergyCheckRate = 0;
  m_vtxNANandINFCheckRate = 0;
  m_vtxDisplacedstatuscode12CheckRate = 0;
  m_vtxDisplacedstatuscodenot12CheckRate = 0;
  m_partMomentumNANandINFCheckRate = 0;
  m_undecayedPi0statuscode12CheckRate = 0;
  m_unstableNoEndVtxCheckRate = 0;
  m_negativeEnergyTachyonicCheckRate = 0;
  m_decayCheckRate = 0;
  m_undisplacedLLHdaughtersCheckRate = 0;
  m_nonZeroPhotonMassCheckRate = 0;
  m_energyBalanceCheckRate = 0;
  m_momentumBalanceCheckRate = 0;
  m_negativeEnergyCheckRate = 0;
  m_tachyonCheckRate = 0;
  m_stableUnstableNoParentCheckRate = 0;
  m_unstablePartNoDecayVtxCheckRate = 0;
  m_undecayedPi0CheckRate = 0;
  m_Status1ShortLifetime  = 0;
  m_vtxDisplacedMoreThan_1m_CheckRateCnt = 0;
  m_undisplacedDecayDaughtersOfDisplacedVtxCheckRate = 0;
  m_nonG4_energyCheckRate = 0;
  m_unknownPDGIDCheckRate = 0;

  m_h_energy_dispVtxCheck =      0;
  m_h_energy_dispVtxCheck_lt10 = 0;
  m_h_pdgid_dispVtxCheck =       0;
  m_h_status_dispVtxCheck =      0;
  m_h_px_dispVtxCheck =          0;
  m_h_py_dispVtxCheck =          0;
  m_h_pz_dispVtxCheck =          0;
  m_h_vx_dispVtxCheck =          0;
  m_h_vy_dispVtxCheck =          0;
  m_h_vz_dispVtxCheck =          0;
  m_h_vxprod_dispVtxCheck =      0;
  m_h_vyprod_dispVtxCheck =      0;
  m_h_vzprod_dispVtxCheck =      0;
  m_h_vtxprod_dispVtxCheck =     0;
  m_h_vtxend_dispVtxCheck =      0;
  m_h_photon_mass =              0;
  m_h_photon_energy =            0;
  m_h_photon_e2_p2_e2 =          0;
  m_h_energyImbalance =          0;
  m_h_momentumImbalance_px =     0;
  m_h_momentumImbalance_py =     0;
  m_h_momentumImbalance_pz =     0;
  m_h_beamparticle1_Energy =     0;
  m_h_beamparticle2_Energy =     0;
  m_h_cmEnergyDiff =             0;
}


StatusCode TestHepMC::initialize() {
  CHECK(GenBase::initialize());

  if (m_doHist){
    CHECK(m_thistSvc.retrieve());

    m_h_energy_dispVtxCheck = new TH1F("h_energy_dispVtxCheck", "h_energy_dispVtxCheck", 2000, 0., 2000.);
    m_h_energy_dispVtxCheck_lt10 = new TH1F("h_energy_dispVtxCheck_lt10", "h_energy_dispVtxCheck_lt10", 1000, 0., 10.);
    m_h_pdgid_dispVtxCheck = new TH1F("h_pdgid_dispVtxCheck", "h_pdgid_dispVtxCheck", 10000, 0., 10000.);
    m_h_status_dispVtxCheck = new TH1F("h_status_dispVtxCheck", "h_status_dispVtxCheck", 10000, 0., 10000.);
    m_h_px_dispVtxCheck = new TH1F("h_px_dispVtxCheck", "h_px_dispVtxCheck", 4000, -2000., 2000.);
    m_h_py_dispVtxCheck = new TH1F("h_py_dispVtxCheck", "h_py_dispVtxCheck", 4000, -2000., 2000.);
    m_h_pz_dispVtxCheck = new TH1F("h_pz_dispVtxCheck", "h_pz_dispVtxCheck", 4000, -2000., 2000.);
    m_h_vx_dispVtxCheck = new TH1F("h_vx_dispVtxCheck", "h_vx_dispVtxCheck", 40000, -200., 200);
    m_h_vy_dispVtxCheck = new TH1F("h_vy_dispVtxCheck", "h_vy_dispVtxCheck", 40000, -200., 200);
    m_h_vz_dispVtxCheck = new TH1F("h_vz_dispVtxCheck", "h_vz_dispVtxCheck", 40000, -200., 200);
    m_h_vxprod_dispVtxCheck = new TH1F("h_vxprod_dispVtxCheck", "h_vxprod_dispVtxCheck", 40000, -200., 200.);
    m_h_vyprod_dispVtxCheck = new TH1F("h_vyprod_dispVtxCheck", "h_vyprod_dispVtxCheck", 40000, -200., 200.);
    m_h_vzprod_dispVtxCheck = new TH1F("h_vzprod_dispVtxCheck", "h_vzprod_dispVtxCheck", 40000, -200., 200.);
    m_h_vtxprod_dispVtxCheck = new TH1F("h_vtxprod_dispVtxCheck", "h_vtxprod_dispVtxCheck", 20000, 0., 200.);
    m_h_vtxend_dispVtxCheck = new TH1F("h_vtxend_dispVtxCheck", "h_vtxend_dispVtxCheck", 20000, 0., 200.);
    m_h_photon_mass = new TH1F("h_photon_mass", "h_photon_mass", 20000, -10000., 10000);
    m_h_photon_energy = new TH1F("h_photon_energy", "h_photon_energy", 20000, -10000., 10000);
    m_h_photon_e2_p2_e2 = new TH1F("h_photon_e2_p2_e2", "h_photon_e2_p2_e2", 20000, -10., 10);
    m_h_energyImbalance = new TH1F("h_energyImbalance", "h_energyImbalance", 2000, 0., 2000.);
    m_h_momentumImbalance_px = new TH1F("h_momentumImbalance_px", "h_momentumImbalance_px", 2000,0., 2000.);
    m_h_momentumImbalance_py = new TH1F("h_momentumImbalance_py", "h_momentumImbalance_py", 2000,0., 2000.);
    m_h_momentumImbalance_pz = new TH1F("h_momentumImbalance_pz", "h_momentumImbalance_pz", 2000,0., 2000.);
    m_h_beamparticle1_Energy = new TH1F("h_beamparticle1_Energy", "h_beamparticle1_Energy", 14000,0., 14000.);
    m_h_beamparticle2_Energy = new TH1F("h_beamparticle2_Energy", "h_beamparticle2_Energy", 14000,0., 14000.);
    m_h_cmEnergyDiff = new TH1F("h_cmEnergyDiff", "h_cmEnergyDiff", 8000, -4000., 4000.);

    CHECK(m_thistSvc->regHist("/TestHepMCname/h_energy_dispVtxCheck", m_h_energy_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_energy_dispVtxCheck_lt10", m_h_energy_dispVtxCheck_lt10));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_pdgid_dispVtxCheck", m_h_pdgid_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_status_dispVtxCheck", m_h_status_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_px_dispVtxCheck", m_h_px_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_py_dispVtxCheck", m_h_py_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_pz_dispVtxCheck", m_h_pz_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vx_dispVtxCheck", m_h_vx_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vy_dispVtxCheck", m_h_vy_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vz_dispVtxCheck", m_h_vz_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vxprod_dispVtxCheck", m_h_vxprod_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vyprod_dispVtxCheck", m_h_vyprod_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vzprod_dispVtxCheck", m_h_vzprod_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vtxprod_dispVtxCheck", m_h_vtxprod_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_vtxend_dispVtxCheck", m_h_vtxend_dispVtxCheck));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_photon_mass", m_h_photon_mass));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_photon_energy", m_h_photon_energy));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_photon_e2_p2_e2", m_h_photon_e2_p2_e2));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_energyImbalance", m_h_energyImbalance));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_momentumImbalance_px", m_h_momentumImbalance_px));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_momentumImbalance_py", m_h_momentumImbalance_py));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_momentumImbalance_pz", m_h_momentumImbalance_pz));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_beamparticle1_Energy", m_h_beamparticle1_Energy));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_beamparticle2_Energy", m_h_beamparticle2_Energy));
    CHECK(m_thistSvc->regHist("/TestHepMCname/h_cmEnergyDiff", m_h_cmEnergyDiff));

    ATH_MSG_INFO("No decay vertex is ignored for particles with status (list):" );
    for ( unsigned int i = 0; i < m_vertexStatuses.size(); i++ ) ATH_MSG_INFO("            : " << m_vertexStatuses.at(i) );
    ATH_MSG_INFO("Vertex statuses finished");

  } // End of histogramming setup

  // open the files and read G4particle_whitelist.txt
  std::ifstream G4file;
  G4file.open("G4particle_whitelist.txt");
  std::string line;
  int G4pdgID;
  if (!G4file.fail()){
    while(std::getline(G4file,line)){
      std::stringstream ss(line);
      ss >> G4pdgID;
      m_G4pdgID_tab.push_back(G4pdgID);
    }
    G4file.close();
  }
  else {
    ATH_MSG_WARNING("Failed to open G4particle_whitelist.txt, checking that all particles are known by Genat4 cannot be performed");
  }

  // Open the param file (G4 white list)
  G4file.open(m_paramFile.c_str());
  if (!G4file.fail()){
    ATH_MSG_INFO("extra white list for G4 found " << m_paramFile.c_str());
    while(std::getline(G4file,line)){
      std::stringstream ss(line);
      ss >> G4pdgID;
      m_G4pdgID_tab.push_back(G4pdgID);
    }
    G4file.close();
  }
  else {
    ATH_MSG_INFO("extra white list for G4 not provided ");
  }

  // Open the files and read susyParticlePdgid.txt
  std::ifstream susyFile;
  susyFile.open("susyParticlePdgid.txt");
  int susyPdgID;
  if (!susyFile.fail()){
    while(getline(susyFile,line)){
      stringstream ss1(line);
      ss1 >> susyPdgID;
      m_SusyPdgID_tab.push_back(susyPdgID);
    }
    susyFile.close();
  }
  else{
    ATH_MSG_WARNING("Failed to open susyParticlePdgid.txt, listing particles not present in PDTTable");
  }

  // Open the file of extra PDG IDs that don't need to obey the rules
  std::ifstream pdgFile;
  pdgFile.open(m_unknownPDGIDFile.c_str());
  int pdgID;
  if (!pdgFile.fail()){
    ATH_MSG_INFO("extra white list for PDG IDs found " << m_unknownPDGIDFile.c_str());
    while(std::getline(pdgFile,line)){
      std::stringstream ss(line);
      ss >> pdgID;
      m_uknownPDGID_tab.push_back(pdgID);
    }
    pdgFile.close();
  }
  else {
    ATH_MSG_INFO("extra white list for PDG IDs not provided");
  }

  return StatusCode::SUCCESS;
}


StatusCode TestHepMC::execute() {

  // Holder for filter outcome; allows us to check all filters on each event and diagnose multiple problems at once
  bool filter_pass = true;

  // Loop over all events in McEventCollection
  /// @todo Use C++ for(:)
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    const HepMC::GenEvent* evt = *itr;

    double totalPx = 0;
    double totalPy = 0;
    double totalPz = 0;
    double totalE  = 0;
    double nonG4_energy = 0;
    vector<int> negEnPart;
    vector<int> tachyons;
    vector<int> unstNoEnd;
    vector<int> unDecPi0;
    vector<int> undisplaceds;

    // Check beams and work out per-event beam energy
#ifdef HEPMC3
    auto beams_t = evt->beams();
    std::pair<std::shared_ptr<const HepMC3::GenParticle>,std::shared_ptr<const HepMC3::GenParticle>> beams;
    beams.first=beams_t.at(0);
    beams.second=beams_t.at(1);
#else
    auto beams = evt->beam_particles();
#endif
    double cmenergy = m_cm_energy;
    if (!HepMC::valid_beam_particles(evt)) {
      ATH_MSG_WARNING("Invalid beam particle pointers -- this generator interface should be fixed");
      if (cmenergy < 0) ATH_MSG_WARNING("Invalid expected beam energy: " << cmenergy << " MeV");
      ++m_invalidBeamParticlesCheckRate;
    } else {
      if (beams.first->status() != 4 || beams.second->status() != 4) {
        ATH_MSG_WARNING("Beam particles have incorrectly set status -- this generator interface should be fixed");
        ++m_beamParticleswithStatusNotFourCheckRate;
      }
      const double sumE = beams.first->momentum().e() + beams.second->momentum().e();
      const double sumP = beams.first->momentum().pz() + beams.second->momentum().pz();
      cmenergy = std::sqrt(sumE*sumE - sumP*sumP);

      if (m_cm_energy > 0 && std::abs(cmenergy - m_cm_energy) > m_cme_diff) {
        ATH_MSG_FATAL("Beam particles have incorrect energy: " << m_cm_energy/1000. << " GeV expected, vs. " << cmenergy/1000. << " GeV found");
        setFilterPassed(false);
        if (m_doHist){
          m_h_beamparticle1_Energy->Fill(beams.first->momentum().e()*1.E-03);
          m_h_beamparticle2_Energy->Fill(beams.second->momentum().e()*1.E-03);
          m_h_cmEnergyDiff->Fill((cmenergy-m_cm_energy)*1.E-03);
        }
        ++m_beamEnergyCheckRate;
        // Special case: this is so bad that we immediately fail out
        return StatusCode::FAILURE;
      }
    }

    // Check vertices
    int vtxDisplacedstatuscode12CheckRateCnt=0;
    int vtxDisplacedstatuscodenot12CheckRateCnt=0;
#ifdef HEPMC3
    for (auto vtx: evt->vertices()) {
#else
    for (auto vitr = evt->vertices_begin(); vitr != evt->vertices_end(); ++vitr ) {
      const HepMC::GenVertex* vtx = *vitr;
#endif
      const HepMC::FourVector pos = vtx->position();

      // Check for NaNs and infs in vertex position components
      if ( std::isnan(pos.x()) || std::isinf(pos.x()) ||
           std::isnan(pos.y()) || std::isinf(pos.y()) ||
           std::isnan(pos.z()) || std::isinf(pos.z()) ) {
        ATH_MSG_WARNING("NaN (Not A Number) or inf found in the event record vertex positions");
        
        ++m_vtxNANandINFCheckRate;
        if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
        if (m_vtxNaNTest) {
          filter_pass = false;
        }
      } // Done of checking for nans and infinities

      // Check for too-far-displaced vertices
      // Anything which propagates macroscopically should be set stable in evgen for G4 to handle
      const double dist_trans2 = pos.x()*pos.x() + pos.y()*pos.y(); // in mm2
      const double dist2 = dist_trans2 + pos.z()*pos.z(); // in mm2
      const double dist_trans = std::sqrt(dist_trans2); // in mm
      const double dist = std::sqrt(dist2); // in mm
      if (dist2 > m_max_dist*m_max_dist) {
        ATH_MSG_WARNING("Found vertex position displaced by more than " << m_max_dist << "mm: " << dist << "mm");
        ++m_vtxDisplacedMoreThan_1m_CheckRateCnt;
        
        if (m_vtxDisplacedTest) {
          filter_pass = false;
        }
      }
      if (dist_trans2 > m_max_dist_trans*m_max_dist_trans) {
        ATH_MSG_WARNING("Found vertex position displaced by more than " << m_max_dist_trans << "mm in transverse distance: " << dist_trans << "mm");

#ifdef HEPMC3
        for (auto part: vtx->particles_in()) {
#else
        for (auto part_it = vtx->particles_in_const_begin(); part_it != vtx->particles_in_const_end(); ++part_it) {
        auto part=(*part_it);
#endif
          ATH_MSG_WARNING("Outgoing particle : ");
          if (m_dumpEvent) HepMC::Print::line(std::cout,part);
          ATH_MSG_WARNING("production vertex = " << part->production_vertex()->position().x() << ", " << part->production_vertex()->position().y() << ", " << part->production_vertex()->position().z());
          ATH_MSG_WARNING("end vertex        = " << part->end_vertex()->position().x() << ", " << part->end_vertex()->position().y() << ", " << part->end_vertex()->position().z());
          ATH_MSG_WARNING("parents info: ");
          if (part->production_vertex()) {
#ifdef HEPMC3
            for(auto p_parents: part->production_vertex()->particles_in()) {
#else
            for(auto p_parents_it = part->production_vertex()->particles_in_const_begin(); p_parents_it != part->production_vertex()->particles_in_const_end(); ++p_parents_it) {
            auto p_parents=(*p_parents_it);
#endif
              if (m_dumpEvent) HepMC::Print::line(std::cout,p_parents);
              ATH_MSG_WARNING("\t");
            }
          } // Done with fancy print

          if (part->status()==1 || part->status()==2){
            vtxDisplacedstatuscode12CheckRateCnt += 1;
          } else {
            vtxDisplacedstatuscodenot12CheckRateCnt += 1;
          }

          if (m_doHist){
            m_h_energy_dispVtxCheck->Fill(part->momentum().e()*1e-3);
            if (part->momentum().e()*1e-3 < 10.) {
              m_h_energy_dispVtxCheck_lt10->Fill(part->momentum().e()*1e-3);
            }
            m_h_pdgid_dispVtxCheck->Fill(part->pdg_id());
            m_h_status_dispVtxCheck->Fill(part->status());
            m_h_px_dispVtxCheck->Fill(part->momentum().px()*1e-3);
            m_h_py_dispVtxCheck->Fill(part->momentum().py()*1e-3);
            m_h_pz_dispVtxCheck->Fill(part->momentum().pz()*1e-3);
            m_h_vx_dispVtxCheck->Fill(part->end_vertex()->position().x());
            m_h_vy_dispVtxCheck->Fill(part->end_vertex()->position().y());
            m_h_vz_dispVtxCheck->Fill(part->end_vertex()->position().z());
            m_h_vxprod_dispVtxCheck->Fill(part->production_vertex()->position().x());
            m_h_vyprod_dispVtxCheck->Fill(part->production_vertex()->position().y());
            m_h_vzprod_dispVtxCheck->Fill(part->production_vertex()->position().z());
            double endvx = part->end_vertex()->position().x();
            double endvy = part->end_vertex()->position().y();
            double endvz = part->end_vertex()->position().z();
            double prodvx = part->production_vertex()->position().x();
            double prodvy = part->production_vertex()->position().y();
            double prodvz = part->production_vertex()->position().z();
            double enddis = std::sqrt(endvx*endvx + endvy*endvy + endvz*endvz);
            double proddis = std::sqrt(prodvx*prodvx + prodvy*prodvy + prodvz*prodvz);
            m_h_vtxend_dispVtxCheck->Fill(enddis);
            m_h_vtxprod_dispVtxCheck->Fill(proddis);
          } // End of the filling of histograms for bad vertices
        } // End of a loop over theparents of the bad vertex
      } // Found a bad vertex
    } // Loop over all vertices
    if (vtxDisplacedstatuscode12CheckRateCnt>0) ++m_vtxDisplacedstatuscode12CheckRate;
    if (vtxDisplacedstatuscodenot12CheckRateCnt>0) ++m_vtxDisplacedstatuscodenot12CheckRate;

    // Check particles
    for (auto pitr: *evt) {

      // Local loop variables to clean up the check code
      const HepMC::FourVector pmom = pitr->momentum();
      const int pstatus = pitr->status();
      const int ppdgid = pitr->pdg_id();
      const int pbarcode = HepMC::barcode(pitr);

      // Check for NaNs and infs in momentum components
      if ( std::isnan(pmom.px()) || std::isinf(pmom.px()) ||
           std::isnan(pmom.py()) || std::isinf(pmom.py()) ||
           std::isnan(pmom.pz()) || std::isinf(pmom.pz()) ||
           std::isnan(pmom.e())  || std::isinf(pmom.e()) ) {
        ATH_MSG_WARNING("NaN (Not A Number) or inf found in the event record momenta");
        ++m_partMomentumNANandINFCheckRate;
        
        if (m_dumpEvent) HepMC::Print::line(std::cout,pitr);
        if (m_momNaNTest) {
          filter_pass = false;
        }
      } // End of check for NaNs and infiinities

      // Check for undecayed pi0s
      if (pstatus == 1 || pstatus == 2) {
        if (ppdgid == 111 && !pitr->end_vertex() ) {
          unDecPi0.push_back( pbarcode);
          ++m_undecayedPi0CheckRate;
        }
      } // End of check for undecayed pi0s

      //check stable particle lifetimes
      if (pstatus == 1) {
        const HepPDT::ParticleData* pd = particleData(ppdgid);
        if (pd != NULL) {
          double plifetime = pd->lifetime()*1e+12;  // why lifetime doesn't come in common units???
          if (plifetime != 0 && plifetime < m_min_tau) { // particles with infinite lifetime get a 0 in the PDT
            ATH_MSG_WARNING("Stable particle found with lifetime = " << plifetime << "~ns!!");
            if (m_dumpEvent) HepMC::Print::line(std::cout,pitr);

            ++m_Status1ShortLifetime;
            
            if (m_lifeTimeTest) {
              filter_pass = false;
            }
          } // Particle did not have infinite lifetime
        } // The particle has a data table (so a lifetime)
        else{
          int susyPart = 0;
          vector<int>::size_type count = 0;
          while (susyPart==0 && (count < m_SusyPdgID_tab.size() )){
            // no warning for SUSY particles from the list susyParticlePdgid.txt
            if (m_SusyPdgID_tab[count] == std::abs(ppdgid)) {
              susyPart=1;
            }
            count++;
          } // Look through the SUSY table to see if this one should be counted
          if (susyPart==0){
            ATH_MSG_WARNING("Stable particle not found in PDT, no lifetime check done");
            if (m_dumpEvent) HepMC::Print::line(std::cout,pitr);
          } // It's a SUSY particle -- skip the lifetime check
        } // The particle has no data table
      } // Test if the particle is stable

      //Check that stable particles are known by G4 or they are non-interacting
      HepPDT::ParticleID pid(ppdgid);
      int first_dig = ppdgid;
      while (first_dig > 9) first_dig /= 10;

      if ((pstatus == 1 ) && (!pitr->end_vertex()) && (!m_nonint.operator()(pitr)) && (!pid.isNucleus()) && (first_dig != 9) ) {

        int known_byG4 = 0;
        vector<int>::size_type count =0;

        while (known_byG4==0 && count < m_G4pdgID_tab.size()){
          if(ppdgid == m_G4pdgID_tab[count]) known_byG4=1;
          count++;
        }
        if(known_byG4==0){
          nonG4_energy += pmom.e();
          ATH_MSG_WARNING("Interacting particle not known by Geant4 with ID " << ppdgid);
        }
      } // End of check that stable particles are known to G4 or are non-interacting

      // Check for bad PDG IDs
      if (!MC::PID::isValid(ppdgid)){
        ATH_MSG_WARNING("Invalid PDG ID found: " << ppdgid);
        if (m_unknownPDGIDTest && std::find(m_uknownPDGID_tab.begin(),m_uknownPDGID_tab.end(),ppdgid)!=m_uknownPDGID_tab.end()){
          filter_pass = false;
        }
      } // End of check for invalid PDG IDs

      // Check for unstables with no end vertex, 
      if (!pitr->end_vertex() && pstatus == 2) {
        unstNoEnd.push_back(pbarcode);
        ++m_unstableNoEndVtxCheckRate;
      } // End of check for unstable with no end vertex

      // Sum final state mom/energy, and note negative energy / tachyonic particles
      //     std::cout << "status " << pstatus << " e " << pmom.e() << " pz " << pmom.pz()<< std::endl;
      if ( pstatus == 1 && !pitr->end_vertex() ) {
        totalPx += pmom.px();
        totalPy += pmom.py();
        totalPz += pmom.pz();
        totalE  += pmom.e();
        if (pmom.e() < 0) {
          negEnPart.push_back(pbarcode);
          ++m_negativeEnergyTachyonicCheckRate;
        }
        const double aener = std::abs(pmom.e());
        if ( aener+m_accur_margin < std::abs(pmom.px()) || aener+m_accur_margin < std::abs(pmom.py()) || aener+m_accur_margin < std::abs(pmom.pz()) ) {
          tachyons.push_back(pbarcode);
          ++m_negativeEnergyTachyonicCheckRate;
        }
      } // End of sums for momentum and energy conservation

      // Decay checks (uses PdgToSearch attr value, for tau by default)
      /// @todo Clean up / improve / apply to *all* decaying species
      int tau_child = 0;
      if (std::abs(ppdgid) == m_pdg && (pstatus == 1 || pstatus == 2)) {
        ++m_TotalTaus;
        auto vtx = pitr->end_vertex();
        if (vtx) {
          double p_energy = 0;
#ifdef HEPMC3
          for (auto  desc: HepMC::descendant_particles(vtx)) {
#else
          for (auto  desc_it = vtx->particles_begin(HepMC::descendants); desc_it != vtx->particles_end(HepMC::descendants); ++desc_it) {
          auto desc=(*desc_it);
#endif
            if (std::abs(desc->pdg_id()) == m_pdg) tau_child = 1;
            if (desc->status() == 1) p_energy += desc->momentum().e();
          }
          if (std::abs( p_energy - pmom.e()) > m_energy_diff && !tau_child) {
            ATH_MSG_WARNING("Energy sum (decay products): "
                            << "Energy (original particle) > " << m_energy_diff << " MeV, "
                            << "Event #" << evt->event_number() << ", "
                            << "Barcode of the original particle = " << pbarcode);
            ++m_decayCheckRate;
            if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
          }
          //most taus should not decay immediately
          const HepMC::FourVector tau_decaypos = vtx->position();
          const double tau_displacement = tau_decaypos.x()*tau_decaypos.x() + tau_decaypos.y()*tau_decaypos.y() + tau_decaypos.z()*tau_decaypos.z();
          //tau_child != 1 exclude cases in which a tau is copied to another vertex or emits a photon
          if ((tau_displacement < 1.e-6) && (tau_child!=1)) ++m_FastDecayedTau;
        } else {
          ATH_MSG_WARNING("UNDECAYED PARTICLE WITH PDG_ID = " << m_pdg);
          ++m_decayCheckRate;
          if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
        }
      } // End of checks for specific particle (tau by default)

      // Check for undisplaced decay daughters from long-lived hadrons
      if (pitr->end_vertex()) {
        auto decayvtx = pitr->end_vertex();
        const HepMC::FourVector decaypos = decayvtx->position();
        const double displacement = decaypos.x()*decaypos.x() + decaypos.y()*decaypos.y() + decaypos.z()*decaypos.z();
        if (displacement > 1e-6) {
          for (auto ip: *decayvtx) {
            const HepMC::FourVector pos2 = ip->production_vertex()->position();
            const double displacement2 = pos2.x()*pos2.x() + pos2.y()*pos2.y() + pos2.z()*pos2.z();
            if (displacement2 < 1e-6) {
              const int pbarcode2 = HepMC::barcode(ip);
              const int vbarcode2 = HepMC::barcode(ip->production_vertex());
              ATH_MSG_WARNING("Decay child " << pbarcode2 << " from " << pbarcode
                              << " has undisplaced vertex (" << vbarcode2
                              << " @ " << displacement2 << "mm) "
                              << " but parent vertex is displaced (" << HepMC::barcode(decayvtx)
                              << " @ " << displacement << "mm)");
              undisplaceds.push_back(pbarcode2);
              ++m_undisplacedLLHdaughtersCheckRate;
            } // Check for displacement below 1 um
          } // Loop over all particles coming from the decay vertex
        } // Displacement of greater than 1 um
      } // End of check for undisplaced decay daughters from long-lived hadrons

      // Check for photons with non-zero masses
      /// @todo Persuade generator authors to set proper generated masses in HepMC, then *really* require mass = 0
      if (ppdgid == 22 && pstatus == 1) {
        const double mass = pitr->generated_mass();
        if (std::abs(mass) > 1.0) { // in MeV
          ATH_MSG_WARNING("Photon with non-zero mass found! Mass: " << mass << " MeV, BARCODE=" << pbarcode);
          ++m_nonZeroPhotonMassCheckRate;
        }
      } // End check for photons with too-large a mass

    } // End of loop over particles in the event

    // Energy of interacting particles not known by Geant4
    if(nonG4_energy > m_nonG4_energy_threshold) {
      ATH_MSG_WARNING("The energy of interacting particles not known by Geant4 is = " << nonG4_energy << " MeV");
      if (m_energyG4Test) {
        filter_pass = false;
      }
      ++m_nonG4_energyCheckRate;
    } // End of check for interacting particles not known by G4

    // Energy balance
    double lostE = std::abs(totalE - cmenergy);
    if (lostE > m_energy_diff) {
      ATH_MSG_WARNING("ENERGY BALANCE FAILED : E-difference = " << lostE << " MeV");

      ATH_MSG_WARNING("balance " << totalPx << " " << totalPy << " " << totalPz << " " << totalE);
      
      if (m_doHist){
        m_h_energyImbalance->Fill(lostE*1.E-03);
      }
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_energyImbalanceTest) {
        filter_pass = false;
      }
      ++m_energyBalanceCheckRate;
    } // End of energy balance check

    // Momentum balance
    if ( std::abs(totalPx) > m_energy_diff || std::abs(totalPy) > m_energy_diff || std::abs(totalPz) > m_energy_diff ) {
      ATH_MSG_WARNING("MOMENTUM BALANCE FAILED : SumPx = " << totalPx << " SumPy = " <<  totalPy << " SumPz = " <<  totalPz << " MeV");
      if (m_doHist){
        m_h_momentumImbalance_px->Fill(std::abs(totalPx)*1.E-03);
        m_h_momentumImbalance_py->Fill(std::abs(totalPy)*1.E-03);
        m_h_momentumImbalance_pz->Fill(std::abs(totalPz)*1.E-03);
      }
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_momImbalanceTest) {
        filter_pass = false;
      }
      ++m_momentumBalanceCheckRate;
    } // End of momentum balance check

    // Negative energy particles
    if (!negEnPart.empty()) {
      stringstream ss;
      ss << "NEGATIVE ENERGY PARTICLES FOUND : BARCODES =";
      for (vector<int>::const_iterator b = negEnPart.begin(); b != negEnPart.end(); ++b){
        ss << " " << *b;
      }
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_negativeEnergyTest) {
        filter_pass = false;
      }
      ++m_negativeEnergyCheckRate;
    } // End of negative energy particle chedk

    // Tachyons
    if (!tachyons.empty()) {
      stringstream ss;
      ss << "PARTICLES WITH |E| < |Pi| (i=x,y,z) FOUND : BARCODES =";
      for (vector<int>::const_iterator b = tachyons.begin(); b != tachyons.end(); ++b){
        ss << " " << *b;
      }
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_tachyonsTest) {
        filter_pass = false;
      }
      ++m_energyBalanceCheckRate;
    } // End of tachyon check

    // Unstable particles with no decay vertex
    if (!unstNoEnd.empty()) {
      stringstream ss;
      ss << "Unstable particle with no decay vertex found: BARCODES =";
      for (vector<int>::const_iterator b = unstNoEnd.begin(); b != unstNoEnd.end(); ++b){
        ss << " " << *b;
      }
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_unstableNoVtxTest) {
        filter_pass = false;
      }
      ++m_unstablePartNoDecayVtxCheckRate;
    } // End of unstable particle with no decay vertex check

    // Undecayed pi0
    if (!unDecPi0.empty()) {
      stringstream ss;
      ss << "pi0 with no decay vertex found: BARCODES =";
      for (vector<int>::const_iterator b = unDecPi0.begin(); b != unDecPi0.end(); ++b){
        ss << " " << *b;
      }
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_pi0NoVtxTest) {
        filter_pass = false;
      }
      ++m_undecayedPi0CheckRate;
    } // End of undecayed pi0 check

    // Undisplaced decay daughters of displaced vertices
    if (!undisplaceds.empty()) {
      stringstream ss;
      ss << "Undisplaced decay vertices from displaced particle: BARCODES =";
      for (vector<int>::const_iterator b = undisplaceds.begin(); b != undisplaceds.end(); ++b){
        ss << " " << *b;
      }
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) HepMC::Print::line(std::cout,**itr);
      if (m_undisplacedDaughtersTest) {
        filter_pass = false;
      }
      ++m_undisplacedDecayDaughtersOfDisplacedVtxCheckRate;
    } // End of undisplaced decay daughter of displaced vertices check

  } // End of loop  over MCEventCollection

  // End of execution for each event - update filter value
  if (!filter_pass){
    setFilterPassed(false);
    ++m_nFail;
  } else {
    ++m_nPass;
  }

  // If the efficiency after 100 events is below 10%, there is an important bug going on:
  // we fail the job immediately so it doesn't run for ever
  const double tmp_efficiency = double(m_nPass) / double(m_nPass + m_nFail);
  if ((m_nPass + m_nFail) > 100 && tmp_efficiency < 0.1) {
    ATH_MSG_FATAL("The efficiency after " << m_nPass + m_nFail << " events is " << tmp_efficiency*100. << "% !!!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TestHepMC::finalize() {

  ATH_MSG_INFO("Events passed = " << m_nPass << ", Events Failed = " << m_nFail);

  ATH_MSG_INFO(" Event rate with invalid Beam Particles = " << m_invalidBeamParticlesCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with beam particles and status not equal to 4 = " << m_beamParticleswithStatusNotFourCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with incorrect beam particle energies = " << m_beamEnergyCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with NaN (Not A Number) or inf found in the event record vertex positions = " << m_vtxNANandINFCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_vtxNaNTest) ATH_MSG_INFO(" The check for NaN or inf in vtx. record is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with vertices displaced more than " << m_max_dist_trans << "~mm in transverse direction for particles with status code other than 1 and 2 = " << m_vtxDisplacedstatuscodenot12CheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with vertices displaced more than " << m_max_dist << "~mm = " << m_vtxDisplacedMoreThan_1m_CheckRateCnt*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_vtxDisplacedTest) ATH_MSG_INFO(" The check for displaced vertices is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with NAN (Not A Number) or inf found in particle momentum values = " << m_partMomentumNANandINFCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_momNaNTest) ATH_MSG_INFO(" The check for NaN/inf in momentum record is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with undecayed pi0's with status 1 or 2 = " << m_undecayedPi0statuscode12CheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with unstable particles with no end vertex = " << m_unstableNoEndVtxCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with negative total energy like for tachyonic particles = " << m_negativeEnergyTachyonicCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with particles with improper decay properties = " << m_decayCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with undisplaced daughters of long lived hadrons = " << m_undisplacedLLHdaughtersCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with non zero photon mass = " << m_nonZeroPhotonMassCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with no energy balance = " << m_energyBalanceCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_energyImbalanceTest) ATH_MSG_INFO(" The check for energy imbalance is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with no momentum balance = " << m_momentumBalanceCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_momImbalanceTest) ATH_MSG_INFO(" The check for momentum imbalance is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with negative energy particles = " << m_negativeEnergyCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_negativeEnergyTest) ATH_MSG_INFO(" The check for particles with negative energy is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with tachyons = " << m_tachyonCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_tachyonsTest) ATH_MSG_INFO(" The check for tachyons is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with stable or unstable particles with no parents = " << m_stableUnstableNoParentCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with unstable particle with no decay vertex = " << m_unstablePartNoDecayVtxCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_unstableNoVtxTest) ATH_MSG_INFO(" The check for unstable part. without end vertex is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with undecayed Pi0's = " << m_undecayedPi0CheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_pi0NoVtxTest) ATH_MSG_INFO(" The check for undecayed pi0's is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with undisplaced decay daughters of displaced vertices = " << m_undisplacedDecayDaughtersOfDisplacedVtxCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_undisplacedDaughtersTest) ATH_MSG_INFO(" The check for  undisplaced daughters is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with particles with status 1 but lifetime < " << m_min_tau << "~ns = " << m_Status1ShortLifetime*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_lifeTimeTest) ATH_MSG_INFO(" The check for status 1 particles with too short lifetime is switched off, so is not included in the final TestHepMC efficiency ");
  ATH_MSG_INFO(" Event rate with energy sum of interacting particles non known by Geant4 above " << m_nonG4_energy_threshold << " MeV = " << m_nonG4_energyCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  if (!m_energyG4Test) ATH_MSG_INFO(" The check for energy not known by G4 is switched off, so is not included in the final TestHepMC efficiency "); 
  ATH_MSG_INFO(" Event rate with unknown PDG IDs " << m_unknownPDGIDCheckRate*100.0/double(m_nPass+m_nFail) << "%");
  if (!m_unknownPDGIDTest) ATH_MSG_INFO(" The check for unknown PDG IDs is sitched off, so it is not included in the final TestHepMC efficiency ");

  const double tau_fastDrate = double(m_FastDecayedTau) / double(m_TotalTaus);
  if(tau_fastDrate > m_tau_eff_threshold){
    ATH_MSG_FATAL("MORE THAN " << 100.*m_tau_eff_threshold << "% OF TAUS DECAYING IMMEDIATELY! " << m_FastDecayedTau << " found, out of: " << m_TotalTaus);
    return StatusCode::FAILURE;
  }


  const double efficiency = double(m_nPass) / double(m_nPass + m_nFail);
  ATH_MSG_INFO("Efficiency = " << efficiency * 100 << "%");

  // Check efficiency, and fail (to kill production jobs) if the pass rate is too low
  if (efficiency < m_eff_fail_threshold) {
    ATH_MSG_FATAL("EFFICIENCY IS TOO LOW! " << 100*efficiency << "% found, but at least: " << 100*m_eff_fail_threshold << "% required");
    return StatusCode::FAILURE;
  } else if (efficiency <= m_eff_warn_threshold) {
    ATH_MSG_WARNING("LOW EFFICIENCY! " << 100*efficiency << "% found, but at least: " << 100*m_eff_warn_threshold << "% expected");
  }

  return StatusCode::SUCCESS;
}


#endif
