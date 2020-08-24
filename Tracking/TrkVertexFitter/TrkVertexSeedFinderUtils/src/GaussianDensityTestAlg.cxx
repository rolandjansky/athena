/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// GaussianDensityTestAlg.cxx 
// Implementation file for class GaussianDensityTestAlg
// Author: Dave Casper <dcasper@uci.edu>
/////////////////////////////////////////////////////////////////// 

// TrkVertexSeedFinderUtils includes
#include "GaussianDensityTestAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

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
  m_firstEvent(true),
  m_iTHistSvc("THistSvc", name)
{
  //
  // Property declaration
  // 
}

// Destructor
///////////////
GaussianDensityTestAlg::~GaussianDensityTestAlg()
= default;

// Athena Algorithm's Hooks
////////////////////////////
StatusCode GaussianDensityTestAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_trackParticlesKey.initialize() );
  ATH_CHECK( m_truthEventsKey.initialize() );
  ATH_CHECK( m_pileupEventsKey.initialize() );

  ATH_CHECK( m_estimator.retrieve() );
  ATH_CHECK( m_trackFilter.retrieve() );
  ATH_CHECK( m_ipEstimator.retrieve() );
  ATH_CHECK(m_beamSpotKey.initialize());
  // setup histograms/trees
  m_h_density = new TH1F("Density", "Density", 800, -200.0, 200.0);
  m_h_truthDensity = new TH1F("Truth", "Truth", 800, -200.0, 200.0);
  m_h_truthVertices = new TH1F("TruthVertices", "TruthVertices", 800, -200.0, 200.0);
  m_h_modeCheck = new TH1F("ModeOffset", "ModeOffset", 100, -2.0, 2.0);

  CHECK( m_iTHistSvc->regHist("/file1/h/density", m_h_density) );
  CHECK( m_iTHistSvc->regHist("/file1/h/truth", m_h_truthDensity) );
  CHECK( m_iTHistSvc->regHist("/file1/h/truthvertices", m_h_truthVertices) );
  CHECK( m_iTHistSvc->regHist("/file1/h/modeoffset", m_h_modeCheck) );

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

  ATH_MSG_VERBOSE("Selecting tracks");
  std::vector<Trk::ITrackLink*> trackVector;
  selectTracks(trackParticles.cptr(), trackVector);

  std::vector<const Trk::TrackParameters*> perigeeList;
  analyzeTracks(trackVector, perigeeList);

  ATH_MSG_VERBOSE("Using density estimator");
  std::unique_ptr<Trk::IVertexTrackDensityEstimator::ITrackDensity> dens;
  double mode = m_estimator->globalMaximum (perigeeList, dens);

  if (m_firstEvent)
  {
    for (int i = 0; i < 800; i++)
    {
      double z = -200.0 + 0.25 + i*0.5;
      double density = dens->trackDensity(z);
      m_h_density->Fill((float) z, (float) density);
    }
  }
  ATH_MSG_VERBOSE("Analyzing MC truth");
  std::vector<Amg::Vector3D> truth;
  ATH_CHECK( findTruth(mode, trackVector, truth) );
  if (m_firstEvent)
  {
    ATH_MSG_VERBOSE("Filling truth vertex histogram");
    for (auto& v : truth) m_h_truthVertices->Fill( v[2] );
  }
  m_firstEvent = false;
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
  bool selectionPassed{false};
  const InDet::BeamSpotData* beamspot = nullptr;
  if(m_useBeamConstraint){
     SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
     if(beamSpotHandle.isValid()) beamspot = beamSpotHandle.retrieve();
  }
  for (auto itr  = trackParticles->begin(); itr != trackParticles->end(); ++itr) {
    if (m_useBeamConstraint) {
      xAOD::Vertex beamposition;
      beamposition.makePrivateStore();
      beamposition.setPosition(beamspot->beamVtx().position());
      beamposition.setCovariancePosition(beamspot->beamVtx().covariancePosition());
      selectionPassed=static_cast<bool>(m_trackFilter->accept(**itr,&beamposition));
    }
    else
    {
      xAOD::Vertex null;
      null.makePrivateStore();
      null.setPosition(Amg::Vector3D(0,0,0));
      AmgSymMatrix(3) vertexError;
      vertexError.setZero();
      null.setCovariancePosition(vertexError);
      selectionPassed=static_cast<bool>(m_trackFilter->accept(**itr,&null));
    }
    if (selectionPassed)
    {
      ElementLink<xAOD::TrackParticleContainer> link;
      link.setElement(*itr);
      Trk::LinkToXAODTrackParticle * linkTT = new Trk::LinkToXAODTrackParticle(link);
      linkTT->setStorableObject(*trackParticles);
      trackVector.push_back(linkTT);
    }
  }
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
StatusCode
GaussianDensityTestAlg::findTruth(double mode,
                                  const std::vector<Trk::ITrackLink*>& trackVector,
                                  std::vector<Amg::Vector3D>& truth) const
{
    double modeClosestDistance = std::numeric_limits<double>::max();

    xAOD::TrackParticle::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > truthParticleAssoc("truthParticleLink");

    SG::ReadHandle<xAOD::TruthEventContainer> signalEvents(m_truthEventsKey);
    if ( signalEvents.isValid() )
    {
      ATH_MSG_VERBOSE("Found signalEvents");
      for (const xAOD::TruthEventBase* evt : *signalEvents)
      {
	const xAOD::TruthVertex* vLink = *(evt->truthVertexLink(0));
	if (vLink == nullptr) ATH_MSG_ERROR("Invalid truthVertexLink from signalEvents");
	Amg::Vector3D vTruth(vLink->x(),vLink->y(),vLink->z());
	int nGoodTracks = 0;
	for (auto trk : trackVector)
	{
	    Trk::LinkToXAODTrackParticle* lxtp = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trk);
	    if (lxtp)
	    {
		bool isAssoc = truthParticleAssoc(**(*lxtp)).isValid();
		if (isAssoc)
	        {
		    auto assocParticle = truthParticleAssoc(**(*lxtp));
		    ATH_MSG_VERBOSE("Found associated truth particle");
		    for (const auto& truthParticle : evt->truthParticleLinks())
		    {
		        if (!truthParticle.isValid()) continue;
			if (assocParticle == truthParticle)
		        {
			  ATH_MSG_VERBOSE("Calling ipSignificance");
			    double significance = ipSignificance(trk->parameters(), &vTruth);
			    ATH_MSG_VERBOSE("ipSignificance returned " << significance);
			    if (significance <= m_significanceTruthCut) 
			    {
			      ATH_MSG_VERBOSE("Adding good track");
			      nGoodTracks++;
			      const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*>(trk->parameters());
			      if (perigee == nullptr) ATH_MSG_ERROR("Invalid Perigee");
			      if (m_firstEvent) 
			      {
				m_h_truthDensity->Fill(vLink->z());
				ATH_MSG_VERBOSE("Filled truth density histogram");
			      }
			    }
			    break;
			}
		    }
	        }
                else
		{
		  ATH_MSG_VERBOSE("No associated truth particle found");
		}
	    }
        }
        if (nGoodTracks >= m_truthVertexTracks)
        {
    	  truth.push_back(vTruth);
	  if (abs(modeClosestDistance) > abs(mode - vTruth[2]))
	    modeClosestDistance = mode - vTruth[2];
        }
      }
    }
    else
    {
      ATH_MSG_WARNING("No TruthEventContainer found");
    }

    SG::ReadHandle<xAOD::TruthPileupEventContainer> pileupEvents(m_pileupEventsKey);
    if ( pileupEvents.isValid() )
    {
      ATH_MSG_VERBOSE("Found pileupEvents");
      for (const xAOD::TruthEventBase* evt : *pileupEvents)
      {
	const xAOD::TruthVertex* vLink = *(evt->truthVertexLink(0));
	if (vLink == nullptr) ATH_MSG_ERROR("Invalid truthVertexLink from pileupEvents");
	Amg::Vector3D vTruth(vLink->x(),vLink->y(),vLink->z());
	int nGoodTracks = 0;
	for (auto trk : trackVector)
	{
	    Trk::LinkToXAODTrackParticle* lxtp = dynamic_cast<Trk::LinkToXAODTrackParticle*>(trk);
	    if (lxtp)
	    {
		bool isAssoc = truthParticleAssoc(**(*lxtp)).isValid();
		if (isAssoc)
	        {
		    auto assocParticle = truthParticleAssoc(**(*lxtp));
		    ATH_MSG_VERBOSE("Found associated truth particle");
		    for (const auto& truthParticle : evt->truthParticleLinks())
		    {
		        if (!truthParticle.isValid()) continue;
			if (assocParticle == truthParticle)
		        {
			  ATH_MSG_VERBOSE("Calling ipSignificance");
			    double significance = ipSignificance(trk->parameters(), &vTruth);
			    ATH_MSG_VERBOSE("ipSignificance returned " << significance);
			    if (significance <= m_significanceTruthCut) 
			    {
			      ATH_MSG_VERBOSE("Adding good track");
			      nGoodTracks++;
			      const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*>(trk->parameters());
			      if (perigee == nullptr) ATH_MSG_ERROR("Invalid Perigee");
			      if (m_firstEvent)
			      {
				m_h_truthDensity->Fill(vLink->z());
				ATH_MSG_VERBOSE("Filled truth density histogram");
			      }
			    }
			    break;
			}
		    }
	        }
                else
		{
		  ATH_MSG_VERBOSE("No associated truth particle found");
		}
	    }
        }
        if (nGoodTracks >= m_truthVertexTracks)
	{
	    truth.push_back(vTruth);
	  if (abs(modeClosestDistance) > abs(mode - vTruth[2]))
	    modeClosestDistance = mode - vTruth[2];
	}
      }
    }
    else
    {
      ATH_MSG_WARNING("No TruthPileupEventContainer found");
    }

    m_h_modeCheck->Fill( modeClosestDistance );
    return StatusCode::SUCCESS;
  }

  double GaussianDensityTestAlg::ipSignificance( const Trk::TrackParameters* params, 
                                                 const Amg::Vector3D * vertex ) const
  {
    xAOD::Vertex v;
    v.makePrivateStore();
    v.setPosition(*vertex);
    v.setCovariancePosition(AmgSymMatrix(3)::Zero(3,3));
    v.setFitQuality(0., 0.);

    double significance = 0.0;
    const ImpactParametersAndSigma* ipas = m_ipEstimator->estimate( params, &v );
    if ( ipas != nullptr )
    {  
      if ( ipas->sigmad0 > 0 && ipas->sigmaz0 > 0)
      {
	significance = sqrt( pow(ipas->IPd0/ipas->sigmad0,2) + pow(ipas->IPz0/ipas->sigmaz0,2) );
      }
      delete ipas;
    }
    return significance;
  }
}
