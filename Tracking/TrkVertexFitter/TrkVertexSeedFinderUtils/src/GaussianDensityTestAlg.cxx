///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GaussianDensityTestAlg.cxx 
// Implementation file for class GaussianDensityTestAlg
// Author: Dave Casper <dcasper@uci.edu>
/////////////////////////////////////////////////////////////////// 

// TrkVertexSeedFinderUtils includes
#include "GaussianDensityTestAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

#include "xAODTracking/Vertex.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

#include <limits>

namespace Trk
{
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
GaussianDensityTestAlg::GaussianDensityTestAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_useBeamConstraint(true),
  m_iBeamCondSvc("BeamCondSvc", name),
  m_iTHistSvc("THistSvc", name)
{
  //
  // Property declaration
  // 
}

// Destructor
///////////////
GaussianDensityTestAlg::~GaussianDensityTestAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode GaussianDensityTestAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_trackParticlesKey.initialize() );

  ATH_CHECK( m_estimator.retrieve() );
  ATH_CHECK( m_trackFilter.retrieve() );

  // setup histograms/trees
  m_h_density = new TH1F("Density", "Density", 800, -200.0, 200.0);

  CHECK( m_iTHistSvc->regHist("/file1/h/density", m_h_density) );

  return StatusCode::SUCCESS;
}

StatusCode GaussianDensityTestAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode GaussianDensityTestAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadHandle<xAOD::TrackParticleContainer> trackParticles(m_trackParticlesKey);

  std::vector<Trk::ITrackLink*> trackVector;
  selectTracks(trackParticles.cptr(), trackVector);

  std::vector<const Trk::TrackParameters*> perigeeList;
  analyzeTracks(trackVector, perigeeList);

  m_estimator->reset();
  m_estimator->addTracks(perigeeList);

  //ATH_MSG_DEBUG("Added " << perigeeList.size() << " tracks to density estimator");

  for (int i = 0; i < 800; i++)
  {
    double z = -200.0 + 0.25 + i*0.5;
    double density = m_estimator->trackDensity(z);
    //ATH_MSG_ALWAYS(" z: " << z << " Density: " << density);
    m_h_density->Fill((float) z, (float) density);
  }


  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void GaussianDensityTestAlg::analyzeTracks(const std::vector<Trk::ITrackLink*>& trackVector,
					 std::vector<const Trk::TrackParameters*>& perigeeList)
{
  for (auto seedtrkAtVtxIter = trackVector.begin();
       seedtrkAtVtxIter != trackVector.end();
       ++seedtrkAtVtxIter) 
  {
    perigeeList.push_back( (*seedtrkAtVtxIter)->parameters() );
  }
}

void GaussianDensityTestAlg::selectTracks(const xAOD::TrackParticleContainer* trackParticles, 
					std::vector<Trk::ITrackLink*>& trackVector)
{
  Root::TAccept selectionPassed;
  for (auto itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
    if (m_useBeamConstraint) {
      xAOD::Vertex beamposition;
      beamposition.makePrivateStore();
      beamposition.setPosition(m_iBeamCondSvc->beamVtx().position());
      beamposition.setCovariancePosition(m_iBeamCondSvc->beamVtx().covariancePosition());
      selectionPassed=m_trackFilter->accept(**itr,&beamposition);
    }
    else
    {
      xAOD::Vertex null;
      null.makePrivateStore();
      null.setPosition(Amg::Vector3D(0,0,0));
      AmgSymMatrix(3) vertexError;
      vertexError.setZero();
      null.setCovariancePosition(vertexError);
      selectionPassed=m_trackFilter->accept(**itr,&null);
    }
    if (selectionPassed)
    {
      ElementLink<xAOD::TrackParticleContainer> link;
      link.setElement(const_cast<xAOD::TrackParticle*>(*itr));
      Trk::LinkToXAODTrackParticle * linkTT = new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles);
      trackVector.push_back(linkTT);
    }
  }
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
}
