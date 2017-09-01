
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventUtils includes
#include "ParticleRemoverAlg.h"

#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"

#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"

// boost includes
#include <boost/algorithm/string/predicate.hpp>


ParticleRemoverAlg::ParticleRemoverAlg( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ),
  m_inCont(""),
  m_separator("___"),
  m_outCont(""),
  m_suffixes(),
  m_viewContNames(),
  m_resetViewConts(true),
  m_outPrefix(""),
  m_inContNameList(),
  m_outContNameList(),
  m_inContList(),
  m_outContList(),
  m_inViewContNameListList(),
  m_outViewContNameListList(),
  m_contType(UNKNOWN)
{
  //
  // Property declaration
  //
  declareProperty("Input",  m_inCont, "Input container name" );
  declareProperty("Output", m_outCont,
                  "The name of the output container with the deep copy of input objects" );

  declareProperty("Separator", m_separator,
                  "The string seperator between the output container name and the sytematic variation (default='___')" );

  declareProperty("Suffixes", m_suffixes,
                  "The names of all suffixes for the input and output container names" );

  declareProperty("SelectedViewContainers", m_viewContNames,
                  "The names of all view containers that contain particles that we want to retain" );

  declareProperty("RemapViewContainers", m_resetViewConts,
                  "Boolean to decide if the existing view containers should be re-mapped (default: true)" );

  declareProperty("OutputViewContainerPrefix", m_outPrefix,
                  "Prefix to be used for all created output view containers" );
}


ParticleRemoverAlg::~ParticleRemoverAlg() {}


StatusCode ParticleRemoverAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print the configuration to the log file
  ATH_MSG_DEBUG( "Using: " << m_inCont );
  ATH_MSG_DEBUG( "Using: " << m_outCont );
  ATH_MSG_DEBUG( "Using: " << m_separator );
  ATH_MSG_DEBUG( "Using: " << m_suffixes );
  ATH_MSG_DEBUG( "Using: " << m_viewContNames );
  ATH_MSG_DEBUG( "Using: " << m_resetViewConts );
  ATH_MSG_DEBUG( "Using: " << m_outPrefix );

  // Perform some sanity checks on the given container names
  if ( m_inCont.value().empty() || m_outCont.value().empty() || m_viewContNames.value().empty() ) {
    ATH_MSG_ERROR("Wrong user setup! You need to give a valid name for both the Input, Output, and SelectedViewContainers!");
    return StatusCode::FAILURE;
  }

  // Abort on an unchecked systematics code
  // CP::SystematicCode::enableFailure();

  // Build the vector of all input and output container names
  const std::size_t totSize = 1 + m_suffixes.value().size(); // the '1' comes from the InputContainer
  m_inContNameList.resize(totSize);
  m_inContList.resize(totSize);
  m_outContNameList.resize(totSize);
  m_inContNameList[0]  = m_inCont.value();
  m_outContNameList[0] = m_outCont.value();
  for ( std::size_t i=1; i<totSize; ++i ) {
    const std::string& currentSuffix = m_suffixes.value()[i-1];
    ATH_MSG_VERBOSE("Using current suffix " << currentSuffix << " to search for matching containers");
    if (boost::starts_with( currentSuffix, m_separator.value() )) {
      m_inContNameList[i]  = m_inCont.value()  + currentSuffix;
      m_outContNameList[i] = m_outCont.value() + currentSuffix;
    }
    else {
      m_inContNameList[i]  = m_inCont.value()  + m_separator.value() + currentSuffix;
      m_outContNameList[i] = m_outCont.value() + m_separator.value() + currentSuffix;
    }
  }
  // Print out the matches that we found
  if ( msgLvl(MSG::VERBOSE) ) {
    for ( std::size_t i=0; i<m_inContNameList.size(); ++i ){
      ATH_MSG_VERBOSE("Matched input number " << i << " with input name " << m_inContNameList[i] << " to output name " << m_outContNameList[i]);
    }
  }



  // Now, also try to map all the view container names to the input (shallow
  // copy) containers. Set up with that the correct mapping of the new output
  // view container names.
  // Assume that all names where we have a suffix, but where we cannot match it
  // to a suffix of the input container, it belongs to the origninal one.
  if ( m_resetViewConts.value() || !(m_outPrefix.value().empty()) ){
    ATH_MSG_VERBOSE("Going to iterate over " << totSize << " elements");
    m_inViewContNameListList.reserve(totSize);
    m_outViewContNameListList.reserve(totSize);
    for ( std::size_t i=0; i<totSize; ++i ) {
      ATH_MSG_VERBOSE("At " << i << "-th element");
      if (i==0){
        // This is the master container without any "___" in its name
        std::vector<std::string> inViewNames;
        std::vector<std::string> outViewNames;
        for ( const std::string& inViewName : m_viewContNames.value() ){
          ATH_MSG_VERBOSE("Looking at input view container name: " << inViewName);
          std::size_t pos = inViewName.find(m_separator.value());
          if ( pos == std::string::npos ){ // the separator is not found
            ATH_MSG_VERBOSE("No seperator found");
            inViewNames.push_back(inViewName);
            outViewNames.push_back( m_outPrefix.value() + inViewName );
            ATH_MSG_VERBOSE("Added input name " << inViewNames.back() << " and output name " << outViewNames.back());
          }
          else {
            pos += m_separator.value().length();
            const std::string foundSuffix = inViewName.substr(pos, std::string::npos);
            ATH_MSG_VERBOSE("Seperator found and suffix found: " << foundSuffix);
            // the separator is found, but the found suffix doesn't match any of the provided ones
            if ( std::find( m_suffixes.value().begin(), m_suffixes.value().end(), foundSuffix) == m_suffixes.value().end() ){
              inViewNames.push_back(inViewName);
              outViewNames.push_back( m_outPrefix.value() + inViewName );
              ATH_MSG_VERBOSE("Added2 input name " << inViewNames.back() << " and output name " << outViewNames.back());
            }
          }
        }
        m_inViewContNameListList.push_back(inViewNames);
        m_outViewContNameListList.push_back(outViewNames);
      }
      else {
        const std::string& currentSuffix = m_suffixes.value()[i-1];
        ATH_MSG_VERBOSE("Looking at current suffix: " << currentSuffix);
        std::vector<std::string> inViewNames;
        std::vector<std::string> outViewNames;
        for ( const std::string& inViewName : m_viewContNames.value() ){
          ATH_MSG_VERBOSE("Looking at current input view container name: " << inViewName);
          if ( inViewName.find(m_separator.value()+currentSuffix) != std::string::npos ){ // the suffix is found
            inViewNames.push_back(inViewName);
            outViewNames.push_back( m_outPrefix.value() + inViewName );
            ATH_MSG_VERBOSE("Added3 input name " << inViewNames.back() << " and output name " << outViewNames.back());
          }
        }
        m_inViewContNameListList.push_back(inViewNames);
        m_outViewContNameListList.push_back(outViewNames);
      }
    } // End: loop over all containers
  }
  // Some sanity printouts
  if ( msgLvl(MSG::VERBOSE) ) {
    ATH_MSG_VERBOSE("Printing final input and output names...");
    for ( std::size_t i=0; i<m_inViewContNameListList.size(); ++i ){
      ATH_MSG_VERBOSE("    At i=" << i << "-th element");
      const std::vector<std::string>& inViewNameList  = m_inViewContNameListList[i];
      const std::vector<std::string>& outViewNameList = m_outViewContNameListList[i];
      ATH_MSG_VERBOSE("    Have " << inViewNameList.size() << " in view elements and " << outViewNameList.size() << " out view elements");
      for ( std::size_t j=0; j<inViewNameList.size(); ++j ){
        ATH_MSG_VERBOSE("        At j=" << j << "-th element");
        const std::string& inName  = inViewNameList[j];
        const std::string& outName = outViewNameList[j];
        ATH_MSG_VERBOSE("        Have input name " << inName << " paired with out name " << outName);
      }
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode ParticleRemoverAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}



StatusCode ParticleRemoverAlg::execute()
{
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  // Let's first clear some stuff
  m_inContList.clear();
  m_inContList.resize(m_inContNameList.size());
  m_outContList.clear();

  // Figure out what type the input container has, if we didn't do it yet
  if ( m_contType == UNKNOWN ){
    if ( evtStore()->contains<xAOD::PhotonContainer>(m_inCont.value()) ){ m_contType = PHOTON; }
    else if ( evtStore()->contains<xAOD::ElectronContainer>(m_inCont.value()) ){ m_contType = ELECTRON; }
    else if ( evtStore()->contains<xAOD::MuonContainer>(m_inCont.value()) ){ m_contType = MUON; }
    else if ( evtStore()->contains<xAOD::TauJetContainer>(m_inCont.value()) ){ m_contType = TAU; }
    else if ( evtStore()->contains<xAOD::JetContainer>(m_inCont.value()) ){ m_contType = JET; }
    else if ( evtStore()->contains<xAOD::TruthParticleContainer>(m_inCont.value()) ){ m_contType = TRUTHPARTICLE; }
    else if ( evtStore()->contains<xAOD::CompositeParticleContainer>(m_inCont.value()) ){ m_contType = COMPOSITEPARTICLE; }
    else if ( evtStore()->contains<xAOD::PFOContainer>(m_inCont.value()) ){ m_contType = PARITCLEFLOW; }
    else if ( evtStore()->contains<xAOD::NeutralParticleContainer>(m_inCont.value()) ){ m_contType = NEUTRALPARTICLE; }
    else if ( evtStore()->contains<xAOD::TrackParticleContainer>(m_inCont.value()) ){ m_contType = TRACKPARTICLE; }
    else if ( evtStore()->contains<xAOD::ParticleContainer>(m_inCont.value()) ){ m_contType = PARTICLE; }
    else if ( evtStore()->contains<xAOD::CaloClusterContainer>(m_inCont.value()) ){ m_contType = CALOCLUSTER; }
  }
  if ( m_contType == UNKNOWN ){
    ATH_MSG_FATAL("We couldn't determine the type of the container... abort!");
    return StatusCode::FAILURE;
  }

  // Open the input container
  for ( std::size_t i=0; i<m_inContNameList.size(); ++i ) {
    ATH_CHECK( evtStore()->retrieve( m_inContList[i], m_inContNameList.at(i) ));
  }

  // Make a quick check that all input containers have the same size
  const std::size_t inContSize = m_inContList[0]->size();
  for ( const xAOD::IParticleContainer* inCont : m_inContList ){
    if ( inContSize != inCont->size() ){
      ATH_MSG_FATAL("The input container and its shallow copies don't have the same size! Aborting...");
      return StatusCode::FAILURE;
    }
  }

  // Create a vector of bools with the same size as the input container. This
  // will be used to say if we want to keep that particular object.
  // All entries will be initialized to false.
  std::vector<bool> keepParticleVec (inContSize, false);
  // Now, loop over all view containers and flag the particles that we want to
  // keep with true in the above vector of bools.
  for ( const std::string& viewContName : m_viewContNames.value() ){
    const xAOD::IParticleContainer* inViewCont = nullptr;
    ATH_CHECK( evtStore()->retrieve( inViewCont, viewContName ) );
    // Make a quick check that the provided view containers are not larger
    if ( inViewCont->size() > inContSize ){
      ATH_MSG_FATAL("One of the input view containers is larger than the input container... aborting.");
      return StatusCode::FAILURE;
    }
    for ( const xAOD::IParticle* part : *inViewCont ){
      const std::size_t idx = part->index();
      keepParticleVec[idx] = true;
    }
  }

  // Do the heavy lifting of actually creating the new and reduced output container(s)
  if ( m_contType == PHOTON ){
    ATH_CHECK( this->removeParticles<xAOD::PhotonContainer>(keepParticleVec) );
  }
  else if ( m_contType == ELECTRON ){
    ATH_CHECK( this->removeParticles<xAOD::ElectronContainer>(keepParticleVec) );
  }
  else if ( m_contType == MUON ){
    ATH_CHECK( this->removeParticles<xAOD::MuonContainer>(keepParticleVec) );
  }
  else if ( m_contType == TAU ){
    ATH_CHECK( this->removeParticles<xAOD::TauJetContainer>(keepParticleVec) );
  }
  else if ( m_contType == JET ){
    ATH_CHECK( this->removeParticles<xAOD::JetContainer>(keepParticleVec) );
  }
  else if ( m_contType == TRUTHPARTICLE ){
    ATH_CHECK( this->removeParticles<xAOD::TruthParticleContainer>(keepParticleVec) );
  }
  else if ( m_contType == COMPOSITEPARTICLE ){
    ATH_CHECK( this->removeParticles<xAOD::CompositeParticleContainer>(keepParticleVec) );
  }
  else if ( m_contType == PARITCLEFLOW ){
    ATH_CHECK( this->removeParticles<xAOD::PFOContainer>(keepParticleVec) );
  }
  else if ( m_contType == NEUTRALPARTICLE ){
    ATH_CHECK( this->removeParticles<xAOD::NeutralParticleContainer>(keepParticleVec) );
  }
  else if ( m_contType == TRACKPARTICLE ){
    ATH_CHECK( this->removeParticles<xAOD::TrackParticleContainer>(keepParticleVec) );
  }
  else if ( m_contType == PARTICLE ){
    ATH_CHECK( this->removeParticles<xAOD::ParticleContainer>(keepParticleVec) );
  }
  else if ( m_contType == CALOCLUSTER ){
    ATH_CHECK( this->removeParticles<xAOD::CaloClusterContainer>(keepParticleVec) );
  }

  return StatusCode::SUCCESS;
}
