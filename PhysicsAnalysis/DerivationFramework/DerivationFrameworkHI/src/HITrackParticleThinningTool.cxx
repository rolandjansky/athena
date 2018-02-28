/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHI/HITrackParticleThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <vector>


namespace DerivationFramework
{
  HITrackParticleThinningTool::HITrackParticleThinningTool( 	const std::string& t,
								const std::string& n,
								const IInterface* p ) :   AthAlgTool(t,n,p),
											  m_thinningSvc("ThinningSvc",n)

  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("InDetTrackParticlesKey", m_TP_key="InDetTrackParticles");
    declareProperty("PrimaryVertexKey", m_vertex_key="PrimaryVertices");
    declareProperty("PrimaryVertexSelection", m_vertex_scheme="SumPt");
    declareProperty("TrackSelectionTool", m_trkSelTool, "Track selection tool" );
  }
  
  // Destructor
  DerivationFramework::HITrackParticleThinningTool::~HITrackParticleThinningTool() 
  {
  }  

  // Athena initialize and finalize
  StatusCode DerivationFramework::HITrackParticleThinningTool::initialize()
  {
    if(!m_trkSelTool)
    {
      ATH_MSG_ERROR("InDetTrackSelectionTool not set. Cannot initialize.");
      return StatusCode::FAILURE;
    }
    //ATH_CHECK(m_trkSelTool->initialize());
    return StatusCode::SUCCESS;
  }
  StatusCode DerivationFramework::HITrackParticleThinningTool::finalize()
  {
    ATH_CHECK(m_trkSelTool->finalize());
    return StatusCode::SUCCESS;
  }

  StatusCode DerivationFramework::HITrackParticleThinningTool::doThinning() const
  {


    const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_TP_key);
    if(!tracks) 
    {
      ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key " << m_TP_key);
      return StatusCode::FAILURE;
    }

    //Define a primary vertex if requested
    const xAOD::Vertex* primary_vertex=nullptr;
    if(m_vertex_scheme.compare("")!=0)
    {
      //Retrieve
      const xAOD::VertexContainer* pv=evtStore()->retrieve< const xAOD::VertexContainer >(m_vertex_key);
      if(!pv)
      {
	ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key " << m_vertex_key);
	return StatusCode::FAILURE;
      }
      //Take PV with either max pT or max tracks depending on chosen scheme
      float ptmax=0.;
      size_t ntrkmax=0;
      for(auto vert : *pv)
      {
	if(!vert) continue;
	if(! (vert->vertexType()==xAOD::VxType::PriVtx)) continue;

	if(m_vertex_scheme.compare("sumPt2")==0)
	{
	  if(vert->isAvailable<float>("sumPt2"))
	  {
	    float sumPT=vert->auxdataConst<float>("sumPt2");
	    if(sumPT > ptmax) 
	    {
	      ptmax=sumPT;
	      primary_vertex=vert;
	    }
	  }
	}
	else
	{
	  size_t ntp=vert->nTrackParticles();
	  if(ntp > ntrkmax)
	  {
	    ntrkmax=ntp;
	    primary_vertex=vert;
	  }
	}
      }
    }

    std::vector<bool> mask;
    mask.reserve(tracks->size());
    for(auto tp : *tracks)
    {
      if(tp)
      {
	const xAOD::Vertex* vert_trk=(primary_vertex) ? primary_vertex : tp->vertex();
	if(vert_trk) mask.push_back(m_trkSelTool->accept(*tp,vert_trk));
	else mask.push_back(m_trkSelTool->accept(*tp));
      }
      else mask.push_back(false);
    }
    if (m_thinningSvc->filter(*tracks, mask, IThinningSvc::Operator::Or).isFailure())
    {
      ATH_MSG_ERROR("Application of thinning service failed! ");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }  
  
}
