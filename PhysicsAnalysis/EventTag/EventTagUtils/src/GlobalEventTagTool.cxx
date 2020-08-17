/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : GlobalEventTagTool.cxx
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a GlobalEventTag - The Tag information associated to the event
          is built here

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "StoreGate/StoreGateSvc.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
//#include "VxVertex/VxTrackAtVertex.h"

#include "EventTagUtils/GlobalEventTagTool.h"
#include "TagEvent/GlobalEventAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include "RecEvent/RecoTimingObj.h"

// Handy definitions for error checking
#include "AthenaKernel/errorcheck.h"

#include <sstream>

/** the constructor */
GlobalEventTagTool::GlobalEventTagTool (const std::string& type, const
                                        std::string& name, const IInterface* parent) :
  EventInfoTagTool( type, name, parent )
  , m_caloClusterContainerName("CaloCalTopoClusters")
  , m_trackContainerName("InDetTrackParticles")
  , m_vertexContainerName("PrimaryVertices")
  , m_mc(false)
  , m_includeVertexFlag(true)
  , m_vertexTrackCut(4)
  , m_includeRecoTime(true)
  , m_recoInclPers(true)
{
  /** AOD Object Name */
  declareProperty("CaloClusterContainer",         m_caloClusterContainerName);
  declareProperty("TrackContainer",               m_trackContainerName);
  declareProperty("VertexContainer",              m_vertexContainerName);
  declareProperty("IncludeVertexFlag",            m_includeVertexFlag);
  declareProperty("UseMC",                        m_mc);
  declareProperty("VertexTrackCut",               m_vertexTrackCut);
  declareProperty("IncludeRecoTime",              m_includeRecoTime);
  declareProperty("RecoIncludesPersistification", m_recoInclPers);
  declareInterface<GlobalEventTagTool>( this );

}

/** initialization - called once at the beginning */
StatusCode  GlobalEventTagTool::initialize() {
  ATH_MSG_DEBUG("in intialize()");

  return EventInfoTagTool::initialize();
}


/** build the attribute list - global event */
StatusCode 
GlobalEventTagTool::attributeSpecification(std::vector<std::pair<std::string,AthenaAttributeType> >& attrMap) 
{

  StatusCode sc = EventInfoTagTool::attributeSpecification(attrMap);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error on EventInfoTagTool::attributeSpecification");
    return sc;
  }
  
  ATH_MSG_DEBUG("in attributeSpecification()");

  if ( m_includeVertexFlag )  {
     for (unsigned int i = Evt::NCaloClus; i <= Evt::VtxNdof; ++i) {
       attrMap.push_back(std::make_pair(EventAttributeSpecs[i].name(),
                                        EventAttributeSpecs[i].attributeType() 
                                        )              );
     }
  }

  if ( m_includeRecoTime )
    {
      for (unsigned int i = Evt::RecoTimeRAW2ESD; i <= Evt::RecoTimeRAWtoALL; ++i)
	{
	  attrMap.push_back(std::make_pair(EventAttributeSpecs[i].name(),
					   EventAttributeSpecs[i].attributeType() ) );
	}
    }
  
  return StatusCode::SUCCESS;
}

/** build the attribute list - global event */
StatusCode 
GlobalEventTagTool::attributeSpecification(std::vector<AthenaAttributeSpecification>& attrMap) 
{

  StatusCode sc = EventInfoTagTool::attributeSpecification(attrMap);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error on EventInfoTagTool::attributeSpecification");
    return sc;
  }

  ATH_MSG_DEBUG("in attributeSpecification()");

  if ( m_includeVertexFlag )  {
     for (unsigned int i = Evt::NCaloClus; i <= Evt::VtxNdof; ++i) {
        attrMap.push_back(EventAttributeSpecs[i]);
     }
  }
  
  if ( m_includeRecoTime )
    {
      for (unsigned int i = Evt::RecoTimeRAW2ESD; i <= Evt::RecoTimeRAWtoALL; ++i)
	{
	  attrMap.push_back(EventAttributeSpecs[i]);
	}
    }
  
  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode  GlobalEventTagTool::execute(TagFragmentCollection& globalEventTag) 
{

  ATH_MSG_DEBUG("in execute()");

  // Must come first if need run or other EventInfo information
  StatusCode scbase = EventInfoTagTool::execute(globalEventTag);
  if (scbase.isFailure()) {
     ATH_MSG_WARNING("Unable to execute base class execute method");
     return scbase;
  }



  if ( m_includeVertexFlag )
    {
      StatusCode sc = caloTag (globalEventTag);
      if (sc.isFailure()) ATH_MSG_WARNING("Unable to build Tag Fragments for CaloClusters");
    
      sc = trackTag (globalEventTag);
      if (sc.isFailure()) ATH_MSG_WARNING("Unable to build Tag Fragments for Tracks");

      sc = vertexTag (globalEventTag);
      if (sc.isFailure()) ATH_MSG_WARNING("Unable to build Tag Fragments for the vertices");
    }
  
  if ( m_mc )
    {
      const DataHandle<RecoTimingObj> recTiming;
      float recoTime(0);
      if ( evtStore()->contains<RecoTimingObj>("EVNTtoHITS_timings") )
        {
          scbase = evtStore()->retrieve( recTiming, "EVNTtoHITS_timings" );
          if (scbase.isFailure()) {
            ATH_MSG_ERROR("Cannot get RecoTimingObj with name <EVNTtoHITS_timings>.");
            return StatusCode::SUCCESS;
          }
          if ( m_recoInclPers ) //hopefully OK to use this here too??
            {
              if ( (*recTiming).size() > 0 )
                recoTime=*((*recTiming).rbegin());
            }
          else
            {
              if ( (*recTiming).size() > 1 )
                recoTime=(*recTiming)[(*recTiming).size()-2];
            }
        }
      ATH_MSG_VERBOSE("inserting RecoTiming <" << recoTime << ">.");
      globalEventTag.insert (EventAttributeSpecs[Evt::RecoTimeEVNT2HITS].name(), recoTime );
      recoTime=0;
      if ( evtStore()->contains<RecoTimingObj>("HITStoRDO_timings") )
        {
          scbase = evtStore()->retrieve( recTiming, "HITStoRDO_timings" );
          if (scbase.isFailure()) {
            ATH_MSG_ERROR("Cannot get RecoTimingObj with name <HITStoRDO_timings>.");
            return StatusCode::SUCCESS;
          }
          if ( m_recoInclPers ) //hopefully OK to use this here too??
            {
              if ( (*recTiming).size() > 0 )
                recoTime=*((*recTiming).rbegin());
            }
          else
            {
              if ( (*recTiming).size() > 1 )
                recoTime=(*recTiming)[(*recTiming).size()-2];
            }
        }
      ATH_MSG_VERBOSE("inserting RecoTiming <" << recoTime << ">.");
      globalEventTag.insert (EventAttributeSpecs[Evt::RecoTimeHITS2RDO].name(), recoTime );
      
    }
  
  if ( m_includeRecoTime )
    {
      const DataHandle<RecoTimingObj> recTiming;
      float recoTime(0);
      if ( evtStore()->contains<RecoTimingObj>("RAWtoESD_timings") )
        {
          scbase = evtStore()->retrieve( recTiming, "RAWtoESD_timings" );
          if (scbase.isFailure()) {
            ATH_MSG_ERROR("Cannot get RecoTimingObj with name <RAWtoESD_timings>.");
            return StatusCode::SUCCESS;
          }
          if ( m_recoInclPers )
            {
              if ( (*recTiming).size() > 0 )
                recoTime=*((*recTiming).rbegin());
            }
          else
            {
              if ( (*recTiming).size() > 1 )
                recoTime=(*recTiming)[(*recTiming).size()-2];
            }
        }
      ATH_MSG_VERBOSE("inserting RecoTiming <" << recoTime << ">.");
      globalEventTag.insert (EventAttributeSpecs[Evt::RecoTimeRAW2ESD].name(), recoTime );
      recoTime=0;
      if ( evtStore()->contains<RecoTimingObj>("ESDtoAOD_timings") )
        {
          scbase = evtStore()->retrieve( recTiming, "ESDtoAOD_timings" );
          if (scbase.isFailure()) {
            ATH_MSG_ERROR("Cannot get RecoTimingObj with name <ESDtoAOD_timings>.");
            return StatusCode::SUCCESS;
          }
          if ( m_recoInclPers )
            {
              if ( (*recTiming).size() > 0 )
                recoTime=*((*recTiming).rbegin());
            }
          else
            {
              if ( (*recTiming).size() > 1 )
                recoTime=(*recTiming)[(*recTiming).size()-2];
            }
        }
      ATH_MSG_VERBOSE("inserting RecoTiming <" << recoTime << ">.");
      globalEventTag.insert (EventAttributeSpecs[Evt::RecoTimeESD2AOD].name(), recoTime );


      recoTime=0;
      if ( evtStore()->contains<RecoTimingObj>("RAWtoALL_timings") )
        {
          scbase = evtStore()->retrieve( recTiming, "RAWtoALL_timings" );
          if (scbase.isFailure()) {
            ATH_MSG_ERROR("Cannot get RecoTimingObj with name <RAWtoALL_timings>.");
            return StatusCode::SUCCESS;
          }
          if ( m_recoInclPers )
            {
              if ( (*recTiming).size() > 0 )
                recoTime=*((*recTiming).rbegin());
            }
          else
            {
              if ( (*recTiming).size() > 1 )
                recoTime=(*recTiming)[(*recTiming).size()-2];
            }
        }
      ATH_MSG_VERBOSE("inserting RecoTiming <" << recoTime << ">.");
      globalEventTag.insert (EventAttributeSpecs[Evt::RecoTimeRAWtoALL].name(), recoTime );

    }
  return StatusCode::SUCCESS;

}



/** build the tag associate to the CaloClusters */
StatusCode GlobalEventTagTool::caloTag(TagFragmentCollection& globalEventTag) {

  ATH_MSG_DEBUG("in execute() - caloTag");

  // retrieve CaloClusters
  const xAOD::CaloClusterContainer* clusterContainer;
  StatusCode sc = evtStore()->retrieve( clusterContainer, m_caloClusterContainerName );
  if (sc.isFailure()) {
      ATH_MSG_WARNING("No CaloCluster found in SG");
      return StatusCode::SUCCESS;
  }
  // number of CaloClusters
  unsigned int num = clusterContainer->size();
  globalEventTag.insert(EventAttributeSpecs[Evt::NCaloClus].name(), num );

  return StatusCode::SUCCESS;
}



/** build the tag associate to the TrackParticles */
StatusCode GlobalEventTagTool::trackTag(TagFragmentCollection& globalEventTag) {

  ATH_MSG_DEBUG("in execute() - trackTag");

  // retrieve TrackParticle
  const xAOD::TrackParticleContainer *tpc;
  StatusCode sc = evtStore()->retrieve( tpc, m_trackContainerName);
  if (sc.isFailure()) {
      ATH_MSG_WARNING("No TrackParticleContainer found in SG");
      return StatusCode::SUCCESS;
  }
  // number of Tracks
  unsigned int num = tpc->size();
  globalEventTag.insert(EventAttributeSpecs[Evt::NTrk].name(), num );

  return StatusCode::SUCCESS;
}



/** build the tag associate to the vertices */
StatusCode GlobalEventTagTool::vertexTag(TagFragmentCollection& globalEventTag) {

  ATH_MSG_DEBUG("in execute() - vertexTag");
  
  const xAOD::VertexContainer *vxc;
  StatusCode sc = evtStore()->retrieve( vxc, m_vertexContainerName);
  if (sc.isFailure()) {
      ATH_MSG_WARNING("No VertexContainer found in SG");
      return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Primary Vertex Container successfully retrieved");

  int NVtx=0;
  xAOD::VertexContainer::const_iterator vtxList=vxc->begin();
  xAOD::VertexContainer::const_iterator vtxList_end=vxc->end();

  for(; vtxList != vtxList_end; ++vtxList) {
    if((*vtxList)!=0)  {
      
      if( (*vtxList)->vertexType()==xAOD::VxType::PriVtx || (*vtxList)->vertexType()==xAOD::VxType::PileUp ){
	NVtx++;
	if( (*vtxList)->vertexType()==xAOD::VxType::PriVtx){
	globalEventTag.insert ( EventAttributeSpecs[Evt::VtxX].name(), (*vtxList)->x() );
	globalEventTag.insert ( EventAttributeSpecs[Evt::VtxY].name(),  (*vtxList)->y() );
	globalEventTag.insert ( EventAttributeSpecs[Evt::VtxZ].name(),  (*vtxList)->z() );
	globalEventTag.insert ( EventAttributeSpecs[Evt::VtxChiSq].name(), (*vtxList)->chiSquared() );
	globalEventTag.insert ( EventAttributeSpecs[Evt::VtxNdof].name(), (*vtxList)->numberDoF());}
      }
    }
  }
  
  globalEventTag.insert ( EventAttributeSpecs[Evt::NVx].name(), NVtx );
  
  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode  GlobalEventTagTool::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return EventInfoTagTool::finalize();
}
