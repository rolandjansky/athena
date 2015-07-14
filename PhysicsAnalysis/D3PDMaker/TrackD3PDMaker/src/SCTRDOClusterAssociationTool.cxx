/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRDOClusterAssociationTool.h"
 
#include "AthenaKernel/errorcheck.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include <map>

#include "TrkTrack/TrackStateOnSurface.h"
#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "InDetIdentifier/SCT_ID.h"
#include "xAODEventInfo/EventInfo.h"


namespace D3PD {
 
  SCTRDOClusterAssociationTool::SCTRDOClusterAssociationTool (const std::string& type,
							      const std::string& name,
							      const IInterface* parent)
    : Base (type, name, parent),
      m_cl(0),
      m_eventNumber(0),
      m_sctId(0)
  {
    declareProperty ("ClusterConteinerKey", m_clusterContainer = "SCT_Clusters");
  }

 
  StatusCode SCTRDOClusterAssociationTool::initialize()
  {
    CHECK( Base::initialize() );
    CHECK( detStore()->retrieve(m_sctId,"SCT_ID") );

    m_eventNumber = -1;
    return StatusCode::SUCCESS;
  }
 
 
  StatusCode SCTRDOClusterAssociationTool::book ()
  {
 
    //CHECK( addVariable ("", m_variable) );
 
    return StatusCode::SUCCESS;
  }


  const InDet::SCT_Cluster* SCTRDOClusterAssociationTool::get (const SCT_RDORawData& rdo)
  {
 
    // Get Track Collections
    // Should be using Getter tools intead of StoreGate
    // -- Tracks

    const InDet::SCT_Cluster *cluster = 0; 

    Identifier rdoId = rdo.identify();
    int rdoTmpID = tempmoduleid(rdoId);
    if ( !rdoTmpID ){ 
      REPORT_MESSAGE (MSG::ERROR) <<" Could not calculate tempID";
      return 0;
    }

    const xAOD::EventInfo* eventInfo;
    StatusCode sc = evtStore()->retrieve(eventInfo);
    if (sc.isFailure())
      {
	REPORT_MESSAGE  (MSG::ERROR) << "Could not retrieve event info" << endreq;
	return 0;
      }
    int eventNumber = (int)eventInfo->eventNumber();

    if ( m_eventNumber != eventNumber ) {

      m_eventNumber = eventNumber;  
      m_rdoClusterMap.clear();

      StatusCode sc = evtStore()->retrieve(m_cl, m_clusterContainer);  
      if(sc.isFailure() || !m_cl) {
	REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve Track Collection";
	return 0;
      }


      InDet::SCT_ClusterContainer::const_iterator clItr = m_cl->begin();
      for( ; clItr!=m_cl->end(); clItr++){

	InDet::SCT_ClusterCollection::const_iterator rioItr = (*clItr)->begin();
	for (; rioItr != (*clItr)->end(); rioItr++) {


	  //const std::vector<Identifier>& rdoList = (*rioItr)->rdoList();

	  Identifier clIdentifier = (*rioItr)->identify();

	  int tmpID = tempmoduleid( clIdentifier );
	  if ( !tmpID ) {

	    REPORT_MESSAGE (MSG::ERROR) << "could not calculater tmpID for cluster"; 
	    continue; 

	  }

	  std::vector< const InDet::SCT_Cluster* > sctClusterIdentifier; 
	  //sctClusterIdentifier.insert(sctClusterIdentifier.end(),rdoList.begin(),rdoList.end());
	  sctClusterIdentifier.push_back ( (*rioItr) );
	  if ( m_rdoClusterMap.find(tmpID) == m_rdoClusterMap.end() ){

	    m_rdoClusterMap.insert(std::pair< int, std::vector<const InDet::SCT_Cluster*> >(tmpID,sctClusterIdentifier) );

	  }
	  else{
	    m_rdoClusterMap[tmpID].push_back( (*rioItr) );

	  }
	} // Cluster Collection Loop
      } // cluster container loop 
    }// one per event

    if (  m_rdoClusterMap.find(rdoTmpID) != m_rdoClusterMap.end() ){

      std::vector< const InDet::SCT_Cluster *> cloi = m_rdoClusterMap[rdoTmpID];
      for( std::vector< const InDet::SCT_Cluster* >::iterator vIt = cloi.begin(); vIt!=cloi.end(); vIt++){

	const std::vector<Identifier>& rdoList = (*vIt)->rdoList();

	if ( find(rdoList.begin(),rdoList.end(),rdoId)!= rdoList.end() )
	  cluster = (*vIt);

	if ( cluster ) break;

      }//vector loop

    } // if module is in the map

    return cluster;
  }

  int SCTRDOClusterAssociationTool::tempmoduleid( Identifier id ){

    int intbuf = 0;
    int bec = (int)m_sctId->barrel_ec(id);
    int bec1 = bec;
    int layer = (int)m_sctId->layer_disk(id);
    int phi_module = (char)m_sctId->phi_module(id);
    int eta_module = (char)m_sctId->eta_module(id);
    int eta_module1 = eta_module;
    int side = (int)m_sctId->side(id);
    int firstPrefix = 1;
    if ( bec != 0 ){
      firstPrefix = 2;
        if ( bec < 0 ) bec1 = bec*(-1);
    }
    int secondPrefix = 2;
    if ( eta_module < 0 ){
      secondPrefix = 1;
      eta_module1 = eta_module*(-1);
    }

    char buf[1024];
    intbuf = 0;
    sprintf(buf,"%i%i%i%i%i%i%i",firstPrefix,secondPrefix,side,bec1,layer,eta_module1,phi_module);
    //if(!(from_string<int>(intbuf, std::string(buf), std::dec))){
    if ( EOF == sscanf(buf, "%i", &intbuf) ){
      REPORT_MESSAGE (MSG::ERROR) <<" Cannot convert string to int...";
      return 0;
    }

    return intbuf;
  }

 
} // namespace D3PD
