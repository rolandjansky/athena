///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrackVertexAssoTestAlg.cxx 
// Implementation file for class TrackVertexAssoTestAlg
// Author: Rachid Mazini, Bo Liu 
/////////////////////////////////////////////////////////////////// 

// TrackVertexAssociationTool includes
#include "TrackVertexAssoTestAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

using namespace std;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TrackVertexAssoTestAlg::TrackVertexAssoTestAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  declareProperty( "TVATool", m_TVATool );
}

// Destructor
///////////////
TrackVertexAssoTestAlg::~TrackVertexAssoTestAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode TrackVertexAssoTestAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_trkContname.initialize() );
  ATH_CHECK( m_vertexContname.initialize() );
  //retrieve tool from ToolHandle
  CHECK(m_TVATool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TrackVertexAssoTestAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TrackVertexAssoTestAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // retrieve containers
  SG::ReadHandle<xAOD::TrackParticleContainer> trkCont(m_trkContname);

  SG::ReadHandle<xAOD::VertexContainer> vxCont(m_vertexContname);

  if (!trkCont.isValid() || !vxCont.isValid()) {
    if (!trkCont.isValid()) ATH_MSG_ERROR("TrackParticle container not found");
    if (!vxCont.isValid()) ATH_MSG_ERROR("Vertex container not found");
    return StatusCode::FAILURE;
  }

  // Test isCompitable
  ATH_MSG_INFO("Testing TrackVertexAssociationTool::isCompatible...");
  if(trkCont->size()!=0 && vxCont->size()!=0)
  {
    bool isMatched = m_TVATool->isCompatible(*(trkCont->at(0)), *(vxCont->at(0)));
    ATH_MSG_INFO("Is the first track compatible with the first vertex (the PriVx)? "<< isMatched);
  }

  // Test getMatchMap
  ATH_MSG_INFO("Testing TrackVertexAssociationTool::getMatchMap...");
  xAOD::TrackVertexAssociationMap trkvxassoMap = m_TVATool->getMatchMap(*trkCont, *vxCont);
  ATH_MSG_INFO("Number of vertices for track-vertex association: " << trkvxassoMap.size());
  for (const auto& assoc: trkvxassoMap) {
    const xAOD::Vertex *vx = assoc.first;
    ATH_MSG_INFO("vertex at x, y, z " <<
                 setprecision(4) << setfill(' ') <<
                 setw(10) << vx->x() << ", " << setw(10) << vx->y() << ", " << setw(10) << vx->z() <<
                 " has " << assoc.second.size() << " associated tracks");
  }

  // Test getUniqueMatchVertex
  ATH_MSG_INFO("Testing TrackVertexAssociationTool::getUniqueMatchVertex...");
  std::vector<const xAOD::Vertex* > v_vx;
  v_vx.clear();
  for(auto *vertex : *vxCont) {
    v_vx.push_back(vertex);
  }
  if(trkCont->size()!=0)
  {
    const xAOD::Vertex *vx=m_TVATool->getUniqueMatchVertex(*(trkCont->at(0)), v_vx);
    ATH_MSG_INFO("Unique match vertex for first track: " << vx);
  }

  // Test getUniqueMatchVertexLink
  ATH_MSG_INFO("Testing TrackVertexAssociationTool::getUniqueMatchVertexLink...");
  if(trkCont->size() > 2)
  {
    ElementLink<xAOD::VertexContainer> match_vx = m_TVATool->getUniqueMatchVertexLink(*(trkCont->at(2)), *vxCont );

    if(match_vx.isValid())
    {
      ATH_MSG_INFO( "Uniquely matched vertex for third track - ");
      ATH_MSG_INFO( "Vertex ElementLink address: " << match_vx );
      ATH_MSG_INFO( "Vertex address: " << *match_vx );
      ATH_MSG_INFO( "Vertex z pos: " << (*match_vx)->z());
    }
  }

  // Test getUniqueMatchMap
  ATH_MSG_INFO("Testing TrackVertexAssociationTool::getUniqueMatchMap...");
  xAOD::TrackVertexAssociationMap trkvxassoUniqueMap = m_TVATool->getUniqueMatchMap(*trkCont, *vxCont);
  ATH_MSG_INFO("Number of vertices for track-vertex association: " << trkvxassoUniqueMap.size());
  for (const auto& assoc: trkvxassoUniqueMap) {
    const xAOD::Vertex *vx = assoc.first;
    ATH_MSG_INFO("vertex at x, y, z " <<
                 setprecision(4) << setfill(' ') <<
                 setw(10) << vx->x() << ", " << setw(10) << vx->y() << ", " << setw(10) << vx->z() <<
                 " has " << assoc.second.size() << " uniquely associated tracks");
  }

  // Example of accessing tracks matched to each vertex. Tracks are stored in a std::vector<xAOD::TrackParticle* >, for more details see TrackVertexAssociationMap.h

  // const xAOD::Vertex *pv=vxCont->at(0);
  // xAOD::TrackVertexAssociationList trkvxassoList = trkvxassoMap[pv];
  // ATH_MSG_INFO("Number of tracks associated to the PriVx: " << trkvxassoList.size());



  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


