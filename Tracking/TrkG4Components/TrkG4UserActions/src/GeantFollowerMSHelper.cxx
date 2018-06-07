/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMSHelper.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// StoreGate
#include "TrkG4UserActions/GeantFollowerMSHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// Trk
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkExUtils/ExtrapolationCache.h"


// constructor
Trk::GeantFollowerMSHelper::GeantFollowerMSHelper(const std::string& t, const std::string& n, const IInterface* p)
 : base_class(t,n,p)
 , m_extrapolator("")
 , m_elossupdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator")
 , m_extrapolateDirectly(false)
 , m_extrapolateIncrementally(false)
 , m_speedup(false)
 , m_parameterCache(nullptr)
 , m_parameterCacheCov(nullptr)
 , m_parameterCacheMS(nullptr)
 , m_parameterCacheMSCov(nullptr)
 , m_tX0Cache(0.)
 , m_crossedMuonEntry(false)
 , m_exitLayer(false)
 , m_destinationSurface(nullptr)
 , m_validationTreeName("G4Follower")
 , m_validationTreeDescription("Output of the G4Follower_")
 , m_validationTreeFolder("/val/G4Follower")
 , m_validationTree(nullptr)
 , m_t_x(0.)
 , m_t_y(0.)
 , m_t_z(0.)
 , m_t_theta(0.)
 , m_t_eta(0.)
 , m_t_phi(0.)
 , m_t_p(0.)
 , m_t_charge(0.)
 , m_t_pdg(0)
 , m_m_x(0.)
 , m_m_y(0.)
 , m_m_z(0.)
 , m_m_theta(0.)
 , m_m_eta(0.)
 , m_m_phi(0.)
 , m_m_p(0.)
 , m_b_x(0.)
 , m_b_y(0.)
 , m_b_z(0.)
 , m_b_theta(0.)
 , m_b_eta(0.)
 , m_b_phi(0.)
 , m_b_p(0.)
 , m_b_X0(0.)
 , m_b_Eloss(0.)
 , m_g4_steps(-1)
 , m_g4_p{0}
 , m_g4_eta{0}
 , m_g4_theta{0}
 , m_g4_phi{0}
 , m_g4_x{0}
 , m_g4_y{0}
 , m_g4_z{0}
 , m_g4_tX0{0}
 , m_g4_t{0}
 , m_g4_X0{0}
 , m_trk_status{0}
 , m_trk_p{0}
 , m_trk_eta{0}
 , m_trk_theta{0}
 , m_trk_phi{0}
 , m_trk_x{0}
 , m_trk_y{0}
 , m_trk_z{0}
 , m_trk_lx{0}
 , m_trk_ly{0}
 , m_trk_eloss{0}
 , m_trk_eloss1{0}
 , m_trk_eloss0{0}
 , m_trk_eloss5{0}
 , m_trk_eloss10{0}
 , m_trk_scaleeloss{0}
 , m_trk_scalex0{0}
 , m_trk_x0{0}
 , m_trk_erd0{0}
 , m_trk_erz0{0}
 , m_trk_erphi{0}
 , m_trk_ertheta{0}
 , m_trk_erqoverp{0}
 , m_trk_scats(0)
 , m_trk_sstatus{0}
 , m_trk_sx{0}
 , m_trk_sy{0}
 , m_trk_sz{0}
 , m_trk_sx0{0}
 , m_trk_seloss{0}
 , m_trk_smeanIoni{0}
 , m_trk_ssigIoni{0}
 , m_trk_smeanRad{0}
 , m_trk_ssigRad{0}
 , m_trk_ssigTheta{0}
 , m_trk_ssigPhi{0}
 , m_g4_stepsMS(-1)

{
   // properties
   declareProperty("Extrapolator",                   m_extrapolator);
   declareProperty("ExtrapolateDirectly",            m_extrapolateDirectly);
   declareProperty("ExtrapolateIncrementally",       m_extrapolateIncrementally);

// SpeedUp False takes more CPU because it will stop at each G4 Step in the Muon Spectrometer

   declareProperty("SpeedUp",                        m_speedup);
}

// destructor
Trk::GeantFollowerMSHelper::~GeantFollowerMSHelper()
{}

// Athena standard methods
// initialize
StatusCode Trk::GeantFollowerMSHelper::initialize()
{
    
   if (m_extrapolator.retrieve().isFailure()){
       ATH_MSG_ERROR("Could not retrieve Extrapolator " << m_extrapolator << " . Abort.");
       return StatusCode::FAILURE;
   }
    
   if (m_elossupdator.retrieve().isFailure()){
       ATH_MSG_ERROR("Could not retrieve ElossUpdator " << m_elossupdator << " . Abort.");
       return StatusCode::FAILURE;
   }
    
   if(m_speedup) {
      ATH_MSG_INFO(" SpeedUp GeantFollowerMS ");
   } else {
      ATH_MSG_INFO(" NO SpeedUp GeantFollowerMS ");
   }
   ATH_MSG_INFO("initialize()");
   
   // create the new Tree
   m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
   
   m_validationTree->Branch("InitX",        &m_t_x,       "initX/F");
   m_validationTree->Branch("InitY",        &m_t_y,       "initY/F");
   m_validationTree->Branch("InitZ",        &m_t_z,       "initZ/F");
   m_validationTree->Branch("InitTheta",    &m_t_theta,   "initTheta/F");
   m_validationTree->Branch("InitEta",      &m_t_eta,     "initEta/F");
   m_validationTree->Branch("InitPhi",      &m_t_phi,     "initPhi/F");
   m_validationTree->Branch("InitP",        &m_t_p,       "initP/F");
   m_validationTree->Branch("InitPdg",      &m_t_pdg,     "initPdg/I");
   m_validationTree->Branch("InitCharge",   &m_t_charge,  "initQ/F");

   m_validationTree->Branch("MEntryX",        &m_m_x,       "mentryX/F");
   m_validationTree->Branch("MEntryY",        &m_m_y,       "mentryY/F");
   m_validationTree->Branch("MEntryZ",        &m_m_z,       "mentryZ/F");
   m_validationTree->Branch("MEntryTheta",    &m_m_theta,   "mentryTheta/F");
   m_validationTree->Branch("MEntryEta",      &m_m_eta,     "mentryEta/F");
   m_validationTree->Branch("MEntryPhi",      &m_m_phi,     "mentryPhi/F");
   m_validationTree->Branch("MEntryP",        &m_m_p,       "mentryP/F");

   m_validationTree->Branch("BackX",        &m_b_x,       "backX/F");
   m_validationTree->Branch("BackY",        &m_b_y,       "backY/F");
   m_validationTree->Branch("BackZ",        &m_b_z,       "backZ/F");
   m_validationTree->Branch("BackTheta",    &m_b_theta,   "backTheta/F");
   m_validationTree->Branch("BackEta",      &m_b_eta,     "backEta/F");
   m_validationTree->Branch("BackPhi",      &m_b_phi,     "backPhi/F");
   m_validationTree->Branch("BackP",        &m_b_p,       "backP/F");
   m_validationTree->Branch("BackX0",       &m_b_X0,      "backX0/F");
   m_validationTree->Branch("BackEloss",    &m_b_Eloss,   "backEloss/F");
 
   m_validationTree->Branch("G4Steps",      &m_g4_steps, "g4steps/I");
   m_validationTree->Branch("TrkStepScats", &m_trk_scats, "trkscats/I");

   m_validationTree->Branch("G4StepP",      m_g4_p,      "g4stepP[g4steps]/F");
   m_validationTree->Branch("G4StepEta",    m_g4_eta,    "g4stepEta[g4steps]/F");
   m_validationTree->Branch("G4StepTheta",  m_g4_theta,  "g4stepTheta[g4steps]/F");
   m_validationTree->Branch("G4StepPhi",    m_g4_phi,    "g4stepPhi[g4steps]/F");
   m_validationTree->Branch("G4StepX",      m_g4_x,      "g4stepX[g4steps]/F");
   m_validationTree->Branch("G4StepY",      m_g4_y,      "g4stepY[g4steps]/F");
   m_validationTree->Branch("G4StepZ",      m_g4_z,      "g4stepZ[g4steps]/F");
   m_validationTree->Branch("G4AccumTX0",   m_g4_tX0,    "g4stepAccTX0[g4steps]/F");
   m_validationTree->Branch("G4StepT",      m_g4_t,      "g4stepTX[g4steps]/F");
   m_validationTree->Branch("G4StepX0",     m_g4_X0,     "g4stepX0[g4steps]/F");
   
   m_validationTree->Branch("TrkStepStatus",m_trk_status, "trkstepStatus[g4steps]/I");
   m_validationTree->Branch("TrkStepP",     m_trk_p,      "trkstepP[g4steps]/F");
   m_validationTree->Branch("TrkStepEta",   m_trk_eta,    "trkstepEta[g4steps]/F");
   m_validationTree->Branch("TrkStepTheta", m_trk_theta,  "trkstepTheta[g4steps]/F");
   m_validationTree->Branch("TrkStepPhi",   m_trk_phi,    "trkstepPhi[g4steps]/F");
   m_validationTree->Branch("TrkStepX",     m_trk_x,      "trkstepX[g4steps]/F");
   m_validationTree->Branch("TrkStepY",     m_trk_y,      "trkstepY[g4steps]/F");
   m_validationTree->Branch("TrkStepZ",     m_trk_z,      "trkstepZ[g4steps]/F");
   m_validationTree->Branch("TrkStepLocX",  m_trk_lx,     "trkstepLX[g4steps]/F");
   m_validationTree->Branch("TrkStepLocY",  m_trk_ly,     "trkstepLY[g4steps]/F");
   m_validationTree->Branch("TrkStepEloss", m_trk_eloss,  "trkstepEloss[g4steps]/F");
   m_validationTree->Branch("TrkStepEloss1", m_trk_eloss1, "trkstepEloss1[g4steps]/F");
   m_validationTree->Branch("TrkStepEloss0", m_trk_eloss0, "trkstepEloss0[g4steps]/F");
   m_validationTree->Branch("TrkStepEloss5", m_trk_eloss5, "trkstepEloss5[g4steps]/F");
   m_validationTree->Branch("TrkStepEloss10", m_trk_eloss10,"trkstepEloss10[g4steps]/F");
   m_validationTree->Branch("TrkStepScaleEloss",m_trk_scaleeloss, "trkstepScaleEloss[g4steps]/F");
   m_validationTree->Branch("TrkStepScaleX0",m_trk_scalex0,"trkstepScaleX0[g4steps]/F");
   m_validationTree->Branch("TrkStepX0",    m_trk_x0,     "trkstepX0[g4steps]/F");
   m_validationTree->Branch("TrkStepErd0",  m_trk_erd0,   "trkstepErd0[g4steps]/F");
   m_validationTree->Branch("TrkStepErz0",  m_trk_erz0,   "trkstepErz0[g4steps]/F");
   m_validationTree->Branch("TrkStepErphi", m_trk_erphi,   "trkstepErphi[g4steps]/F");
   m_validationTree->Branch("TrkStepErtheta",m_trk_ertheta,"trkstepErtheta[g4steps]/F");
   m_validationTree->Branch("TrkStepErqoverp",m_trk_erqoverp,"trkstepErqoverp[g4steps]/F");
   
   m_validationTree->Branch("TrkStepScatStatus", m_trk_sstatus,"trkscatStatus[trkscats]/I");
   m_validationTree->Branch("TrkStepScatX",      m_trk_sx,     "trkscatX[trkscats]/F");
   m_validationTree->Branch("TrkStepScatY",      m_trk_sy,     "trkscatY[trkscats]/F");
   m_validationTree->Branch("TrkStepScatZ",      m_trk_sz,     "trkscatZ[trkscats]/F");
   m_validationTree->Branch("TrkStepScatX0",     m_trk_sx0,    "trkscatX0[trkscats]/F");
   m_validationTree->Branch("TrkStepScatEloss",  m_trk_seloss, "trkscatEloss[trkscats]/F");
   m_validationTree->Branch("TrkStepScatMeanIoni",m_trk_smeanIoni, "trkscatMeanIoni[trkscats]/F");
   m_validationTree->Branch("TrkStepScatSigIoni",m_trk_ssigIoni, "trkscatSigIoni[trkscats]/F");
   m_validationTree->Branch("TrkStepScatMeanRad",m_trk_smeanRad, "trkscatMeanRad[trkscats]/F");
   m_validationTree->Branch("TrkStepScatSigRad", m_trk_ssigRad, "trkscatSigRad[trkscats]/F");
   m_validationTree->Branch("TrkStepScatSigTheta", m_trk_ssigTheta, "trkscatSigTheta[trkscats]/F");
   m_validationTree->Branch("TrkStepScatSigPhi", m_trk_ssigPhi, "trkscatSigPhi[trkscats]/F");

   // initialize
   //
   m_t_x        = 0.;    
   m_t_y        = 0.; 
   m_t_z        = 0.; 
   m_t_theta    = 0.; 
   m_t_eta      = 0.; 
   m_t_phi      = 0.; 
   m_t_p        = 0.; 
   m_t_charge   = 0.; 
   m_t_pdg      = 0;         
   m_g4_steps   = -1;

   m_m_x        = 0.;
   m_m_y        = 0.;
   m_m_z        = 0.;
   m_m_theta    = 0.;
   m_m_eta      = 0.;
   m_m_phi      = 0.;
   m_m_p        = 0.;

   m_b_x        = 0.;
   m_b_y        = 0.;
   m_b_z        = 0.;
   m_b_theta    = 0.;
   m_b_eta      = 0.;
   m_b_phi      = 0.;
   m_b_p        = 0.;
   m_b_X0       = 0.;
   m_b_Eloss    = 0.;

   m_trk_scats  = 0;
   
   m_crossedMuonEntry = false;
   m_exitLayer = false;   
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

StatusCode Trk::GeantFollowerMSHelper::finalize()
{
   return StatusCode::SUCCESS;
}



void Trk::GeantFollowerMSHelper::beginEvent() const
{
    m_t_x        = 0.;    
    m_t_y        = 0.;
    m_t_z        = 0.;
    m_t_theta    = 0.; 
    m_t_eta      = 0.;
    m_t_phi      = 0.;
    m_t_p        = 0.;
    m_t_charge   = 0.;
    m_t_pdg      = 0; 

    m_m_x        = 0.;
    m_m_y        = 0.;
    m_m_z        = 0.;
    m_m_theta    = 0.;
    m_m_eta      = 0.;
    m_m_phi      = 0.;
    m_m_p        = 0.;

    m_b_x        = 0.;
    m_b_y        = 0.;
    m_b_z        = 0.;
    m_b_theta    = 0.;
    m_b_eta      = 0.;
    m_b_phi      = 0.;
    m_b_p        = 0.;
    m_b_X0       = 0.;
    m_b_Eloss    = 0.;

    m_g4_steps   = -1;
    m_g4_stepsMS = -1;
    m_trk_scats  = 0;
    m_tX0Cache   = 0.;

    m_crossedMuonEntry = false;
    m_exitLayer = false;   
 
}

void Trk::GeantFollowerMSHelper::trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const
{
    
// as the MS starts at 6736 in R.07 the cut is just before

    double zMuonEntry = 6735.;
//    zMuonEntry = 6000.;

    double scale = 1.;
//    if(pos.perp()>13000.) scale = 13000./pos.perp();

//    std::cout << " trackParticle eta " << mom.eta() << " radius " << pos.perp() << " z " << pos.z() << " charge " << charge << " scale " << scale << std::endl; 
    
    Amg::Vector3D npos(scale*pos.x(),scale*pos.y(),scale*pos.z());                                                                                                  
    Amg::Vector3D nmom(mom.x(),mom.y(),mom.z());                                                                                                  

//    if(m_crossedMuonEntry)  std::cout << " crossed Muon Entry " << std::endl;
//    if(m_exitLayer)  std::cout << " crossed Exit Layer " << std::endl;

    if (m_g4_steps==-1){
        ATH_MSG_INFO("Initial step ... preparing event cache.");
        m_t_x        = npos.x();        
        m_t_y        = npos.y(); 
        m_t_z        = npos.z(); 
        m_t_theta    = nmom.theta(); 
        m_t_eta      = nmom.eta(); 
        m_t_phi      = nmom.phi(); 
        m_t_p        = nmom.mag(); 
        m_t_charge   = charge; 
        m_t_pdg      = pdg;         
        m_g4_steps   = 0;
        m_tX0Cache   = 0.;
        // construct the intial parameters
        
        m_parameterCache = new Trk::CurvilinearParameters(npos, nmom, charge);
        AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5);;
        covMatrix->setZero();
        ATH_MSG_DEBUG( " covMatrix " << covMatrix);
        m_parameterCacheCov = new Trk::CurvilinearParameters(npos, nmom, charge, covMatrix);
        ATH_MSG_DEBUG( " Made m_parameterCacheCov with covMatrix " << covMatrix);
        return;
    }

    float tX0 = X0 > 10e-5 ? t/X0 : 0.;
    m_tX0Cache              += tX0;
    ATH_MSG_DEBUG(" position R " << npos.perp() << " z " << npos.z() << " X0 " << X0 << " t " << t << " m_tX0Cache " << m_tX0Cache);

    bool m_useMuonEntry = true;
// Stop in ID temp
//    if(m_useMuonEntry&&!m_crossedMuonEntry&&(fabs(npos.z())>2744||npos.perp()>1106)) {
// Muon Entry 
    if(m_useMuonEntry&&!m_crossedMuonEntry&&(fabs(npos.z())>zMuonEntry||npos.perp()>4254)) {
        m_m_x        = npos.x();
        m_m_y        = npos.y();
        m_m_z        = npos.z();
        m_m_theta    = nmom.theta();
        m_m_eta      = nmom.eta();
        m_m_phi      = nmom.phi();
        m_m_p        = nmom.mag();
// overwrite everything before ME layer
        m_g4_stepsMS   = 0;
        // construct the intial parameters
        m_parameterCacheMS = new Trk::CurvilinearParameters(npos, nmom, charge);
        m_parameterCache = new Trk::CurvilinearParameters(npos, nmom, charge);
        AmgSymMatrix(5)* covMatrix = new AmgSymMatrix(5);
        covMatrix->setZero();
        m_parameterCacheMSCov = new Trk::CurvilinearParameters(npos, nmom, charge, covMatrix);
        ATH_MSG_DEBUG( "m_crossedMuonEntry x " << m_parameterCacheMS->position().x() << " y " << m_parameterCacheMS->position().y() << " z " << m_parameterCacheMS->position().z() );
        m_crossedMuonEntry = true;
        Trk::CurvilinearParameters* g4Parameters = new Trk::CurvilinearParameters(npos, nmom, m_t_charge);
// Muon Entry
        m_destinationSurface = &(g4Parameters->associatedSurface());
        delete g4Parameters;
    }

    
    // jumping over inital step
    m_g4_steps = (m_g4_steps == -1) ? 0 : m_g4_steps;
    
    if (!m_parameterCache){
        ATH_MSG_WARNING("No Parameters available. Bailing out.");
        return;
    }
    
    if ( m_g4_steps >= MAXPROBES) {
        ATH_MSG_WARNING("Maximum number of " << MAXPROBES << " reached, step is ignored.");
        return;
    }

//    std::cout << " HERE we are m_tX0Cache " << m_tX0Cache << std::endl;
//    if(m_crossedMuonEntry)  std::cout << " Second crossed Muon Entry " << std::endl;

// DO NOT store before MuonEntry (gain CPU) 

    if(!m_crossedMuonEntry) return;
    if(m_exitLayer) return;
    if(m_speedup) {
      bool crossedExitLayer = false;
// ID envelop
//      if(fabs(npos.z())>zMuonEntry||npos.perp()>4255) crossedExitLayer = true;
      if(fabs(npos.z())>21800||npos.perp()>12500) crossedExitLayer = true;
      if(m_crossedMuonEntry&&m_g4_steps>=2&&!crossedExitLayer) return;
      if(m_g4_steps>2) return;
      if(m_g4_steps>4) return;
    }

    Trk::EnergyLoss* eloss = new EnergyLoss(0.,0.,0.,0.,0.,0);
    Trk::ExtrapolationCache* extrapolationCache =  new ExtrapolationCache(0.,eloss);

// Cache ONLY used for extrapolateM and extrapolate with covariance Matrix

    // parameters of the G4 step point
    Trk::CurvilinearParameters* g4Parameters = new Trk::CurvilinearParameters(npos, nmom, m_t_charge);
    // destination surface
    const Trk::PlaneSurface& destinationSurface = g4Parameters->associatedSurface();
    // extrapolate to the destination surface
    const Trk::TrackParameters* trkParameters = m_extrapolateDirectly&&m_crossedMuonEntry ?
        m_extrapolator->extrapolateDirectly(*m_parameterCache,destinationSurface,Trk::alongMomentum,false,Trk::muon) :
        m_extrapolator->extrapolate(*m_parameterCache,destinationSurface,Trk::alongMomentum,false,Trk::muon);
    if(m_g4_stepsMS==0) {
        ATH_MSG_DEBUG( " Extrapolate m_parameterCacheCov with covMatrix ");
        extrapolationCache->reset();
        trkParameters = m_extrapolateDirectly&&m_crossedMuonEntry ?
        m_extrapolator->extrapolateDirectly(*m_parameterCacheCov,destinationSurface,Trk::alongMomentum,false,Trk::muon) :
        m_extrapolator->extrapolate(*m_parameterCacheCov,destinationSurface,Trk::alongMomentum,false,Trk::muon,Trk::addNoise,extrapolationCache);

        ATH_MSG_DEBUG(" G4 extrapolate to Muon Entry system " << " X0 " << extrapolationCache->x0tot() << " Eloss deltaE " <<   extrapolationCache->eloss()->deltaE()  << " Eloss sigma " << extrapolationCache->eloss()->sigmaDeltaE() << " meanIoni " << extrapolationCache->eloss()->meanIoni()  << " sigmaIoni " << extrapolationCache->eloss()->sigmaIoni() << " meanRad " <<  extrapolationCache->eloss()->meanRad() << " sigmaRad " << extrapolationCache->eloss()->sigmaRad()  << " depth " << extrapolationCache->eloss()->length());


        ATH_MSG_DEBUG( " Extrapolation OK ");
    }
    
    //sroe: coverity 31530
    m_trk_status[m_g4_steps] = trkParameters ? 1 : 0;

    if(!trkParameters) {
      delete eloss;
      delete extrapolationCache;
      delete g4Parameters;
      return;
    } 
// 
// Max radius 13400 maxZ 26050
//
// ID exit and MS entry
//    if(!m_exitLayer&&(fabs(npos.z())>zMuonEntry||npos.perp()>4255)&&trkParameters) {
    if(!m_exitLayer&&(fabs(npos.z())>21800||npos.perp()>12500)&&trkParameters) {
      ATH_MSG_DEBUG (" exit layer found ");
      m_trk_status[m_g4_steps] =  1000;
// Get extrapolatio with errors 
      extrapolationCache->reset();
      trkParameters = m_extrapolateDirectly&&m_crossedMuonEntry ?
      m_extrapolator->extrapolateDirectly(*m_parameterCacheMSCov,destinationSurface,Trk::alongMomentum,false,Trk::muon) :
      m_extrapolator->extrapolate(*m_parameterCacheMSCov,destinationSurface,Trk::alongMomentum,false,Trk::muon,Trk::addNoise,extrapolationCache);
// Forward from ME to Exit
      const Trk::TrackParameters* trkParameters_FW = m_extrapolateDirectly ?
        m_extrapolator->extrapolateDirectly(*m_parameterCacheMS,destinationSurface,Trk::alongMomentum,false,Trk::muon) :
        m_extrapolator->extrapolate(*m_parameterCacheMS,destinationSurface,Trk::alongMomentum,false,Trk::muon);
// Backwards from Exit to ME 
      if(trkParameters_FW) {
       ATH_MSG_DEBUG (" forward extrapolation succeeded ");
       bool doBackWard = false;
       if(doBackWard) { 
        const Trk::TrackParameters* trkParameters_BACK = m_extrapolateDirectly ?
          m_extrapolator->extrapolateDirectly(*trkParameters_FW,*m_destinationSurface,Trk::oppositeMomentum,false,Trk::muon) :
          m_extrapolator->extrapolate(*trkParameters_FW,*m_destinationSurface,Trk::oppositeMomentum,false,Trk::muon);
//        if(m_usePropagator) {
//          std::cout << " Use Propagator " << std::endl;
//          trkParameters_BACK = m_propagator->propagate(*trkParameters,*m_destinationSurface,Trk::oppositeMomentum,false,*m_magFieldProperties);
//          if(trkParameters_BACK) std::cout << " Use Propagator theta " << trkParameters_BACK->momentum().theta() << " init theta " <<  m_t_theta << std::endl;
//        }
        if(trkParameters_BACK) {
          ATH_MSG_DEBUG (" back extrapolation succeeded ");
          m_exitLayer = true;
          m_b_p      =  trkParameters_BACK->momentum().mag();
          m_b_eta    =  trkParameters_BACK->momentum().eta();
          m_b_theta  =  trkParameters_BACK->momentum().theta();
          m_b_phi    =  trkParameters_BACK->momentum().phi();
          m_b_x      =  trkParameters_BACK->position().x();
          m_b_y      =  trkParameters_BACK->position().y();
          m_b_z      =  trkParameters_BACK->position().z();
          if(fabs(m_m_p-m_b_p)>10.) ATH_MSG_DEBUG (" Back extrapolation to Muon Entry finds different momentum  difference MeV " << m_m_p-m_b_p);  
          delete trkParameters_BACK;
          extrapolationCache->reset();
          const std::vector<const Trk::TrackStateOnSurface*> *matvec_BACK = m_extrapolator->extrapolateM(*trkParameters_FW,*m_destinationSurface,Trk::oppositeMomentum,false,Trk::muon,extrapolationCache);
          double Eloss = 0.;
          double x0 = 0.;

          int mmat = 0;
          if (matvec_BACK && !matvec_BACK->empty()&& matvec_BACK->size()>0){    
            std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = matvec_BACK->begin();
            std::vector<const Trk::TrackStateOnSurface*>::const_iterator it_end = matvec_BACK->end();
            for ( ; it != it_end; ++it ) {
              const Trk::MaterialEffectsBase* matEf = (*it)->materialEffectsOnTrack();
              if( matEf ) {
                mmat++;
                if(m_trk_status[m_g4_steps] == 1000) ATH_MSG_DEBUG (" mmat " << mmat << " matEf->thicknessInX0() " << matEf->thicknessInX0() );
                x0 += matEf->thicknessInX0();
                const Trk::MaterialEffectsOnTrack* matEfs = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(matEf);
                double eloss0 = 0.;
                double meanIoni = 0.;
                double sigmaIoni = 0.;
                double meanRad = 0.;
                double sigmaRad = 0.;
                double sigmaTheta = 0.;
                double sigmaPhi = 0.;
                if(matEfs) {
                  const Trk::EnergyLoss* eLoss = (matEfs)->energyLoss();
                  if(eLoss) {
                    Eloss += eLoss->deltaE();
                    eloss0 = eLoss->deltaE();
                    meanIoni = eLoss->meanIoni();
                    sigmaIoni = eLoss->sigmaIoni();
                    meanRad = eLoss->meanRad();
                    sigmaRad = eLoss->sigmaRad();
                    if(m_trk_status[m_g4_steps] == 1000) ATH_MSG_DEBUG ( " mmat " << mmat << " eLoss->deltaE() "  << eLoss->deltaE() << "  eLoss->length() " << eLoss->length() );
                  }
                }
                const Trk::ScatteringAngles* scatAng = (matEfs)->scatteringAngles();
                if(scatAng) {
                   sigmaTheta = scatAng->sigmaDeltaTheta();
                   sigmaPhi = scatAng->sigmaDeltaPhi();
                }             
                if ( m_trk_scats < 500) {
// backwards
                  if(m_trk_status[m_g4_steps] == 1000) m_trk_sstatus[m_trk_scats] = -1000;
                  if((*it)->trackParameters()) {
                    m_trk_sx[m_trk_scats] = (*it)->trackParameters()->position().x();
                    m_trk_sy[m_trk_scats] = (*it)->trackParameters()->position().y();
                    m_trk_sz[m_trk_scats] = (*it)->trackParameters()->position().z();
                  }
                  m_trk_sx0[m_trk_scats] = matEf->thicknessInX0();   
                  m_trk_seloss[m_trk_scats] = eloss0;   
                  m_trk_smeanIoni[m_trk_scats] = meanIoni;   
                  m_trk_ssigIoni[m_trk_scats] = sigmaIoni;   
                  m_trk_smeanRad[m_trk_scats] = meanRad;   
                  m_trk_ssigRad[m_trk_scats] = sigmaRad;   
                  m_trk_ssigTheta[m_trk_scats] = sigmaTheta;   
                  m_trk_ssigPhi[m_trk_scats] = sigmaPhi;   
                  m_trk_scats++;
                }
              }
            }
          }
          m_b_X0         =  x0;
          m_b_Eloss      =  Eloss;
          delete matvec_BACK;
         }
        }
        delete trkParameters_FW;
      }
    }

    extrapolationCache->reset();
    const std::vector<const Trk::TrackStateOnSurface*> *matvec = m_extrapolator->extrapolateM(*m_parameterCache,destinationSurface,Trk::alongMomentum,false,Trk::muon,extrapolationCache);
    if(m_g4_stepsMS==0) matvec = m_extrapolator->extrapolateM(*m_parameterCacheCov,destinationSurface,Trk::alongMomentum,false,Trk::muon,extrapolationCache);

    if(m_g4_stepsMS==0) ATH_MSG_DEBUG(" G4 extrapolateM to Muon Entry " << " X0 " << extrapolationCache->x0tot() << " Eloss deltaE " <<   extrapolationCache->eloss()->deltaE()  << " Eloss sigma " << extrapolationCache->eloss()->sigmaDeltaE() << " meanIoni " << extrapolationCache->eloss()->meanIoni()  << " sigmaIoni " << extrapolationCache->eloss()->sigmaIoni() << " meanRad " <<  extrapolationCache->eloss()->meanRad() << " sigmaRad " << extrapolationCache->eloss()->sigmaRad());
//    if(m_trk_status[m_g4_steps] == 1000) matvec = m_extrapolator->extrapolateM(*m_parameterCache,destinationSurface,Trk::alongMomentum,false,Trk::muon);

//      modifyTSOSvector(const std::vector<const Trk::TrackStateOnSurface*> matvec, double scaleX0, double scaleEloss, bool reposition, bool aggregate, bool updateEloss, double caloEnergy, double caloEnergyError, double pCaloEntry, double momentumError, double & Eloss_tot);
    double Elosst = 0.;
    const std::vector<const Trk::TrackStateOnSurface*>  matvecNewRepAggrUp = modifyTSOSvector(*matvec, 1.0, 1.0, true, true, true, 0., 0., 10000., 0., Elosst);

    double X0Scale = 1.0;
    double ElossScale = 1.0;

    double Eloss0 = 0.;
    double Eloss1 = 0.;
    double Eloss5 = 0.;
    double Eloss10 = 0.;
    bool muonSystem = false;
    bool calorimeter = false;

    if(!matvec->empty()) { 
      if(m_crossedMuonEntry&&!m_exitLayer) calorimeter = true;
      if(m_crossedMuonEntry&&m_exitLayer)  muonSystem = true;
    }

    ATH_MSG_DEBUG ( " muonSystem " << muonSystem << " calorimeter " << calorimeter );
    if(muonSystem) {
//
// Muon sytem
//   
      m_elossupdator->getX0ElossScales(0, m_m_eta, m_m_phi, X0Scale, ElossScale );
      ATH_MSG_DEBUG ( " muonSystem scales X0 " << X0Scale << " ElossScale " << ElossScale);
      
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew1 = modifyTSOSvector(*matvec, X0Scale , 1., true, true, true, 0., 0., m_m_p, 0., Eloss1);
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew0 = modifyTSOSvector(*matvec, X0Scale , ElossScale, true, true, true, 0., 0., m_m_p, 0., Eloss0);
      ATH_MSG_DEBUG ( " muon system modify with 5 percent ");
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew5 = modifyTSOSvector(*matvec, X0Scale , ElossScale, true, true, true, 0., 0., m_m_p, 0.05*m_m_p, Eloss5);
      ATH_MSG_DEBUG ( " muon system modify with 10 percent ");
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew10 = modifyTSOSvector(*matvec, X0Scale , ElossScale, true, true, true, 0., 0., m_m_p, 0.10*m_m_p, Eloss10);

//      if(&matvecNew0!=0)  delete &matvecNew0;
//      if(&matvecNew5!=0)  delete &matvecNew5;
//      if(&matvecNew10!=0) delete &matvecNew10;
    }
    if(calorimeter) {
//
// Calorimeter  sytem
//    
      double phiCaloExit = atan2(m_m_y,m_m_x); 
      m_elossupdator->getX0ElossScales(1, m_t_eta, phiCaloExit , X0Scale, ElossScale );
      ATH_MSG_DEBUG ( " calorimeter scales X0 " << X0Scale << " ElossScale " << ElossScale);
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew1 = modifyTSOSvector(*matvec, X0Scale , 1., true, true, true, 0., 0., m_m_p, 0., Eloss1);
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew0 = modifyTSOSvector(*matvec, X0Scale , ElossScale, true, true, true, 0., 0., m_t_p, 0., Eloss0);
      if(fabs(Eloss1)>0) ATH_MSG_DEBUG ( " **** Cross Check calorimeter with Eloss Scale1 " <<  Eloss1 << " Eloss0 " << Eloss0 << " ratio " << Eloss0/Eloss1 );

      ATH_MSG_DEBUG ( " calorimeter modify with 5 percent ");
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew5 = modifyTSOSvector(*matvec, X0Scale , ElossScale, true, true, true, 0., 0., m_t_p, 0.05*m_m_p, Eloss5);
      ATH_MSG_DEBUG ( " calorimeter modify with 10 percent ");
      const std::vector<const Trk::TrackStateOnSurface*>  matvecNew10 = modifyTSOSvector(*matvec, X0Scale , ElossScale, true, true, true, 0., 0., m_t_p, 0.10*m_m_p, Eloss10);

//      if(&matvecNew0!=0)  delete &matvecNew0;
//      if(&matvecNew5!=0)  delete &matvecNew5;
//      if(&matvecNew10!=0) delete &matvecNew10;
    }

    ATH_MSG_DEBUG ( " status " << m_trk_status[m_g4_steps] << "Eloss1 " << Eloss1 << " Eloss0 " << Eloss0 << " Eloss5 " << Eloss5 << " Eloss10 " << Eloss10 );


    double Eloss = 0.;
    double x0 = 0.;

    int mmat = 0;
    //sroe: coverity 31541; matvec cannot be null here
    if (!(matvec->empty())){    
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = matvec->begin();
        std::vector<const Trk::TrackStateOnSurface*>::const_iterator it_end = matvec->end();
        for ( ; it != it_end; ++it ) {
          const Trk::MaterialEffectsBase* matEf = (*it)->materialEffectsOnTrack();
          if( matEf ) {
             mmat++;
             if(m_trk_status[m_g4_steps] == 1000) ATH_MSG_DEBUG (" mmat " << mmat << " matEf->thicknessInX0() " << matEf->thicknessInX0() );
             x0 += matEf->thicknessInX0();
             const Trk::MaterialEffectsOnTrack* matEfs = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(matEf);
             double eloss0 = 0.;
             double meanIoni = 0.;
             double sigmaIoni = 0.;
             double meanRad = 0.;
             double sigmaRad = 0.;
             double sigmaTheta = 0.;
             double sigmaPhi = 0.;
             if(matEfs) {
                  const Trk::EnergyLoss* eLoss = (matEfs)->energyLoss();
                  if(eLoss) {
                    Eloss += eLoss->deltaE();
                    eloss0 = eLoss->deltaE();
                    meanIoni = eLoss->meanIoni();
                    sigmaIoni = eLoss->sigmaIoni();
                    meanRad = eLoss->meanRad();
                    sigmaRad = eLoss->sigmaRad();
                    ATH_MSG_DEBUG ( "m_g4_stepsMS " << m_g4_stepsMS <<" mmat " << mmat << " X0 " << matEf->thicknessInX0() << " eLoss->deltaE() "  << eLoss->deltaE() << " meanIoni " << meanIoni << " Total Eloss " << Eloss << "  eLoss->length() " << eLoss->length() );
//                    if(m_trk_status[m_g4_steps] == 1000) ATH_MSG_DEBUG ( " mmat " << mmat << " eLoss->deltaE() "  << eLoss->deltaE() );
                  } 
             } 
             //sroe: coverity 31532
             const Trk::ScatteringAngles* scatAng = (matEfs)?( (matEfs)->scatteringAngles() ): (nullptr);
             
             if(scatAng) {
                sigmaTheta = scatAng->sigmaDeltaTheta();
                sigmaPhi = scatAng->sigmaDeltaPhi();
                ATH_MSG_DEBUG ( "m_g4_stepsMS " << m_g4_stepsMS <<" mmat " << mmat << " sigmaTheta " << sigmaTheta << " sigmaPhi " << sigmaPhi );
             }             

             if ( m_trk_scats < 500) {
               if( m_g4_stepsMS==0 || m_trk_status[m_g4_steps]==1000 )  {
// forwards
                 if(m_g4_stepsMS==0) m_trk_sstatus[m_trk_scats] = 10;
                 if(m_trk_status[m_g4_steps]==1000) m_trk_sstatus[m_trk_scats] = 1000;
                 if((*it)->trackParameters()) {
                   m_trk_sx[m_trk_scats] = (*it)->trackParameters()->position().x();
                   m_trk_sy[m_trk_scats] = (*it)->trackParameters()->position().y();
                   m_trk_sz[m_trk_scats] = (*it)->trackParameters()->position().z();
                 }
                 m_trk_sx0[m_trk_scats] = matEf->thicknessInX0();   
                 m_trk_seloss[m_trk_scats] = eloss0;   
                 m_trk_smeanIoni[m_trk_scats] = meanIoni;   
//                 std::cout << "  eloss0 " <<  eloss0 << " meanIoni " << meanIoni << std::endl;
                 m_trk_ssigIoni[m_trk_scats] = sigmaIoni;   
                 m_trk_smeanRad[m_trk_scats] = meanRad;   
                 m_trk_ssigRad[m_trk_scats] = sigmaRad;   
                 m_trk_ssigTheta[m_trk_scats] = sigmaTheta;   
                 m_trk_ssigPhi[m_trk_scats] = sigmaPhi;   
                 m_trk_scats++;
               }
             }  
          }
        }
        delete matvec;
    }

    ATH_MSG_DEBUG ("  m_g4_steps " << m_g4_steps << " Radius " << npos.perp() << " z " << npos.z() << " size matvec " << " total X0 " << x0 << " total Eloss " << Eloss );
//    std::cout << "  m_g4_steps " << m_g4_steps << " Radius " << npos.perp() << " z " << npos.z() << " size matvec " << " total X0 " << x0 << " total Eloss " << Eloss << std::endl;
    // fill the geant information and the trk information
    m_g4_p[m_g4_steps]       =  nmom.mag(); 
    m_g4_eta[m_g4_steps]     =  nmom.eta();   
    m_g4_theta[m_g4_steps]   =  nmom.theta();
    m_g4_phi[m_g4_steps]     =  nmom.phi();
    m_g4_x[m_g4_steps]       =  npos.x();
    m_g4_y[m_g4_steps]       =  npos.y();
    m_g4_z[m_g4_steps]       =  npos.z();
    m_g4_tX0[m_g4_steps]     = m_tX0Cache;
    m_g4_t[m_g4_steps]       = t;
    m_g4_X0[m_g4_steps]      = X0;
    
    m_trk_p[m_g4_steps]      = trkParameters ? trkParameters->momentum().mag()      : 0.;
    m_trk_eta[m_g4_steps]    = trkParameters ? trkParameters->momentum().eta()      : 0.;
    m_trk_theta[m_g4_steps]  = trkParameters ? trkParameters->momentum().theta()    : 0.;
    m_trk_phi[m_g4_steps]    = trkParameters ? trkParameters->momentum().phi()      : 0.;
    m_trk_x[m_g4_steps]      = trkParameters ? trkParameters->position().x()        : 0.;
    m_trk_y[m_g4_steps]      = trkParameters ? trkParameters->position().y()        : 0.;
    m_trk_z[m_g4_steps]      = trkParameters ? trkParameters->position().z()        : 0.;
    m_trk_lx[m_g4_steps]     = trkParameters ? trkParameters->parameters()[Trk::locX] : 0.;
    m_trk_ly[m_g4_steps]     = trkParameters ? trkParameters->parameters()[Trk::locY] : 0.;
    m_trk_eloss[m_g4_steps]  = Eloss;
    m_trk_eloss0[m_g4_steps] = Eloss0;
    m_trk_eloss1[m_g4_steps] = Eloss1;
    m_trk_eloss5[m_g4_steps] = Eloss5;
    m_trk_eloss10[m_g4_steps]= Eloss10;
    m_trk_scaleeloss[m_g4_steps]= ElossScale;
    m_trk_scalex0[m_g4_steps]   = X0Scale;
    m_trk_x0[m_g4_steps]     = x0;
    if(m_g4_stepsMS==0)  m_trk_status[m_g4_steps] =  10;

    double errord0 = 0.;
    double errorz0 = 0.;
    double errorphi = 0.;
    double errortheta = 0.;
    double errorqoverp = 0.;
    if(trkParameters&&trkParameters->covariance()) {
      errord0 = (*trkParameters->covariance())(Trk::d0,Trk::d0);
      errorz0 = (*trkParameters->covariance())(Trk::z0,Trk::z0);
      errorphi = (*trkParameters->covariance())(Trk::phi,Trk::phi);
      errortheta = (*trkParameters->covariance())(Trk::theta,Trk::theta);
      errorqoverp = (*trkParameters->covariance())(Trk::qOverP,Trk::qOverP);
      ATH_MSG_DEBUG (" Covariance found for m_trk_status "  << m_trk_status[m_g4_steps]);
      if( m_trk_status[m_g4_steps] == 10 ||  m_trk_status[m_g4_steps] == 1000 ) {
        double x00 = errortheta*1000000.;
// assume beta = 1 for check
        double sigPhi = sqrt(x0)*13.6*(1+0.038*log(x00))/m_trk_p[m_g4_steps]/sin(m_trk_theta[m_g4_steps]);
        double ratio = sqrt(errorphi)/sigPhi;
       std::cout << " m_trk_x " << m_trk_x[m_g4_steps] << "  m_trk_y " << m_trk_y[m_g4_steps] << " m_trk_z " << m_trk_z[m_g4_steps] << " cov 33 " << errortheta*1000000. << " cov22 " <<  errorphi*1000000000. << " ratio error in phi " << ratio << std::endl; 
       std::cout << " "  << std::endl;
      }
    }

    m_trk_erd0[m_g4_steps]  = sqrt(errord0); 
    m_trk_erz0[m_g4_steps]  = sqrt(errorz0); 
    m_trk_erphi[m_g4_steps]  = sqrt(errorphi); 
    m_trk_ertheta[m_g4_steps]  = sqrt(errortheta); 
    m_trk_erqoverp[m_g4_steps]  = sqrt(errorqoverp); 

// reset X0 at Muon Entry  
    if(m_g4_stepsMS==0)  m_tX0Cache   = 0.;
    // update the parameters if needed/configured
    if (m_extrapolateIncrementally && trkParameters) {
        delete m_parameterCache;
        m_parameterCache = trkParameters;
    }

//    std::cout << " eloss " << eloss << std::endl; 
//    std::cout << " extrapolationCache " << extrapolationCache << std::endl; 
//    std::cout << " g4Parameters " << g4Parameters << std::endl; 
//    std::cout << " trkParameters " << trkParameters << std::endl; 

    // delete cache and increment
    delete eloss;
    delete extrapolationCache;
    delete g4Parameters;
    delete trkParameters;
    ++m_g4_steps;
    if(m_g4_stepsMS!=-1)  ++m_g4_stepsMS; 
}
const std::vector<const Trk::TrackStateOnSurface*> Trk::GeantFollowerMSHelper::modifyTSOSvector(const std::vector<const Trk::TrackStateOnSurface*> matvec, double scaleX0, double scaleEloss, bool reposition, bool aggregate, bool updateEloss, double caloEnergy, double caloEnergyError, double pCaloEntry, double momentumError, double & Eloss_tot) const
{
//
// inputs: TSOSs for material (matvec) and scale factors for X0 (scaleX0) and Eloss (scaleEloss)   
//
// returns: new vector of TSOSs including scaling of X0 and Eloss; 
// 
// options:
// bool reposition    correct repositioning of the scattering centers in space 
// bool aggregate     put scattering centra together in two planes
// bool update Eloss  correct energy loss 1) including the measured calorimeter Eloss 2) include smearing of the muon momentum
//
// the routine should NOT be called for the ID
// for best use in the Calorimeter:      bool reposition = true, bool aggregate = true and updateEloss = true (measured caloEnergy and caloEnergyError should be passed) 
//                                       note that the updateEloss is only active with aggregate = true 
// for best use in the Muon Specrometer: bool reposition = true, bool aggregate = true and updateEloss = false 
//                                       
// if one runs with reposition = false the scattering centra are kept at the END of the thick/dense material: that is not right for thick material for thin it is OK                                       
//
   std::vector<const Trk::TrackStateOnSurface*> newTSOSvector;
   int maxsize = 2*matvec.size();
   if(aggregate) maxsize = 2;
   newTSOSvector.reserve(maxsize);
//
// initialize total sum variables 
//
//  
   Eloss_tot = 0.;

   double X0_tot = 0.; 

   double sigmaDeltaPhi2_tot = 0.; 
   double sigmaDeltaTheta2_tot = 0.; 
   double deltaE_tot = 0.;
   double sigmaDeltaE_tot = 0.;
   double sigmaPlusDeltaE_tot = 0.;
   double sigmaMinusDeltaE_tot = 0.;
   double deltaE_ioni_tot = 0.; 
   double sigmaDeltaE_ioni_tot=0.; 
   double deltaE_rad_tot = 0.; 
   double sigmaDeltaE_rad_tot =0.;

   const Trk::TrackStateOnSurface* mprevious = 0;
   const Trk::TrackStateOnSurface* mfirst = 0;
   const Trk::TrackStateOnSurface* mlast = 0;
   Amg::Vector3D posFirst(0.,0.,0.);
//   double distLast = 0.;
   double deltaEFirst = 0.;

   double deltaPhi = 0.; 
   double deltaTheta = 0.;

   double depth_tot = 0.;
   int n_tot = 0; 

   double w_tot = 0.; 
   double wdist2 = 0.;
   Amg::Vector3D wdir(0.,0.,0.);
   Amg::Vector3D wpos(0.,0.,0.);

   std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
   meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
   meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
// meotPattern.set(Trk::MaterialEffectsBase::FittedMaterialEffects);

   std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
   typePattern.set(Trk::TrackStateOnSurface::InertMaterial);
   typePattern.set(Trk::TrackStateOnSurface::Scatterer);

   std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternDeposit(0);
   typePatternDeposit.set(Trk::TrackStateOnSurface::InertMaterial);
   typePatternDeposit.set(Trk::TrackStateOnSurface::CaloDeposit);
   typePatternDeposit.set(Trk::TrackStateOnSurface::Scatterer);

   for(auto m : matvec) {

     if(!m->trackParameters()) {
       ATH_MSG_WARNING("No trackparameters on TrackStateOnSurface ");
       continue;
     }
     if(m->materialEffectsOnTrack()) {
       double X0 = m->materialEffectsOnTrack()->thicknessInX0();
       const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
       const Trk::EnergyLoss* energyLoss = 0;
       const Trk::ScatteringAngles* scat = 0;
       if(meot) {
         energyLoss = meot->energyLoss();
         if (energyLoss) {
//           double deltaE = energyLoss->deltaE();
         } else {
           ATH_MSG_WARNING("No energyLoss on TrackStateOnSurface ");
           continue;
         }
         scat = meot->scatteringAngles();
         if(scat) {
//           double dtheta = scat->sigmaDeltaTheta();
         } else {
           ATH_MSG_WARNING("No scatteringAngles on TrackStateOnSurface ");
           continue;
         }
      } else {
        ATH_MSG_WARNING("No materialEffectsOnTrack on TrackStateOnSurface ");
        continue;
      }


      double depth = energyLoss->length();
      ATH_MSG_DEBUG(" ");
      ATH_MSG_DEBUG(" original TSOS type " << m->dumpType() << " TSOS surface " << m->trackParameters()->associatedSurface() << " position x " << m->trackParameters()->position().x() << " y " << m->trackParameters()->position().y() << " z " << m->trackParameters()->position().z()   << " direction x " << m->trackParameters()->momentum().unit().x() << " y " << m->trackParameters()->momentum().unit().y() << " z " << m->trackParameters()->momentum().unit().z() << " p " << m->trackParameters()->momentum().mag() << " X0 " << X0 << " deltaE " << energyLoss->deltaE() << " sigma deltaTheta " <<  scat->sigmaDeltaTheta() << " depth " << depth );

      X0_tot +=scaleX0*X0;

      sigmaDeltaTheta2_tot += scaleX0*scat->sigmaDeltaTheta()*scat->sigmaDeltaTheta();
      sigmaDeltaPhi2_tot   += scaleX0*scat->sigmaDeltaPhi()*scat->sigmaDeltaPhi();

// Eloss sigma values add up linearly for Landau and exponential distributions       

      deltaE_tot           += scaleEloss*energyLoss->deltaE();
      sigmaDeltaE_tot      += scaleEloss*energyLoss->sigmaDeltaE();
      sigmaPlusDeltaE_tot  += scaleEloss*energyLoss->sigmaPlusDeltaE();
      sigmaMinusDeltaE_tot += scaleEloss*energyLoss->sigmaMinusDeltaE() ;
      deltaE_ioni_tot      += scaleEloss*energyLoss->meanIoni(); 
      sigmaDeltaE_ioni_tot += scaleEloss*energyLoss->sigmaIoni(); 
      deltaE_rad_tot       += scaleEloss*energyLoss->meanRad(); 
      sigmaDeltaE_rad_tot  += scaleEloss*energyLoss->sigmaRad();

      depth_tot += depth;
      n_tot++;

      Amg::Vector3D dir = m->trackParameters()->momentum().unit();
      Amg::Vector3D pos = m->trackParameters()->position();
      if(mprevious) {
             dir += mprevious->trackParameters()->momentum().unit();
      } 

      dir = dir/dir.mag();  
      ATH_MSG_DEBUG(" position at end " << pos.x() << " y " << pos.y() << " z " << pos.z() << " perp " << pos.perp() );
      ATH_MSG_DEBUG(" direction x " << dir.x() << " y " << dir.y() << " z " << dir.z());
      Amg::Vector3D pos0   = pos - (depth/2.+depth/sqrt(12.))*dir;
      Amg::Vector3D posNew = pos - (depth/2.-depth/sqrt(12.))*dir;

      ATH_MSG_DEBUG(" position scattering centre0 x " << pos0.x() << " y " << pos0.y() << " z " << pos0.z() << " perp " << pos0.perp() );
      ATH_MSG_DEBUG(" position scattering centre1 x " << posNew.x() << " y " << posNew.y() << " z " << posNew.z() << " perp " << posNew.perp() << " distance " << (pos0-posNew).mag() << " depth " << depth);  
      if(!mfirst) {
        mfirst = m;
        posFirst = pos0;
        deltaEFirst = energyLoss->deltaE(); 
      } 
      mlast = m;

      double w = scat->sigmaDeltaTheta()*scat->sigmaDeltaTheta();
      w_tot += w; 
      wpos += w*pos0/2.; 
      wpos += w*posNew/2.; 
      wdir += w*dir; 

      wdist2 += w*(pos0-posFirst).mag2()/2.;
      wdist2 += w*(posNew-posFirst).mag2()/2.;
//      distLast = (posNew-posFirst).mag();

      if (!aggregate&&!reposition) {

        const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot),sqrt(sigmaDeltaTheta2_tot));
        const Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaPlusDeltaE_tot, sigmaMinusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, depth ) ;
        const Trk::Surface& surf = *(meot->associatedSurface().clone());
        const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot, scatNew, energyLossNew, surf, meotPattern);
        const Trk::TrackParameters* pars = m->trackParameters()->clone();

// make new TSOS 
        const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface( 0, pars, 0, meotLast, typePattern );
        newTSOSvector.push_back(newTSOS);
        ATH_MSG_DEBUG(" NO aggregation and NO reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << X0_tot << " deltaE " << energyLossNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

        Eloss_tot += energyLossNew->deltaE();

        X0_tot = 0.;
        sigmaDeltaTheta2_tot = 0.;
        sigmaDeltaPhi2_tot = 0.;
        deltaE_tot = 0.;
        sigmaDeltaE_tot = 0;         
        sigmaPlusDeltaE_tot = 0.;
        sigmaMinusDeltaE_tot = 0.;
        deltaE_ioni_tot = 0.;
        sigmaDeltaE_ioni_tot = 0.;
        deltaE_rad_tot = 0.;
        sigmaDeltaE_rad_tot = 0.;


      } else if(!aggregate&&reposition) {

        if(fabs(depth)<10.) {

//        Thin scatterer: make only one TSOS 

          const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot),sqrt(sigmaDeltaTheta2_tot));
          const Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaPlusDeltaE_tot, sigmaMinusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, depth ) ;
          const Trk::Surface& surf = *(meot->associatedSurface().clone());
          const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot, scatNew, energyLossNew, surf, meotPattern);
          const Trk::TrackParameters* pars = m->trackParameters()->clone();
//        make new TSOS 
          const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface( 0, pars, 0, meotLast, typePattern );
          newTSOSvector.push_back(newTSOS);
          ATH_MSG_DEBUG(" NO aggregation and WITH reposition Thin Scatterer  TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << energyLossNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

          Eloss_tot += energyLossNew->deltaE();

          X0_tot = 0.;
          sigmaDeltaTheta2_tot = 0.;
          sigmaDeltaPhi2_tot = 0.;
          deltaE_tot = 0.;
          sigmaDeltaE_tot = 0;         
          sigmaPlusDeltaE_tot = 0.;
          sigmaMinusDeltaE_tot = 0.;
          deltaE_ioni_tot = 0.;
          sigmaDeltaE_ioni_tot = 0.;
          deltaE_rad_tot = 0.;
          sigmaDeltaE_rad_tot = 0.;

        } else { 
//
//        Thick scatterer: make two TSOSs 
//
//        prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss = 0 and scattering2 = total2 / 2. depth = 0
          const Trk::EnergyLoss* energyLoss0 = new EnergyLoss(0.,0.,0.,0.);
          const Trk::ScatteringAngles* scatFirst = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));

//        prepare for second MaterialEffectsOnTrack with X0 =  X0/2 Eloss = Eloss total and scattering2 = total2 / 2. depth = 0
          const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));
          const Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaPlusDeltaE_tot, sigmaMinusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.) ;
          double norm = dir.perp();          
//        Rotation matrix representation 
          Amg::Vector3D colx(-dir.y()/norm,        dir.x()/norm,           0);
          Amg::Vector3D coly(-dir.x()*dir.z()/norm, -dir.y()*dir.z()/norm, norm);
          Amg::Vector3D colz( dir.x(),             dir.y(),                dir.z());
        
          Amg::Transform3D* surfaceTransformFirst = new Amg::Transform3D(colx,coly,colz,pos0);
          Amg::Transform3D* surfaceTransformLast = new Amg::Transform3D(colx,coly,colz,posNew);
          Trk::PlaneSurface* surfFirst = new Trk::PlaneSurface( surfaceTransformFirst );
          Trk::PlaneSurface* surfLast = new Trk::PlaneSurface( surfaceTransformLast );
//        make MaterialEffectsOnTracks 
          const Trk::MaterialEffectsOnTrack*  meotFirst   =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatFirst,   energyLoss0,   *surfFirst,   meotPattern);
          const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatNew, energyLossNew, *surfLast, meotPattern);

//        calculate TrackParameters at first surface 
          double qOverP0 = m->trackParameters()->charge()/ (m->trackParameters()->momentum().mag()-fabs(energyLoss->deltaE())); 
          if(mprevious) qOverP0 = mprevious->trackParameters()->charge()/mprevious->trackParameters()->momentum().mag();
          const Trk::TrackParameters* parsFirst  = surfFirst->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverP0);
//        calculate TrackParameters at second surface 
          double qOverPNew = m->trackParameters()->charge()/m->trackParameters()->momentum().mag();
          const Trk::TrackParameters* parsLast  = surfLast->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverPNew);
// make TSOS 
//      
          const Trk::TrackStateOnSurface* newTSOSFirst = new Trk::TrackStateOnSurface( 0, parsFirst,     0, meotFirst,   typePattern );
          const Trk::TrackStateOnSurface* newTSOS  = new Trk::TrackStateOnSurface( 0, parsLast,   0, meotLast,   typePattern );

          ATH_MSG_DEBUG(" first NO aggregation and WITH reposition   TSOS type " << newTSOSFirst->dumpType() << " TSOS surface " << newTSOSFirst->trackParameters()->associatedSurface() << " position x " << newTSOSFirst->trackParameters()->position().x() << " y " << newTSOSFirst->trackParameters()->position().y() << " z " << newTSOSFirst->trackParameters()->position().z()   << " direction x " << newTSOSFirst->trackParameters()->momentum().unit().x() << " y " << newTSOSFirst->trackParameters()->momentum().unit().y() << " z " << newTSOSFirst->trackParameters()->momentum().unit().z() << " p " << newTSOSFirst->trackParameters()->momentum().mag() << " X0 " << meotFirst->thicknessInX0()  << " deltaE " << energyLoss0->deltaE() << " sigma deltaTheta " <<  scatFirst->sigmaDeltaTheta()  );
          ATH_MSG_DEBUG(" second NO aggregation and WITH reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << energyLossNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

          newTSOSvector.push_back(newTSOSFirst);
          newTSOSvector.push_back(newTSOS);
          Eloss_tot += energyLossNew->deltaE();

          X0_tot = 0.;
          sigmaDeltaTheta2_tot = 0.;
          sigmaDeltaPhi2_tot = 0.;
          deltaE_tot = 0.;
          sigmaDeltaE_tot = 0;         
          sigmaPlusDeltaE_tot = 0.;
          sigmaMinusDeltaE_tot = 0.;
          deltaE_ioni_tot = 0.;
          sigmaDeltaE_ioni_tot = 0.;
          deltaE_rad_tot = 0.;
          sigmaDeltaE_rad_tot = 0.;

        }
      }       

      mprevious = m;


     }
   } 
   if (aggregate&&reposition) {

        if (n_tot>0) {
//
//        Make three scattering planes in Calorimeter else make two 
//
          Amg::Vector3D pos = wpos/w_tot;
          bool threePlanes = false;
          if(X0_tot>50&&fabs(pos.z())<6700&&pos.perp()<4200) threePlanes = true; 
//
          const Trk::EnergyLoss* energyLoss0 = new EnergyLoss(0.,0.,0.,0.);
          const Trk::ScatteringAngles* scatFirst = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));

          const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));
          Trk::EnergyLoss* energyLoss2 = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaPlusDeltaE_tot, sigmaMinusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.) ;

          int elossFlag = 0; // return Flag for updateEnergyLoss Calorimeter energy (0 = not used) 
          const Trk::EnergyLoss* energyLossNew = (updateEloss ? m_elossupdator->updateEnergyLoss(energyLoss2, caloEnergy, caloEnergyError, pCaloEntry, momentumError, elossFlag):  new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaPlusDeltaE_tot, sigmaMinusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.)); 
          delete energyLoss2;

//          const Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaPlusDeltaE_tot, sigmaMinusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.);
 
//        direction of plane 
          Amg::Vector3D dir = wdir/w_tot; 
          dir = dir/dir.mag();  
          double norm = dir.perp();          
//        Rotation matrix representation 
          Amg::Vector3D colx(-dir.y()/norm,        dir.x()/norm,           0);
          Amg::Vector3D coly(-dir.x()*dir.z()/norm, -dir.y()*dir.z()/norm, norm);
          Amg::Vector3D colz( dir.x(),             dir.y(),                dir.z());
//        Centre position of the two planes
          double halflength2 = wdist2/w_tot - (pos-posFirst).mag()*(pos-posFirst).mag();
          double halflength = 0.;
          if(halflength2>0) halflength = sqrt(halflength2);
          Amg::Vector3D pos0   = pos - halflength*dir;
          Amg::Vector3D posNew = pos + halflength*dir;
          if(updateEloss) ATH_MSG_DEBUG("WITH updateEloss");
  
          ATH_MSG_DEBUG(" WITH aggregation and WITH reposition center planes x " << pos.x() << " y " << pos.y() << " z " << pos.z() << " halflength " << halflength << " w_tot " << w_tot << " X0_tot " << X0_tot ); 
 
          Amg::Transform3D* surfaceTransformFirst = new Amg::Transform3D(colx,coly,colz,pos0);
          Amg::Transform3D* surfaceTransformLast = new Amg::Transform3D(colx,coly,colz,posNew);
          Trk::PlaneSurface* surfFirst = new Trk::PlaneSurface( surfaceTransformFirst );
          Trk::PlaneSurface* surfLast = new Trk::PlaneSurface( surfaceTransformLast );
//        calculate TrackParameters at first surface 
          double qOverP0 = mfirst->trackParameters()->charge()/(mfirst->trackParameters()->momentum().mag()+fabs(deltaEFirst)); 
//        calculate TrackParameters at last surface 
          double qOverPNew = mlast->trackParameters()->charge()/mlast->trackParameters()->momentum().mag();
          const Trk::TrackParameters* parsFirst  = surfFirst->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverP0);
          const Trk::TrackParameters* parsLast  = surfLast->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverPNew);

          if(!threePlanes) {
//
// make two scattering planes and TSOS 
//
//          prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss = 0 and scattering2 = total2 / 2. depth = 0
            const Trk::MaterialEffectsOnTrack*  meotFirst   =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatFirst,   energyLoss0,   *surfFirst,   meotPattern);
//          prepare for second MaterialEffectsOnTrack with X0 =  X0/2 Eloss = Eloss total and scattering2 = total2 / 2. depth = 0
            const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatNew, energyLossNew, *surfLast, meotPattern);
//
//      
            const Trk::TrackStateOnSurface* newTSOSFirst = new Trk::TrackStateOnSurface( 0, parsFirst,     0, meotFirst,   typePattern );
            const Trk::TrackStateOnSurface* newTSOS  =  (elossFlag!=0 ?  new Trk::TrackStateOnSurface( 0, parsLast,   0, meotLast,   typePatternDeposit ) : new Trk::TrackStateOnSurface( 0, parsLast,   0, meotLast,   typePattern )) ;
            ATH_MSG_DEBUG(" first WITH aggregation and WITH reposition   TSOS type " << newTSOSFirst->dumpType() << " TSOS surface " << newTSOSFirst->trackParameters()->associatedSurface() << " position x " << newTSOSFirst->trackParameters()->position().x() << " y " << newTSOSFirst->trackParameters()->position().y() << " z " << newTSOSFirst->trackParameters()->position().z()   << " direction x " << newTSOSFirst->trackParameters()->momentum().unit().x() << " y " << newTSOSFirst->trackParameters()->momentum().unit().y() << " z " << newTSOSFirst->trackParameters()->momentum().unit().z() << " p " << newTSOSFirst->trackParameters()->momentum().mag() << " X0 " << meotFirst->thicknessInX0()  << " deltaE " << energyLoss0->deltaE() << " sigma deltaTheta " <<  scatFirst->sigmaDeltaTheta()  );
            ATH_MSG_DEBUG(" second WITH aggregation and WITH reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << energyLossNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );
            newTSOSvector.push_back(newTSOSFirst);
            newTSOSvector.push_back(newTSOS);
          } else {
//
// make three scattering planes and TSOS in Calorimeter 
//
            const Trk::ScatteringAngles* scatZero = new ScatteringAngles(0.,0.,0.,0.);
            Amg::Transform3D* surfaceTransform = new Amg::Transform3D(colx,coly,colz,pos);
            Trk::PlaneSurface* surf = new Trk::PlaneSurface( surfaceTransform );
            const Trk::TrackParameters* pars  = surf->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverPNew);
//        prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss = 0 and scattering2 = total2 / 2. depth = 0
            const Trk::MaterialEffectsOnTrack*  meotFirst   =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatFirst,   energyLoss0,   *surfFirst,   meotPattern);
//        prepare for middle MaterialEffectsOnTrack with X0 =  0 Eloss = ElossNew and scattering2 = 0. depth = 0
            const Trk::MaterialEffectsOnTrack*  meot =  new Trk::MaterialEffectsOnTrack(0., scatZero, energyLossNew, *surf, meotPattern);
//        prepare for last MaterialEffectsOnTrack with X0 =  X0/2 Eloss = 0 total and scattering2 = total2 / 2. depth = 0
            const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatNew, energyLoss0, *surfLast, meotPattern);
            const Trk::TrackStateOnSurface* newTSOSFirst = new Trk::TrackStateOnSurface( 0, parsFirst,     0, meotFirst,   typePattern );
            const Trk::TrackStateOnSurface* newTSOS  =    new Trk::TrackStateOnSurface( 0, pars,   0, meot,   typePatternDeposit );
            const Trk::TrackStateOnSurface* newTSOSLast = new Trk::TrackStateOnSurface( 0, parsLast,     0, meotLast,   typePattern );
            newTSOSvector.push_back(newTSOSFirst);
            newTSOSvector.push_back(newTSOS);
            newTSOSvector.push_back(newTSOSLast);
            ATH_MSG_DEBUG(" first WITH aggregation and WITH reposition   TSOS type " << newTSOSFirst->dumpType() << " TSOS surface " << newTSOSFirst->trackParameters()->associatedSurface() << " position x " << newTSOSFirst->trackParameters()->position().x() << " y " << newTSOSFirst->trackParameters()->position().y() << " z " << newTSOSFirst->trackParameters()->position().z()   << " direction x " << newTSOSFirst->trackParameters()->momentum().unit().x() << " y " << newTSOSFirst->trackParameters()->momentum().unit().y() << " z " << newTSOSFirst->trackParameters()->momentum().unit().z() << " p " << newTSOSFirst->trackParameters()->momentum().mag() << " X0 " << meotFirst->thicknessInX0()  << " deltaE " << energyLoss0->deltaE() << " sigma deltaTheta " <<  scatFirst->sigmaDeltaTheta()  );
            ATH_MSG_DEBUG(" second WITH aggregation and WITH reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << energyLossNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );
            ATH_MSG_DEBUG(" second WITH aggregation and WITH reposition   TSOS type " << newTSOSLast->dumpType() << " TSOS surface " << newTSOSLast->trackParameters()->associatedSurface() << " position x " << newTSOSLast->trackParameters()->position().x() << " y " << newTSOSLast->trackParameters()->position().y() << " z " << newTSOSLast->trackParameters()->position().z()   << " direction x " << newTSOSLast->trackParameters()->momentum().unit().x() << " y " << newTSOSLast->trackParameters()->momentum().unit().y() << " z " << newTSOSLast->trackParameters()->momentum().unit().z() << " p " << newTSOSLast->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << energyLossNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

          }

          Eloss_tot += energyLossNew->deltaE();
        }

   } 

   return newTSOSvector;
}
void Trk::GeantFollowerMSHelper::endEvent() const
{
    // fill the validation tree
    m_validationTree->Fill();
//    std::cout << "  endEvent m_parameterCache " << m_parameterCache << std::endl;
//    std::cout << " m_parameterCacheCov " << m_parameterCacheCov << std::endl; 
    delete m_parameterCache;
    delete m_parameterCacheCov;

//    std::cout << " GeantFollowerMSHelper::endEvent m_parameterCacheMS " << m_parameterCacheMS << std::endl;
//    std::cout << " GeantFollowerMSHelper::endEvent m_parameterCacheMSCov " << m_parameterCacheMSCov << std::endl;
    if(m_crossedMuonEntry) {
      if(m_parameterCacheMS) delete m_parameterCacheMS;
      if(m_parameterCacheMSCov) delete m_parameterCacheMSCov;
    }
}
      
      


