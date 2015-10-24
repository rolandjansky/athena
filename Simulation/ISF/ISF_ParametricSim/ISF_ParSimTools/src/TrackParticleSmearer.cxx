/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleSmearer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_ParSimTools/TrackParticleSmearer.h"
// ISF
#include "ISF_Event/ISFParticle.h"
// Trk
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "TrkParameters/TrackParameters.h"


//================ Constructor ====================================================
iParSim::TrackParticleSmearer::TrackParticleSmearer(const std::string& t, const std::string& n, const IInterface*  p ) :
  AthAlgTool(t,n,p),
  m_incidentSvc("IncidentSvc", n),
  m_tpContainerName("SmearedTrackParticles"),
  m_extrapolationEngine(""),
  m_ISPtoPerigeeTool("")
{
    declareInterface<ISF::IParticleProcessor>(this);
    // The collection to be written out
    declareProperty("TrackParticleCollection"  ,      m_tpContainerName);
    // The Tools for the smearers
    declareProperty("ChargedSmearers"          ,      m_chargedSmearers);
    // extrapolation engine
    declareProperty("ExtrapolationEngine"      ,      m_extrapolationEngine);
    // ISP to Perigee Parameters tool
    declareProperty("ISPtoPerigeeTool"         ,      m_ISPtoPerigeeTool);

}

//================ Destructor =====================================================

iParSim::TrackParticleSmearer::~TrackParticleSmearer()
{}

//================ Initialisation =================================================
StatusCode iParSim::TrackParticleSmearer::initialize()
{
    ATH_MSG_DEBUG("::: TrackParticleSmearer ::: initialize()");

    // Random number service
    //if ( m_randomSvc.retrieve().isFailure() ) {
    //    ATH_MSG_ERROR( "Could not retrieve " << m_randomSvc );
    //    return StatusCode::FAILURE;
    //}
    //Get own engine with own seeds:
    //m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
    //if (!m_randomEngine) {
    //    ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
    //    return StatusCode::FAILURE;
    //}
    
    // Athena/Gaudi framework
    if (m_incidentSvc.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_incidentSvc << ". Exiting.");
        return StatusCode::FAILURE;
    }
    // register to the incident service: BeginEvent for TrackCollection
    m_incidentSvc->addListener( this, IncidentType::BeginEvent);
    
    // retrieve the smearers (DefaultSmearer, MuonSmearer, ...)
    if (m_chargedSmearers.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_chargedSmearers << ". Exiting.");
        return StatusCode::FAILURE; 
    } else {
        // reorder the smearers
        // screen output
        ATH_MSG_INFO("Retrieved in total "  << m_chargedSmearers.size() << " smearers.");
        for (auto& smearer: m_chargedSmearers){
            // get the pointer to the smearer
            const iParSim::IChargedSmearer* ics = &*smearer;
            m_chargedSmearerMap[ics->pdg()] = ics;
            ATH_MSG_VERBOSE("- retrieved "  << ics->name() << " for pdg = " << ics->pdg() <<  " as a " 
                                            << (ics->pdg() ?  "specific smearer." : "default smearer.") );
        }
     }

    // ISP to Perigee Parameters tool
    if (m_ISPtoPerigeeTool.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_ISPtoPerigeeTool << ". Exiting.");
        return StatusCode::FAILURE;
    }

    // retrieve the extrapolation engine
    /* not for now...
    if (m_extrapolationEngine.retrieve().isFailure()){
       ATH_MSG_WARNING("Could not retrieve the ExtrapolationEngine " << m_extrapolationEngine);
       return StatusCode::FAILURE;
    }
    */

    ATH_MSG_INFO( "initialize() successful." );
    return StatusCode::SUCCESS;
}

/** Creates a new ISFParticle from a given ParticleState, universal transport tool */
ISF::ISFParticle* iParSim::TrackParticleSmearer::process(const ISF::ISFParticle& isp) {

  ATH_MSG_VERBOSE("::: TrackParticleSmearer ::: Processing " << isp << " particle");

  const unsigned int ispPdgCode = std::abs(isp.pdgCode());
  if (ispPdgCode != 13) {
    ATH_MSG_DEBUG("ISF Particle is not a muon. Skipping this particle...");
    return 0;
  }
  const iParSim::IChargedSmearer* ics = ( m_chargedSmearerMap.find(ispPdgCode) == m_chargedSmearerMap.end() )
                                      ?   m_chargedSmearerMap[0]
                                      :   m_chargedSmearerMap[ispPdgCode];
  if (ics->pdg() == 0) ATH_MSG_DEBUG("     No existing smearer for ISF particle. DefaultSmearer will be used.");

  // create a new xAOD::TrackParticle and push it into the container
  xAOD::TrackParticle* xaodTP = new xAOD::TrackParticle;
  m_trackParticleContainer->push_back(xaodTP);

  // Get the track parameters from the ISF particle
  const Trk::TrackParameters* tP = m_ISPtoPerigeeTool->extractTrkParameters(isp);
  if (tP==0) {
    ATH_MSG_WARNING("Trk::Extrapolator failed to extrapolate the cParameters. Will not smear this particle.");
    return 0;
    }

  // Defining parameters of the Truth Particle (they need to be smeared later)
  xaodTP->setDefiningParameters(
        tP->parameters()[Trk::d0] , tP->parameters()[Trk::z0],
        tP->parameters()[Trk::phi], tP->parameters()[Trk::theta],
        tP->parameters()[Trk::qOverP]                                 
  );

  // Origin of the parameters (Beam Spot) (vx, vy, vz)
  const Amg::Vector3D Origin( m_ISPtoPerigeeTool->getPerigee() );
  xaodTP->setParametersOrigin(Origin[0], Origin[1], Origin[2]);

  if ( !ics->smear(xaodTP) ) {
    ATH_MSG_WARNING("Track Particle was not smeared! Deleting particle.");
    m_trackParticleContainer->pop_back();
  }

  //ATH_MSG_VERBOSE("     Track Parameters Covariance Matrix: "     << "\n" << xaodTP->definingParametersCovMatrix());
  
  /* EXTRAPOLATION ... not implemented for now
  ===========================================================================================================================
  Trk::CurvilinearParameters csp(isp.position(), isp.momentum(), isp.charge()); // needs to be updated to smeared parameters
  // the extrapolation cell with it
  Trk::ExtrapolationCell<Trk::TrackParameters> ecc(csp);
  ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Trk::ExtrapolationMode::Direct);
  // and extrapolate to the Perigee surface (at 0,0,0 for the moment, may be changed to BS )
  Trk::ExtrapolationCode eCode = m_extrapolationEngine->extrapolate(ecc);
  if (eCode.isSuccess()){
      // code to be filled -> use particle creator 
  } else {
      // take this as an inefficiency
  }
  ===========================================================================================================================*/

  return 0; // Will return an extrapolated smeared ISF particle someday
}

StatusCode iParSim::TrackParticleSmearer::finalize()
{ 
    ATH_MSG_INFO( "finalize() successful " );
    return StatusCode::SUCCESS;
}

void iParSim::TrackParticleSmearer::handle( const Incident& inc ) {
  // check the incident type 
  if ( inc.type() == IncidentType::BeginEvent ){
    // create a new xAOD::TrackParticle collection
    ATH_MSG_VERBOSE("::: TrackParticleSmearer ::: Creating TrackParticleCollection " << m_tpContainerName);
    m_trackParticleContainer = new xAOD::TrackParticleContainer; 
    if ( (evtStore()->record(m_trackParticleContainer, m_tpContainerName, true)).isFailure() ) {
        ATH_MSG_ERROR( "Unable to record TrackParticleCollection " << m_tpContainerName);
        delete m_trackParticleContainer; m_trackParticleContainer=0;
    }
    // create a new xAOD::TrackParticle AUX collection
    ATH_MSG_VERBOSE("::: TrackParticleSmearer ::: Creating TrackParticleAuxCollection " << m_tpContainerName +"Aux.");
    m_trackParticleAuxContainer = new xAOD::TrackParticleAuxContainer; 
    if ( (evtStore()->record(m_trackParticleAuxContainer, m_tpContainerName + "Aux.", true)).isFailure() ) {
        ATH_MSG_ERROR( "Unable to record TrackParticleAuxCollection " << m_tpContainerName + "Aux.");
        delete m_trackParticleAuxContainer; m_trackParticleAuxContainer=0;
    }
    else { 
        m_trackParticleContainer->setStore( m_trackParticleAuxContainer ); 
    }

  }
  return;
}