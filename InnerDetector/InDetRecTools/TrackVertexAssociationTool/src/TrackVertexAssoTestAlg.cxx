///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackVertexAssoTestAlg.cxx 
// Implementation file for class TrackVertexAssoTestAlg
// Author: Rachid Mazini, Bo Liu 
/////////////////////////////////////////////////////////////////// 

// TrackVertexAssociationTool includes
#include "TrackVertexAssoTestAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

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
  declareProperty( "TrackContainer", m_trkContname );
  declareProperty( "VertexContainer", m_vertexContname );
  declareProperty( "TightTrackVertexAssoTool", m_tighttrackvertexassoTool);
  declareProperty( "LooseTrackVertexAssoTool", m_loosetrackvertexassoTool);
  declareProperty( "ElectronTrackVertexAssoTool", m_electrontrackvertexassoTool);

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

  //retrieve tool from ToolHandle
  CHECK(m_loosetrackvertexassoTool.retrieve());
  CHECK(m_tighttrackvertexassoTool.retrieve());
  CHECK(m_electrontrackvertexassoTool.retrieve());


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

  // retrieve TrackContainer 

  const xAOD::TrackParticleContainer *trkCont=0;
  CHECK(evtStore()->retrieve(trkCont, m_trkContname));

  const xAOD::VertexContainer *vxCont=0;
  CHECK(evtStore()->retrieve(vxCont, m_vertexContname));

  const xAOD::Vertex *vx_test=0;

  // Check compitable 
  //bool isMatched=m_tighttrackvertexassoTool->isCompatible(*(trkCont->at(0)), *(vxCont->at(0)));
  if(trkCont->size()!=0)
  {
  bool isMatched=m_loosetrackvertexassoTool->isCompatible(*(trkCont->at(0)), *vx_test);
  ATH_MSG_INFO("compitable? "<< isMatched);
  }

  // do Match, match to all compitable vertices
  xAOD::TrackVertexAssociationMap trkvxassoMap_tight = m_tighttrackvertexassoTool->getMatchMap(*trkCont, *vxCont);
  xAOD::TrackVertexAssociationMap trkvxassoMap_loose = m_loosetrackvertexassoTool->getMatchMap(*trkCont, *vxCont);
  xAOD::TrackVertexAssociationMap trkvxassoMap_electron = m_electrontrackvertexassoTool->getMatchMap(*trkCont, *vxCont);

  ATH_MSG_INFO("Number of vertices for electron track-vertex association: " << trkvxassoMap_electron.size());
  for (const auto& assoc: trkvxassoMap_electron) {
    const xAOD::Vertex *vx = assoc.first;
    ATH_MSG_INFO("vertex at x, y, z " <<
                 setprecision(4) << setfill(' ') <<
                 setw(10) << vx->x() << ", " << setw(10) << vx->y() << ", " << setw(10) << vx->z() <<
                 " has " << assoc.second.size() << " associated tracks");
  }

  std::vector<const xAOD::Vertex* > v_vx;
  v_vx.clear();

  if(trkCont->size()!=0)
  {
    const xAOD::Vertex *vx=m_tighttrackvertexassoTool->getUniqueMatchVertex(*(trkCont->at(0)), v_vx);
    ATH_MSG_INFO(vx);
  }

  // do Match, only match the best matched vertex
  xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_tight = m_tighttrackvertexassoTool->getUniqueMatchMap(*trkCont, *vxCont);
  xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_loose = m_loosetrackvertexassoTool->getUniqueMatchMap(*trkCont, *vxCont);
  xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_electron = m_electrontrackvertexassoTool->getUniqueMatchMap(*trkCont, *vxCont);

//  std::vector<xAOD::TrackParticle &> trk_list_ref;
//  trk_list_ref.clear();


  // example of access tracks match to each vertex, tracks stored in std::vector<xAOD::TrackParticle* >, more seen TrackVertexAssociationMap.h file

//  const xAOD::Vertex *pv=vxCont->at(0);
//  xAOD::TrackVertexAssociationList trkvxassoList_tight=trkvxassoMap_tight[pv];
//  ATH_MSG_INFO("Number of track PV associated: "<< trkvxassoList_tight.size());


  // Test of ElementLink
  if(trkCont->size()>0)
  {
    ElementLink<xAOD::VertexContainer> match_vx=m_tighttrackvertexassoTool->getUniqueMatchVertexLink(*(trkCont->at(0)), *vxCont );

    //  // 


    if(match_vx.isValid())
    {
      ATH_MSG_INFO( match_vx );
      ATH_MSG_INFO( *match_vx );
      ATH_MSG_INFO( (*match_vx)->z());
    }
  }




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


