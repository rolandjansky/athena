/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GeneratorObjects/McEventCollection.h"
#include "PathResolver/PathResolver.h"

#include "ForwardTransportSvc.h"
#include "ForwardTransportSvc/IForwardTransportSvc.h"
#include "ForwardTracker/TwissFile.h"

#include <fstream>
#include "TTree.h"

ForwardTransportSvc::ForwardTransportSvc(const std::string& name, ISvcLocator* svc) : 
  AthService (name,           svc),
  m_StoreGate("StoreGateSvc", name),
  m_thistSvc ("THistSvc",     name) 
{  
  declareProperty("TwissFile1",    m_cData.twissFile1);
  declareProperty("TwissFile2",    m_cData.twissFile2);
  declareProperty("PositionC1",    m_cData.positionC1);
  declareProperty("PositionC2",    m_cData.positionC2);
  declareProperty("ApertureC1",    m_cData.apertureC1);
  declareProperty("ApertureC2",    m_cData.apertureC2);
  declareProperty("EndMarker",     m_cData.endMarker);
  declareProperty("TransportFlag", m_transportFlag = 0);     // Transport only neutrals (0) or charged (1)
  declareProperty("EtaCut",        m_etaCut        = 7.5);   // Minimum pseudorapidty of transported particles
  declareProperty("XiCut",         m_xiCut         = 0.8);   // Minimum momentum over beam energy of transported particles
  declareProperty("FillRootTree",  m_fillRootTree  = false); // Turn on output debug tree
  declareProperty("RootFilePath",  m_rootFilePath  = ".");   // Path to output debug file
  declareProperty("McCollection",  m_MCkey         = "TruthEvent");
  declareProperty("THistSvc",	   m_thistSvc);
  declareProperty("StoreGateSvc",  m_StoreGate);  
  
  m_tree = 0; 

  m_evt = 0;
  m_pid = 0;
  m_x0  = 0;
  m_y0  = 0;
  m_z0  = 0;
  m_t0  = 0;
  m_px0 = 0;
  m_py0 = 0;
  m_pz0 = 0;
  m_e0  = 0;
  m_x1  = 0;
  m_y1  = 0;
  m_z1  = 0;
  m_t1  = 0;
  m_x2  = 0;
  m_y2  = 0;
  m_z2  = 0;
  m_t2  = 0;
  m_x   = 0;
  m_y   = 0;
  m_z   = 0;
  m_t   = 0;
  m_px  = 0;
  m_py  = 0;
  m_pz  = 0;
  m_e   = 0;
  m_ist = 0;
}

StatusCode ForwardTransportSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

  if (IID_IForwardTransportSvc == riid) *ppvInterface = (IForwardTransportSvc*)this;
  else                                  return AthService::queryInterface(riid, ppvInterface);
  
  addRef();
  
  return StatusCode::SUCCESS;
}

StatusCode ForwardTransportSvc::initialize() {  

  ATH_MSG_INFO("ForwardTransportSvc::initialize");
    
  m_cData.twissFile1 = PathResolverFindCalibFile( m_cData.twissFile1 );
  m_cData.twissFile2 = PathResolverFindCalibFile( m_cData.twissFile2 );

  ATH_MSG_DEBUG( "Using the following Twiss files:" );
  ATH_MSG_DEBUG( "  - " << m_cData.twissFile1 );
  ATH_MSG_DEBUG( "  - " << m_cData.twissFile2 );


  if (!m_fillRootTree) return StatusCode::SUCCESS;

  m_tree = new TTree("t", "FwdTransportAnalysis");
  
  if (m_thistSvc->regTree("/AANT/FwdTransportAnalysis", m_tree).isFailure()) { ATH_MSG_WARNING(" Unable to register the tree "); return StatusCode::SUCCESS; }
  
  m_tree->Branch("evt", &m_evt, "evt/I");
  m_tree->Branch("pid", &m_pid, "pid/I");
  m_tree->Branch("x0",  &m_x0,  "x0/D");
  m_tree->Branch("y0",  &m_y0,  "y0/D");
  m_tree->Branch("z0",  &m_z0,  "z0/D");
  m_tree->Branch("t0",  &m_t0,  "t0/D");
  m_tree->Branch("px0", &m_px0, "px0/D");
  m_tree->Branch("py0", &m_py0, "py0/D");
  m_tree->Branch("pz0", &m_pz0, "pz0/D");
  m_tree->Branch("e0",  &m_e0,  "e0/D");
  m_tree->Branch("x1",  &m_x1,  "x1/D");
  m_tree->Branch("y1",  &m_y1,  "y1/D");
  m_tree->Branch("z1",  &m_z1,  "z1/D");
  m_tree->Branch("t1",  &m_t1,  "t1/D");
  m_tree->Branch("x2",  &m_x2,  "x2/D");
  m_tree->Branch("y2",  &m_y2,  "y2/D");
  m_tree->Branch("z2",  &m_z2,  "z2/D");
  m_tree->Branch("t2",  &m_t2,  "t2/D");
  m_tree->Branch("x",   &m_x,   "x/D");
  m_tree->Branch("y",   &m_y,   "y/D");
  m_tree->Branch("z",   &m_z,   "z/D");
  m_tree->Branch("t",   &m_t,   "t/D");
  m_tree->Branch("px",  &m_px,  "px/D");
  m_tree->Branch("py",  &m_py,  "py/D");
  m_tree->Branch("pz",  &m_pz,  "pz/D");
  m_tree->Branch("e",   &m_e,   "e/D");
  m_tree->Branch("ist", &m_ist, "ist/O");

  return StatusCode::SUCCESS;
}  

bool ForwardTransportSvc::selectedParticle(G4ThreeVector momentum, int pid) {
  
  std::ifstream pfile(m_cData.twissFile1.c_str());
  double beamEnergy = ForwardTracker::GetBeamEnergy(pfile)*Gaudi::Units::GeV;
  
  double eta = fabs(momentum.pseudoRapidity());
  double xi  = momentum.mag()/beamEnergy; 
  
  ATH_MSG_INFO(" pseudoRapidity: " << std::setprecision(9) << eta);
  ATH_MSG_INFO(" p/beamEnergy:   " << std::setprecision(9) << xi);
  
  if (eta < m_etaCut) return false;
  if (xi  < m_xiCut)  return false;
  
  if (m_transportFlag) {
    
    if (pid == 2212) return true; // proton
  }
  else {
    
    if (pid == 2112) return true; // neutron
    if (pid == 22)   return true; // gamma
    if (pid == 3122) return true; // lambda
  }
  
  return false;
}

void ForwardTransportSvc::fillMcInfo(G4ThreeVector pos, double tim, G4ThreeVector mom, double ene) {
  
  if (!m_fillRootTree) return;
  
  m_x0  = pos.x(); // [mm]
  m_y0  = pos.y(); // [mm]
  m_z0  = pos.z(); // [mm]
  m_t0  = tim;     // [ns]
  m_px0 = mom.x(); // [MeV]
  m_py0 = mom.y(); // [MeV]
  m_pz0 = mom.z(); // [MeV]
  m_e0  = ene;     // [MeV]

  int sign = (m_pz0 > 0) ? 1 : -1;
  
  //Straight line projection at the end of the ATLAS cavern (z = 19.040 m). Interesting for neutrons.
  m_z1 = sign*19.040*Gaudi::Units::m;
  m_x1 = m_x0 + (m_z1 - m_z0)*m_px0/m_pz0;
  m_y1 = m_y0 + (m_z1 - m_z0)*m_py0/m_pz0;
  m_t1 = m_t0 + (m_z1 - m_z0)*m_e0 /m_pz0;
  m_t1 = m_t1/Gaudi::Units::c_light;
  
  //Straight line projection at the detector position (z = endMarker). Interesting for neutrons.
  m_z2 = sign*m_cData.endMarker*Gaudi::Units::m;
  m_x2 = m_x0 + (m_z2 - m_z0)*m_px0/m_pz0;
  m_y2 = m_y0 + (m_z2 - m_z0)*m_py0/m_pz0;
  m_t2 = m_t0 + (m_z2 - m_z0)*m_e0 /m_pz0;
  m_t2 = m_t2/Gaudi::Units::c_light;
}

void ForwardTransportSvc::addMcVertex(G4ThreeVector pos, double tim, G4ThreeVector mom, double ene, int pid, bool isTransported, HepMC::GenEvent* gEvent) {

  int statusCode = (isTransported) ? 212 : 211;
  
  HepMC::GenVertex*   gVertex   = new HepMC::GenVertex  (HepMC::FourVector(pos.x(), pos.y(), pos.z(), tim));
  HepMC::GenParticle* gParticle = new HepMC::GenParticle(HepMC::FourVector(mom.x(), mom.y(), mom.z(), ene), pid, statusCode);
  
  gVertex->add_particle_out(gParticle);
 
  gEvent->add_vertex(gVertex);

  if (!m_fillRootTree) return;

  m_evt = gEvent->event_number();
  m_pid = pid;
  m_x   = pos.x(); // [mm]
  m_y   = pos.y(); // [mm]
  m_z   = pos.z(); // [mm]
  m_t   = tim;     // [ns]
  m_px  = mom.x(); // [MeV]
  m_py  = mom.y(); // [MeV]
  m_pz  = mom.z(); // [MeV]
  m_e   = ene;     // [MeV]
  m_ist = isTransported;
  
  if (m_tree) m_tree->Fill();
}

HepMC::GenEvent* ForwardTransportSvc::getEvent() {
  
  const McEventCollection* mcCollptr;
  
  if (m_StoreGate->retrieve(mcCollptr, m_MCkey).isFailure()) { ATH_MSG_WARNING(" Unable to retrieve the McEventCollection! "); return 0; } 

  HepMC::GenEvent* gEvent = 0;
  
  for (McEventCollection::const_iterator itr = mcCollptr->begin(); itr != mcCollptr->end(); ++itr) {
    
    gEvent = (*itr);
  }
  
  return gEvent;
}
