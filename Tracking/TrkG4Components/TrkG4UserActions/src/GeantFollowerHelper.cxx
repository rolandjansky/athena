/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerHelper.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// StoreGate
#include "TrkG4UserActions/GeantFollowerHelper.h"
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
// Trk
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PlaneSurface.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
//other

// constructor
Trk::GeantFollowerHelper::GeantFollowerHelper(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_extrapolator(""),
  m_extrapolateDirectly(true),
  m_extrapolateIncrementally(true),
  m_parameterCache(nullptr),
  m_tX0Cache(0.),
  m_validationTreeName("G4Follower_"+n),
  m_validationTreeDescription("Output of the G4Follower_"),
  m_validationTreeFolder("/val/G4Follower_"+n),
  m_validationTree(nullptr)
{
   // properties
   declareProperty("Extrapolator",                   m_extrapolator);
   declareProperty("ExtrapolateDirectly",            m_extrapolateDirectly);
   declareProperty("ExtrapolateIncrementally",       m_extrapolateIncrementally);
}

// destructor
Trk::GeantFollowerHelper::~GeantFollowerHelper()
{}

// Athena standard methods
// initialize
StatusCode Trk::GeantFollowerHelper::initialize()
{
  m_treeData = std::make_unique<TreeData>();
  
  if (m_extrapolator.retrieve().isFailure()){
    ATH_MSG_ERROR("Could not retrieve Extrapolator " << m_extrapolator << " . Abort.");
    return StatusCode::FAILURE;
  }

  // create the new Tree
  m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

  m_validationTree->Branch("InitX",        &m_treeData->m_t_x,       "initX/F");
  m_validationTree->Branch("InitY",        &m_treeData->m_t_y,       "initY/F");
  m_validationTree->Branch("InitZ",        &m_treeData->m_t_z,       "initZ/F");
  m_validationTree->Branch("InitTheta",    &m_treeData->m_t_theta,   "initTheta/F");
  m_validationTree->Branch("InitEta",      &m_treeData->m_t_eta,     "initEta/F");
  m_validationTree->Branch("InitPhi",      &m_treeData->m_t_phi,     "initPhi/F");
  m_validationTree->Branch("InitP",        &m_treeData->m_t_p,       "initP/F");
  m_validationTree->Branch("InitPdg",      &m_treeData->m_t_pdg,     "initPdg/I");
  m_validationTree->Branch("InitCharge",   &m_treeData->m_t_charge,  "initQ/F");

  m_validationTree->Branch("G4Steps",      &m_treeData->m_g4_steps, "g4steps/I");
  m_validationTree->Branch("G4StepP",      m_treeData->m_g4_p,      "g4stepP[g4steps]/F");
  m_validationTree->Branch("G4StepEta",    m_treeData->m_g4_eta,    "g4stepEta[g4steps]/F");
  m_validationTree->Branch("G4StepTheta",  m_treeData->m_g4_theta,  "g4stepTheta[g4steps]/F");
  m_validationTree->Branch("G4StepPhi",    m_treeData->m_g4_phi,    "g4stepPhi[g4steps]/F");
  m_validationTree->Branch("G4StepX",      m_treeData->m_g4_x,      "g4stepX[g4steps]/F");
  m_validationTree->Branch("G4StepY",      m_treeData->m_g4_y,      "g4stepY[g4steps]/F");
  m_validationTree->Branch("G4StepZ",      m_treeData->m_g4_z,      "g4stepZ[g4steps]/F");
  m_validationTree->Branch("G4AccumTX0",   m_treeData->m_g4_tX0,    "g4stepAccTX0[g4steps]/F");
  m_validationTree->Branch("G4StepT",      m_treeData->m_g4_t,      "g4stepTX[g4steps]/F");
  m_validationTree->Branch("G4StepX0",     m_treeData->m_g4_X0,     "g4stepX0[g4steps]/F");

  m_validationTree->Branch("TrkStepStatus",m_treeData->m_trk_status, "trkstepStatus[g4steps]/I");
  m_validationTree->Branch("TrkStepP",     m_treeData->m_trk_p,      "trkstepP[g4steps]/F");
  m_validationTree->Branch("TrkStepEta",   m_treeData->m_trk_eta,    "trkstepEta[g4steps]/F");
  m_validationTree->Branch("TrkStepTheta", m_treeData->m_trk_theta,  "trkstepTheta[g4steps]/F");
  m_validationTree->Branch("TrkStepPhi",   m_treeData->m_trk_phi,    "trkstepPhi[g4steps]/F");
  m_validationTree->Branch("TrkStepX",     m_treeData->m_trk_x,      "trkstepX[g4steps]/F");
  m_validationTree->Branch("TrkStepY",     m_treeData->m_trk_y,      "trkstepY[g4steps]/F");
  m_validationTree->Branch("TrkStepZ",     m_treeData->m_trk_z,      "trkstepZ[g4steps]/F");
  m_validationTree->Branch("TrkStepLocX",  m_treeData->m_trk_lx,     "trkstepLX[g4steps]/F");
  m_validationTree->Branch("TrkStepLocY",  m_treeData->m_trk_ly,     "trkstepLY[g4steps]/F");

  // now register the Tree
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR( "Could not find Hist Service -> Switching ValidationMode Off !" );
    delete m_validationTree; m_validationTree = 0;
  }
  if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
    ATH_MSG_ERROR( "Could not register the validation Tree -> Switching ValidationMode Off !" );
    delete m_validationTree; m_validationTree = 0;
  }

  ATH_MSG_INFO("initialize() successful" );
  return StatusCode::SUCCESS;
}

StatusCode Trk::GeantFollowerHelper::finalize()
{
   return StatusCode::SUCCESS;
}



void Trk::GeantFollowerHelper::beginEvent() const
{
  m_treeData->m_t_x        = 0.;
  m_treeData->m_t_y        = 0.;
  m_treeData->m_t_z        = 0.;
  m_treeData->m_t_theta    = 0.;
  m_treeData->m_t_eta      = 0.;
  m_treeData->m_t_phi      = 0.;
  m_treeData->m_t_p        = 0.;
  m_treeData->m_t_charge   = 0.;
  m_treeData->m_t_pdg      = 0;
  m_treeData->m_g4_steps   = 0;
  m_tX0Cache   = 0.;
}

void Trk::GeantFollowerHelper::trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const
{
  // construct the initial parameters
  Amg::Vector3D npos(pos.x(),pos.y(),pos.z());
  Amg::Vector3D nmom(mom.x(),mom.y(),mom.z());
  if (!m_treeData->m_g4_steps){
    ATH_MSG_INFO("Initial step ... preparing event cache.");
    m_treeData->m_t_x        = pos.x();
    m_treeData->m_t_y        = pos.y();
    m_treeData->m_t_z        = pos.z();
    m_treeData->m_t_theta    = mom.theta();
    m_treeData->m_t_eta      = mom.eta();
    m_treeData->m_t_phi      = mom.phi();
    m_treeData->m_t_p        = mom.mag();
    m_treeData->m_t_charge   = charge;
    m_treeData->m_t_pdg      = pdg;
    m_treeData->m_g4_steps   = -1;
    m_tX0Cache   = 0.;
    m_parameterCache = new Trk::CurvilinearParameters(npos, nmom, charge);
    return;
  }

  // jumping over inital step
  m_treeData->m_g4_steps = (m_treeData->m_g4_steps == -1) ? 0 : m_treeData->m_g4_steps;

  if (!m_parameterCache){
    ATH_MSG_WARNING("No Parameters available. Bailing out.");
    return;
  }

  if ( m_treeData->m_g4_steps >= MAXPROBES) {
    ATH_MSG_WARNING("Maximum number of " << MAXPROBES << " reached, step is ignored.");
    return;
  }
  // parameters of the G4 step point
  Trk::CurvilinearParameters* g4Parameters = new Trk::CurvilinearParameters(npos, nmom, m_treeData->m_t_charge);
  // destination surface
  const Trk::PlaneSurface& destinationSurface = g4Parameters->associatedSurface();
  // extrapolate to the destination surface
  const Trk::TrackParameters* trkParameters = m_extrapolateDirectly ?
    m_extrapolator->extrapolateDirectly(*m_parameterCache,destinationSurface,Trk::alongMomentum,false) :
    m_extrapolator->extrapolate(*m_parameterCache,destinationSurface,Trk::alongMomentum,false);
  // fill the geant information and the trk information
  m_treeData->m_g4_p[m_treeData->m_g4_steps]       =  mom.mag();
  m_treeData->m_g4_eta[m_treeData->m_g4_steps]     =  mom.eta();
  m_treeData->m_g4_theta[m_treeData->m_g4_steps]   =  mom.theta();
  m_treeData->m_g4_phi[m_treeData->m_g4_steps]     =  mom.phi();
  m_treeData->m_g4_x[m_treeData->m_g4_steps]       =  pos.x();
  m_treeData->m_g4_y[m_treeData->m_g4_steps]       =  pos.y();
  m_treeData->m_g4_z[m_treeData->m_g4_steps]       =  pos.z();
  float tX0 = X0 > 10e-5 ? t/X0 : 0.;
  m_tX0Cache              += tX0;
  m_treeData->m_g4_tX0[m_treeData->m_g4_steps]     = m_tX0Cache;
  m_treeData->m_g4_t[m_treeData->m_g4_steps]       = t;
  m_treeData->m_g4_X0[m_treeData->m_g4_steps]      = X0;

  m_treeData->m_trk_status[m_treeData->m_g4_steps] = trkParameters ? 1 : 0;
  m_treeData->m_trk_p[m_treeData->m_g4_steps]      = trkParameters ? trkParameters->momentum().mag()      : 0.;
  m_treeData->m_trk_eta[m_treeData->m_g4_steps]    = trkParameters ? trkParameters->momentum().eta()      : 0.;
  m_treeData->m_trk_theta[m_treeData->m_g4_steps]  = trkParameters ? trkParameters->momentum().theta()    : 0.;
  m_treeData->m_trk_phi[m_treeData->m_g4_steps]    = trkParameters ? trkParameters->momentum().phi()      : 0.;
  m_treeData->m_trk_x[m_treeData->m_g4_steps]      = trkParameters ? trkParameters->position().x()        : 0.;
  m_treeData->m_trk_y[m_treeData->m_g4_steps]      = trkParameters ? trkParameters->position().y()        : 0.;
  m_treeData->m_trk_z[m_treeData->m_g4_steps]      = trkParameters ? trkParameters->position().z()        : 0.;
  m_treeData->m_trk_lx[m_treeData->m_g4_steps]     = trkParameters ? trkParameters->parameters()[Trk::locX] : 0.;
  m_treeData->m_trk_ly[m_treeData->m_g4_steps]     = trkParameters ? trkParameters->parameters()[Trk::locY] : 0.;

  // update the parameters if needed/configured
  if (m_extrapolateIncrementally && trkParameters) {
    delete m_parameterCache;
    m_parameterCache = trkParameters;
  }
  // delete cache and increment
  delete g4Parameters;
  ++m_treeData->m_g4_steps;
}

void Trk::GeantFollowerHelper::endEvent() const
{
    // fill the validation tree
    m_validationTree->Fill();
    delete m_parameterCache;
}
      
      


