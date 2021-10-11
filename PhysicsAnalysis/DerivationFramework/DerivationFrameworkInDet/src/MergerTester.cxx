/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   TrackParticleMerger tester
//   Author: Bingxuan Liu, bingxuan.liu@cern.ch
//   This is a modified version of exisitng TrackCollectionMerger
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Track Particle merger to be used downstream, mainly for 
//          combining LRT and standard tracks
//          This is a tester demo to retrive the combined track particles
//////////////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/MergerTester.h"
#include "GaudiKernel/MsgStream.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace DerivationFramework {

  MergerTester::MergerTester(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    // The default goal of this merger is to create a collection combining standard and LRT tracks
    m_inputParticleLocation = "InDetWithLRTTrackParticles"    ;
    declareProperty("InputTrack",         m_inputParticleLocation); 
 }

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////

  StatusCode MergerTester::initialize()
  {
    ATH_MSG_DEBUG("Initializing MergerTester");
    ATH_CHECK( m_inputParticleLocation.initialize() );
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute
  ///////////////////////////////////////////////////////////////////
  StatusCode MergerTester::execute()
  {
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////

  StatusCode MergerTester::finalize()
  {
     return StatusCode::SUCCESS;
  }

 
  StatusCode MergerTester::addBranches() const
  {
    // Get the combined track particle and print some info
    // use this block to test the necessary functionalities in your downstream workflow
    SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCol (m_inputParticleLocation);
    ATH_MSG_INFO("Tester Info: ");
    ATH_MSG_INFO(trackParticleCol->size());  
    for (const auto *const rf: *trackParticleCol) {
      ATH_MSG_INFO(rf->d0());
      ATH_MSG_INFO(rf->auxdata<float>("truthMatchProbability"));
    } 
    return StatusCode::SUCCESS;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Merge track collections and remove duplicates
  ///////////////////////////////////////////////////////////////////
}

