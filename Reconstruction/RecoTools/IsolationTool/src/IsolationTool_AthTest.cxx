/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IsolationTool includes
#include "IsolationTool_AthTest.h"
// #include "xAODTracking/TrackingPrimitives.h"
// #include "xAODTracking/TrackParticleContainer.h"

static const int NDIFF = xAOD::Iso::ptvarcone20 - xAOD::Iso::ptcone20;

IsolationTool_AthTest::IsolationTool_AthTest( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ),
m_trackIsolationTool(),
m_caloIsolationTool(),
m_decorators(xAOD::Iso::numIsolationTypes, nullptr)
{

  //declareProperty( "Property", m_nProperty ); //example property declaration
declareProperty("TrackIsolationTool", m_trackIsolationTool);
declareProperty("CaloIsolationTool",  m_caloIsolationTool);
declareProperty("TargetContainer",    m_containerName = "InDetTrackParticles");
declareProperty("ptcones",            m_ptcones);
declareProperty("topoetcones",        m_topoetcones);
declareProperty("Prefix",             m_prefix="");
}


IsolationTool_AthTest::~IsolationTool_AthTest() {}


StatusCode IsolationTool_AthTest::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // load the matching tool
  if( ! m_caloIsolationTool.empty() ) {
     CHECK( m_caloIsolationTool.retrieve() );
     ATH_MSG_INFO( "Successfully retrived the CaloIsolationTool!" );
  }

  if( ! m_trackIsolationTool.empty() ) {
     CHECK( m_trackIsolationTool.retrieve() );
     ATH_MSG_INFO( "Successfully retrived the CaloIsolationTool!" );
  }

  m_trkCorrList.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
  m_caloCorrList.calobitset.set(static_cast<unsigned int>(xAOD::Iso::coreCone));
  m_caloCorrList.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));

  /// create decorator list
  m_ptconeTypes.clear();
  for(auto c: m_ptcones){
    xAOD::Iso::IsolationType t = static_cast<xAOD::Iso::IsolationType>(c);
    m_decorators[c] = new SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toCString(t));
    m_decorators[c+NDIFF] = new SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toCString(static_cast<xAOD::Iso::IsolationType>(c+NDIFF)));
    m_ptconeTypes.push_back(t);
  }
  m_topoetconeTypes.clear();
  for(auto c: m_topoetcones) {
    xAOD::Iso::IsolationType t = static_cast<xAOD::Iso::IsolationType>(c);
    m_decorators[c] = new SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toCString(t));
    m_topoetconeTypes.push_back(t);
  }

  return StatusCode::SUCCESS;
}

StatusCode IsolationTool_AthTest::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  for(auto d: m_decorators) {if(d) delete d;}

  return StatusCode::SUCCESS;
}

StatusCode IsolationTool_AthTest::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // retrieve tag (muon) container
   const xAOD::IParticleContainer* toDecorate = nullptr;
   if(evtStore()->retrieve(toDecorate, m_containerName).isFailure()) {
     ATH_MSG_FATAL( "Unable to retrieve " << m_containerName );
     return StatusCode::FAILURE;
   }

  /// Loop over tracks
  for(auto particle : *toDecorate) {

    /// track isolation
    xAOD::TrackIsolation resultTrack;
    if (m_trackIsolationTool->trackIsolation(resultTrack, *particle, m_ptconeTypes, m_trkCorrList)){
        for(unsigned int i=0; i<m_ptcones.size(); i++){
          (*(m_decorators[m_ptcones[i]]))(*particle) = resultTrack.ptcones[i];
          (*(m_decorators[m_ptcones[i]+NDIFF]))(*particle) = resultTrack.ptvarcones_10GeVDivPt[i];
          ATH_MSG_DEBUG("ptcone" << i << " " << resultTrack.ptcones[i] << " & " << resultTrack.ptvarcones_10GeVDivPt[i]);
        }
    }else{
        ATH_MSG_WARNING("Failed to apply the track isolation for a particle");
    }

    /// calo isolation
    xAOD::CaloIsolation resultCalo;
    if (m_caloIsolationTool->caloTopoClusterIsolation(resultCalo, *particle, m_topoetconeTypes, m_caloCorrList)){
        for(unsigned int i=0; i<m_topoetcones.size(); i++){
          (*(m_decorators[m_topoetcones[i]]))(*particle) = resultCalo.etcones[i];
          ATH_MSG_DEBUG("topoetcone" << i << " " << resultCalo.etcones[i]);
        }
    }else {
        ATH_MSG_WARNING("Failed to apply the calo isolation for a particle");
    }
  } 

  return StatusCode::SUCCESS;
}
