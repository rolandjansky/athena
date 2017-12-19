/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/TestHepMC.h"
#include "GaudiKernel/DataSvc.h"

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

  declareProperty( "G4ExtraWhiteFile", m_paramFile       = "g4_extrawhite.param" );

  declareProperty("NoDecayVertexStatuses", m_vertexStatuses );
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
  ATH_MSG_INFO("Vertex statuses finsihed");

  }

  //open the files and read G4particle_whitelist.txt
  m_G4file.open("G4particle_whitelist.txt");
         std::string line;
         int G4pdgID;
        
         if (!m_G4file.fail()){
       
            while(std::getline(m_G4file,line)){
                 std::stringstream ss(line);
                 ss >> G4pdgID;

                 m_G4pdgID_tab.push_back(G4pdgID);
                 
            }
            m_G4file.close();
         }
         else{
          ATH_MSG_WARNING("Failed to open G4particle_whitelist.txt, checking that all particles are known by Genat4 cannot be performed");
         }

 //open the files and read G4particle_whitelist.txt
	 m_G4file.open(m_paramFile.c_str());
	 //         std::string line;
	 //         int G4pdgID;
        
         if (!m_G4file.fail()){
	   ATH_MSG_INFO("extra white list for G4 found " << m_paramFile.c_str());
            while(std::getline(m_G4file,line)){
                 std::stringstream ss(line);
                 ss >> G4pdgID;

                 m_G4pdgID_tab.push_back(G4pdgID);
                 
            }
            m_G4file.close();
         }
         else{
          ATH_MSG_INFO("extra white list for G4 not provided ");
         }

 //open the files and read susyParticlePdgid.txt
  m_susyFile.open("susyParticlePdgid.txt");
         string line1;
         int susyPdgID;
        
         if (!m_susyFile.fail()){
       
            while(getline(m_susyFile,line1)){
                 stringstream ss1(line1);
                 ss1 >> susyPdgID;

                 m_SusyPdgID_tab.push_back(susyPdgID);
                 
            }
            m_susyFile.close();
         }
         else{
          ATH_MSG_WARNING("Failed to open susyParticlePdgid.txt, listing particles not present in PDTTable");
         }

  return StatusCode::SUCCESS;
}


StatusCode TestHepMC::execute() {
  // If the efficiency after 100 events is below 10%, there is an important bug going on:
  // we fail the job immediately so it doesn't run for ever
  const double tmp_efficiency = double(m_nPass) / double(m_nPass + m_nFail);
  if ((m_nPass + m_nFail) > 100 && tmp_efficiency < 0.1) {
    ATH_MSG_FATAL("The efficiency after " << m_nPass + m_nFail << " events is " << tmp_efficiency*100. << "% !!!");
    return StatusCode::FAILURE;
  }

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
    const pair<HepMC::GenParticle*, HepMC::GenParticle*> beams = evt->beam_particles();
    double cmenergy = m_cm_energy;
    if (!evt->valid_beam_particles()) {
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
      cmenergy = sqrt(sumE*sumE - sumP*sumP);

      if (m_cm_energy > 0 && fabs(cmenergy - m_cm_energy) > m_cme_diff) {
        ATH_MSG_FATAL("Beam particles have incorrect energy: " << m_cm_energy/1000. << " GeV expected, vs. " << cmenergy/1000. << " GeV found");
        setFilterPassed(false);
       if (m_doHist){
        m_h_beamparticle1_Energy->Fill(beams.first->momentum().e()*1.E-03);
        m_h_beamparticle2_Energy->Fill(beams.second->momentum().e()*1.E-03);
        m_h_cmEnergyDiff->Fill((cmenergy-m_cm_energy)*1.E-03);
       }
        ++m_beamEnergyCheckRate;
        //return StatusCode::SUCCESS;
        return StatusCode::FAILURE;
      }
    }


    // Check vertices
    int vtxDisplacedstatuscode12CheckRateCnt=0;
    int vtxDisplacedstatuscodenot12CheckRateCnt=0;
    for (HepMC::GenEvent::vertex_const_iterator vitr = evt->vertices_begin(); vitr != evt->vertices_end(); ++vitr ) {
      const HepMC::GenVertex* vtx = *vitr;
      const HepMC::ThreeVector pos = vtx->point3d();

      // Check for NaNs and infs in vertex position components
      if ( std::isnan(pos.x()) || std::isinf(pos.x()) ||
           std::isnan(pos.y()) || std::isinf(pos.y()) ||
           std::isnan(pos.z()) || std::isinf(pos.z()) ) {
        ATH_MSG_WARNING("NaN (Not A Number) or inf found in the event record vertex positions");
        ++m_nFail;
        ++m_vtxNANandINFCheckRate;
        if (m_dumpEvent) (*itr)->print();
        setFilterPassed(false);
        return StatusCode::SUCCESS;
      }

      // Check for too-far-displaced vertices
      // Anything which propagates macroscopically should be set stable in evgen for G4 to handle
      const double dist_trans2 = pos.x()*pos.x() + pos.y()*pos.y(); // in mm2
      const double dist2 = dist_trans2 + pos.z()*pos.z(); // in mm2
      const double dist_trans = sqrt(dist_trans2); // in mm
      const double dist = sqrt(dist2); // in mm
      if (dist2 > m_max_dist*m_max_dist) {
        ATH_MSG_WARNING("Found vertex position displaced by more than " << m_max_dist << "mm: " << dist << "mm");
        ++m_vtxDisplacedMoreThan_1m_CheckRateCnt;
        ++m_nFail;
        setFilterPassed(false);
        return StatusCode::SUCCESS;
      }
      if (dist_trans2 > m_max_dist_trans*m_max_dist_trans) {
        ATH_MSG_WARNING("Found vertex position displaced by more than " << m_max_dist_trans << "mm in transverse distance: " << dist_trans << "mm");

        HepMC::GenVertex::particle_iterator par = (*vitr)->particles_begin(HepMC::parents);
        for (; par != (*vitr)->particles_end(HepMC::parents); ++par) {
	  //  std::cout << "Outgoing particle : " << std::endl;
	  //          (*par)->print();
          ATH_MSG_WARNING("Outgoing particle : ");
          if (m_dumpEvent) (*par)->print();
          ATH_MSG_WARNING("production vertex = " << (*par)->production_vertex()->point3d().x() << ", " << (*par)->production_vertex()->point3d().y() << ", " << (*par)->production_vertex()->point3d().z());
          ATH_MSG_WARNING("end vertex        = " << (*par)->end_vertex()->point3d().x() << ", " << (*par)->end_vertex()->point3d().y() << ", " << (*par)->end_vertex()->point3d().z());
          ATH_MSG_WARNING("parents info: ");
	  //	  std::cout << "production vertex = " << (*par)->production_vertex()->point3d().x() << ", " << (*par)->production_vertex()->point3d().y() << ", " << (*par)->production_vertex()->point3d().z() << endl;
	  //	  std::cout << "end vertex        = " << (*par)->end_vertex()->point3d().x() << ", " << (*par)->end_vertex()->point3d().y() << ", " << (*par)->end_vertex()->point3d().z() << endl;
	  //   std::cout <<  << std::endl;
          if ((*par)->production_vertex()) {
            HepMC::GenVertex::particle_iterator p_parents = (*par)->production_vertex()->particles_begin(HepMC::parents);
            for(; p_parents != (*par)->production_vertex()->particles_end(HepMC::parents); ++p_parents) {
	      // cout << "\t";
	      if (m_dumpEvent) (*p_parents)->print();
	      ATH_MSG_WARNING("\t");
          
            }
          }

          if ((*par)->status()==1 || (*par)->status()==2)
            vtxDisplacedstatuscode12CheckRateCnt += 1;
          else
            vtxDisplacedstatuscodenot12CheckRateCnt += 1;


       if (m_doHist){
          m_h_energy_dispVtxCheck->Fill((*par)->momentum().e()*1e-3);
          if ((*par)->momentum().e()*1e-3 < 10.) {
            m_h_energy_dispVtxCheck_lt10->Fill((*par)->momentum().e()*1e-3);}
          m_h_pdgid_dispVtxCheck->Fill((*par)->pdg_id());
          m_h_status_dispVtxCheck->Fill((*par)->status());
          m_h_px_dispVtxCheck->Fill((*par)->momentum().px()*1e-3);
          m_h_py_dispVtxCheck->Fill((*par)->momentum().py()*1e-3);
          m_h_pz_dispVtxCheck->Fill((*par)->momentum().pz()*1e-3);
          m_h_vx_dispVtxCheck->Fill((*par)->end_vertex()->point3d().x());
          m_h_vy_dispVtxCheck->Fill((*par)->end_vertex()->point3d().y());
          m_h_vz_dispVtxCheck->Fill((*par)->end_vertex()->point3d().z());
          m_h_vxprod_dispVtxCheck->Fill((*par)->production_vertex()->point3d().x());
          m_h_vyprod_dispVtxCheck->Fill((*par)->production_vertex()->point3d().y());
          m_h_vzprod_dispVtxCheck->Fill((*par)->production_vertex()->point3d().z());
          double endvx = (*par)->end_vertex()->point3d().x();
          double endvy = (*par)->end_vertex()->point3d().y();
          double endvz = (*par)->end_vertex()->point3d().z();
          double prodvx = (*par)->production_vertex()->point3d().x();
          double prodvy = (*par)->production_vertex()->point3d().y();
          double prodvz = (*par)->production_vertex()->point3d().z();
          double enddis = sqrt(endvx*endvx + endvy*endvy + endvz*endvz);
          double proddis = sqrt(prodvx*prodvx + prodvy*prodvy + prodvz*prodvz);
          m_h_vtxend_dispVtxCheck->Fill(enddis);
          m_h_vtxprod_dispVtxCheck->Fill(proddis);
       }
        }
      }
    }
    if (vtxDisplacedstatuscode12CheckRateCnt>0) ++m_vtxDisplacedstatuscode12CheckRate;
    if (vtxDisplacedstatuscodenot12CheckRateCnt>0) ++m_vtxDisplacedstatuscodenot12CheckRate;

    // Check particles
    for (HepMC::GenEvent::particle_const_iterator pitr = evt->particles_begin(); pitr != evt->particles_end(); ++pitr ) {

      // Local loop variables to clean up the check code
      const HepMC::FourVector pmom = (*pitr)->momentum();
      const int pstatus = (*pitr)->status();
      const int ppdgid = (*pitr)->pdg_id();
      const int pbarcode = (*pitr)->barcode();
      // Check for NaNs and infs in momentum components
      if ( std::isnan(pmom.px()) || std::isinf(pmom.px()) ||
           std::isnan(pmom.py()) || std::isinf(pmom.py()) ||
           std::isnan(pmom.pz()) || std::isinf(pmom.pz()) ||
           std::isnan(pmom.e())  || std::isinf(pmom.e()) ) {
        ATH_MSG_WARNING("NaN (Not A Number) or inf found in the event record momenta");
        ++m_partMomentumNANandINFCheckRate;
        ++m_nFail;
        if (m_dumpEvent) (*pitr)->print();
        setFilterPassed(false);
        return StatusCode::SUCCESS;
      }

      // Check for undecayed pi0s
      if (pstatus == 1 || pstatus == 2) {
        if (ppdgid == 111 && !(*pitr)->end_vertex() ) {
          unDecPi0.push_back( pbarcode);
          ++m_undecayedPi0CheckRate;
        }
      }

      //check stable particle lifetimes
      if (pstatus == 1) {
        const HepPDT::ParticleData* pd = particleData(ppdgid);
        if (pd != NULL) {
          double plifetime = pd->lifetime()*1e+12;  // why lifetime doesn't come in common units???
          if (plifetime != 0 && plifetime < m_min_tau) { // particles with infinite lifetime get a 0 in the PDT
            ATH_MSG_WARNING("Stable particle found with lifetime = " << plifetime << "~ns!!");
	    if (m_dumpEvent) (*pitr)->print();

            ++m_Status1ShortLifetime;
            ++m_nFail;
            setFilterPassed(false);
            return StatusCode::SUCCESS;
          }
        }
        else{
          int susyPart = 0;
          vector<int>::size_type count = 0;
          while (susyPart==0 && (count < m_SusyPdgID_tab.size() )){
	    // no warning for SUSY particles from the list susyParticlePdgid.txt
            if (m_SusyPdgID_tab[count] == abs(ppdgid)) {
	      //  cout << "susy particle " << ppdgid << endl;
              susyPart=1;
	    }
            count++;
	  }
	  if (susyPart==0){
            ATH_MSG_WARNING("Stable particle not found in PDT, no lifetime check done");
	    if (m_dumpEvent) (*pitr)->print();
    
	  }
        }
      }

      //Check that stable particles are known by G4 or they are non-interacting
      HepPDT::ParticleID pid(ppdgid);
      int first_dig = ppdgid;
      while(first_dig > 9) first_dig /= 10;
         
      if ((pstatus == 1 ) && (!(*pitr)->end_vertex()) && (!m_nonint.operator()(*pitr)) && (!pid.isNucleus()) && (first_dig != 9) ) {

           int known_byG4 = 0;
           vector<int>::size_type count =0;


	     while (known_byG4==0 && count < m_G4pdgID_tab.size()){
//	       std::cout<< "G4 pdgid " <<  m_G4pdgID_tab[count] << std::endl;
                 if(ppdgid == m_G4pdgID_tab[count]) known_byG4=1;
                 count++;
            }
            if(known_byG4==0){
              nonG4_energy += pmom.e();
              ATH_MSG_WARNING("Interacting particle not known by Geant4 with ID " << ppdgid);
            }
      }

      // Check for unstables with no end vertex, such as undecayed gluons, Ws, Zs, and h [not status 3 to avoid probles with photos]
      if (!(*pitr)->end_vertex() &&
          ( ( std::find( m_vertexStatuses.begin(), m_vertexStatuses.end(), pstatus ) == m_vertexStatuses.end() ) 
	    || 
	    ((abs(ppdgid) == 23 || ppdgid == 24 || ppdgid == 25) && pstatus != 3))) {
        unstNoEnd.push_back(pbarcode);
        ++m_unstableNoEndVtxCheckRate;
      }

      // Sum final state mom/energy, and note negative energy / tachyonic particles
      //     std::cout << "status " << pstatus << " e " << pmom.e() << " pz " << pmom.pz()<< std::endl;
      if ( pstatus == 1 && !(*pitr)->end_vertex() ) {
        totalPx += pmom.px();
        totalPy += pmom.py();
        totalPz += pmom.pz();
        totalE  += pmom.e();
        if (pmom.e() < 0) {negEnPart.push_back(pbarcode); ++m_negativeEnergyTachyonicCheckRate;}
        const double aener = fabs(pmom.e());
        if ( aener+m_accur_margin < fabs(pmom.px()) || aener+m_accur_margin < fabs(pmom.py()) || aener+m_accur_margin < fabs(pmom.pz()) ) {
          tachyons.push_back(pbarcode);
          ++m_negativeEnergyTachyonicCheckRate;
        }
      }

      // Decay checks (uses PdgToSearch attr value, for tau by default)
      /// @todo Clean up / improve / apply to *all* decaying species
      int tau_child = 0;
      if (abs(ppdgid) == m_pdg && (pstatus == 1 || pstatus == 2)) {
        ++m_TotalTaus;
        HepMC::GenVertex* vtx = (*pitr)->end_vertex();
        if (vtx) {
          double p_energy = 0;
          HepMC::GenVertex::particle_iterator desc = vtx->particles_begin(HepMC::descendants);
          for ( ; desc != vtx->particles_end(HepMC::descendants); ++desc) {
            if (abs((*desc)->pdg_id()) == m_pdg) tau_child = 1;
            if ((*desc)->status() == 1) p_energy += (*desc)->momentum().e();
          }
          if (fabs( p_energy - pmom.e()) > m_energy_diff && !tau_child) {
            ATH_MSG_WARNING("Energy sum (decay products): "
                            << "Energy (original particle) > " << m_energy_diff << " MeV, "
                            << "Event #" << evt->event_number() << ", "
                            << "Barcode of the original particle = " << pbarcode);
            ++m_decayCheckRate;
            if (m_dumpEvent) (*itr)->print();
          }
          //most taus should not decay immediately
          const HepMC::FourVector tau_decaypos = vtx->position();
          const double tau_displacement = tau_decaypos.x()*tau_decaypos.x() + tau_decaypos.y()*tau_decaypos.y() + tau_decaypos.z()*tau_decaypos.z();
          //tau_child!=1 exclude cases in which a tau is copied to another vertex or emits a photon
          if ((tau_displacement < 1.e-6) && (tau_child!=1)) ++m_FastDecayedTau;
        } else {
          ATH_MSG_WARNING("UNDECAYED PARTICLE WITH PDG_ID = " << m_pdg);
          ++m_decayCheckRate;
          if (m_dumpEvent) (*itr)->print();
        }
      }

      // Check for undisplaced decay daughters from long-lived hadrons
      if ((*pitr)->end_vertex()) {
        HepMC::GenVertex* decayvtx = (*pitr)->end_vertex();
        const HepMC::FourVector decaypos = decayvtx->position();
        const double displacement = decaypos.x()*decaypos.x() + decaypos.y()*decaypos.y() + decaypos.z()*decaypos.z();
        if (displacement > 1e-6) {
          HepMC::GenVertex::particle_iterator ipbegin = decayvtx->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator ipend = decayvtx->particles_end(HepMC::children);
          for (HepMC::GenVertex::particle_iterator ip = ipbegin; ip != ipend; ++ip) {
            const HepMC::FourVector pos2 = (*ip)->production_vertex()->position();
            const double displacement2 = pos2.x()*pos2.x() + pos2.y()*pos2.y() + pos2.z()*pos2.z();
            if (displacement2 < 1e-6) {
              const int pbarcode2 = (*ip)->barcode();
              const int vbarcode2 = (*ip)->production_vertex()->barcode();
              ATH_MSG_WARNING("Decay child " << pbarcode2 << " from " << pbarcode
                              << " has undisplaced vertex (" << vbarcode2
                              << " @ " << displacement2 << "mm) "
                              << " but parent vertex is displaced (" << decayvtx->barcode()
                              << " @ " << displacement << "mm)");
              undisplaceds.push_back(pbarcode2);
              ++m_undisplacedLLHdaughtersCheckRate;
            }
          }
        }
      }

      // Check for photons with non-zero masses
      /// @todo Persuade generator authors to set proper generated masses in HepMC, then *really* require mass = 0
      if (ppdgid == 22 && pstatus == 1) {
        const double mass = (*pitr)->generated_mass();
        if (fabs(mass) > 1.0) { // in MeV
          ATH_MSG_WARNING("Photon with non-zero mass found! Mass: " << mass << " MeV, BARCODE=" << pbarcode);
          ++m_nonZeroPhotonMassCheckRate;
        }
      }

    }

    // Energy of interacting particles not known by Geant4
    if(nonG4_energy > m_nonG4_energy_threshold) {
      ATH_MSG_WARNING("The energy of interacting particles not known by Geant4 is = " << nonG4_energy << " MeV");
      setFilterPassed(false);
      ++m_nonG4_energyCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;      
    }

    // Energy balance
    double lostE = fabs(totalE - cmenergy);
    if (lostE > m_energy_diff) {
      ATH_MSG_WARNING("ENERGY BALANCE FAILED : E-difference = " << lostE << " MeV");

	  //	  std::cout << "balance " << totalPx << " " << totalPy << " " << totalPz << " " << totalE << std::endl;
      ATH_MSG_WARNING("balance " << totalPx << " " << totalPy << " " << totalPz << " " << totalE);
      
     if (m_doHist){
      m_h_energyImbalance->Fill(lostE*1.E-03);
      //     std::cout << "hidt filled " << std::endl;
     }
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_energyBalanceCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    // Momentum balance
    if ( fabs(totalPx) > m_energy_diff || fabs(totalPy) > m_energy_diff || fabs(totalPz) > m_energy_diff ) {
      ATH_MSG_WARNING("MOMENTUM BALANCE FAILED : SumPx = " << totalPx << " SumPy = " <<  totalPy << " SumPz = " <<  totalPz << " MeV");
      //if (m_dumpEvent || fabs(totalPx) > m_max_energy_diff || fabs(totalPy) > m_max_energy_diff || fabs(totalPz) > m_max_energy_diff) (*itr)->print();
    if (m_doHist){
      m_h_momentumImbalance_px->Fill(fabs(totalPx)*1.E-03);
      m_h_momentumImbalance_py->Fill(fabs(totalPy)*1.E-03);
      m_h_momentumImbalance_pz->Fill(fabs(totalPz)*1.E-03);
    }
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_momentumBalanceCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    // Negative energy particles
    if (!negEnPart.empty()) {
      stringstream ss;
      ss << "NEGATIVE ENERGY PARTICLES FOUND : BARCODES =";
      for (vector<int>::const_iterator b = negEnPart.begin(); b != negEnPart.end(); ++b)
        ss << " " << *b;
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_negativeEnergyCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    // Tachyons
    if (!tachyons.empty()) {
      stringstream ss;
      ss << "PARTICLES WITH |E| < |Pi| (i=x,y,z) FOUND : BARCODES =";
      for (vector<int>::const_iterator b = tachyons.begin(); b != tachyons.end(); ++b)
        ss << " " << *b;
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_energyBalanceCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    // Unstable particles with no decay vertex
    if (!unstNoEnd.empty()) {
      stringstream ss;
      ss << "Unstable particle with no decay vertex found: BARCODES =";
      for (vector<int>::const_iterator b = unstNoEnd.begin(); b != unstNoEnd.end(); ++b)
        ss << " " << *b;
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_unstablePartNoDecayVtxCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    // Undecayed pi0
    if (!unDecPi0.empty()) {
      stringstream ss;
      ss << "pi0 with no decay vertex found: BARCODES =";
      for (vector<int>::const_iterator b = unDecPi0.begin(); b != unDecPi0.end(); ++b)
        ss << " " << *b;
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_undecayedPi0CheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    // Undisplaced decay daughters of displaced vertices
    if (!undisplaceds.empty()) {
      stringstream ss;
      ss << "Undisplaced decay vertices from displaced particle: BARCODES =";
      for (vector<int>::const_iterator b = undisplaceds.begin(); b != undisplaceds.end(); ++b)
        ss << " " << *b;
      ATH_MSG_WARNING(ss.str());
      if (m_dumpEvent) (*itr)->print();
      setFilterPassed(false);
      ++m_undisplacedDecayDaughtersOfDisplacedVtxCheckRate;
      ++m_nFail;
      return StatusCode::SUCCESS;
    }

    }

  // End of execution for each event
  ++m_nPass;

  return StatusCode::SUCCESS;
  }


StatusCode TestHepMC::finalize() {

  ATH_MSG_INFO("Events passed = " << m_nPass << ", Events Failed = " << m_nFail);

  ATH_MSG_INFO(" Event rate with invalid Beam Particles = " << m_invalidBeamParticlesCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with beam particles and status not equal to 4 = " << m_beamParticleswithStatusNotFourCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with incorrect beam particle energies = " << m_beamEnergyCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with NaN (Not A Number) or inf found in the event record vertex positions = " << m_vtxNANandINFCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with vertices displaced more than " << m_max_dist_trans << "~mm in transverse direction for particles with status codes 1 and 2 = " << m_vtxDisplacedstatuscode12CheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with vertices displaced more than " << m_max_dist_trans << "~mm in transverse direction for particles with status code other than 1 and 2 = " << m_vtxDisplacedstatuscodenot12CheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with vertices displaced more than " << m_max_dist << "~mm = " << m_vtxDisplacedMoreThan_1m_CheckRateCnt*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with NAN (Not A Number) or inf found in particle momentum values = " << m_partMomentumNANandINFCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with undecayed pi0's with status 1 or 2 = " << m_undecayedPi0statuscode12CheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with unstable particles with no end vertex = " << m_unstableNoEndVtxCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with negative total energy like for tachyonic particles = " << m_negativeEnergyTachyonicCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with particles with improper decay properties = " << m_decayCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with undisplaced daughters of long lived hadrons = " << m_undisplacedLLHdaughtersCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with non zero photon mass = " << m_nonZeroPhotonMassCheckRate*100.0/double(m_nPass + m_nFail) << "% (not included in test efficiency)");
  ATH_MSG_INFO(" Event rate with no energy balance = " << m_energyBalanceCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with no momentum balance = " << m_momentumBalanceCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with negative energy particles = " << m_negativeEnergyCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with tachyons = " << m_tachyonCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with stable or unstable particles with no parents = " << m_stableUnstableNoParentCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with unstable particle with no decay vertex = " << m_unstablePartNoDecayVtxCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with undecayed Pi0's = " << m_undecayedPi0CheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with undisplaced decay daughters of displaced vertices = " << m_undisplacedDecayDaughtersOfDisplacedVtxCheckRate*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with particles with status 1 but lifetime < " << m_min_tau << "~ns = " << m_Status1ShortLifetime*100.0/double(m_nPass + m_nFail) << "%");
  ATH_MSG_INFO(" Event rate with energy sum of interacting particles non known by Geant4 above " << m_nonG4_energy_threshold << " MeV = " << m_nonG4_energyCheckRate*100.0/double(m_nPass + m_nFail) << "%");
 
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
