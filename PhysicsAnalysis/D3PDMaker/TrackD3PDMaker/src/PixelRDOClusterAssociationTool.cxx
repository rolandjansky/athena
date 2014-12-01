/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDOClusterAssociationTool.h"
 
#include "AthenaKernel/errorcheck.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include <map>

#include "TrkTrack/TrackStateOnSurface.h"
#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "InDetIdentifier/PixelID.h"
#include "xAODEventInfo/EventInfo.h"


namespace D3PD {
 
  PixelRDOClusterAssociationTool::PixelRDOClusterAssociationTool (const std::string& type,
				       			      const std::string& name,
							      const IInterface* parent)
    : Base (type, name, parent),
      m_cl(0),
      m_eventNumber(0),
      m_pixelId(0)
  {
    declareProperty ("ClusterConteinerKey", m_clusterContainer = "PixelClusters");
  }

 
  StatusCode PixelRDOClusterAssociationTool::initialize()
  {
    CHECK( Base::initialize() );
    CHECK( detStore()->retrieve(m_pixelId,"PixelID") );

    m_eventNumber = -1;
    return StatusCode::SUCCESS;
  }
 
 
  StatusCode PixelRDOClusterAssociationTool::book ()
  {
 
    //CHECK( addVariable ("", m_variable) );
 
    return StatusCode::SUCCESS;
  }


  const InDet::PixelCluster* PixelRDOClusterAssociationTool::get (const PixelRDORawData& rdo)
  {
 
    // Get Track Collections
    // Should be using Getter tools intead of StoreGate
    // -- Tracks

    const InDet::PixelCluster *cluster = 0; 

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


      InDet::PixelClusterContainer::const_iterator clItr = m_cl->begin();
      for( ; clItr!=m_cl->end(); clItr++){

	InDet::PixelClusterCollection::const_iterator rioItr = (*clItr)->begin();
	for (; rioItr != (*clItr)->end(); rioItr++) {


	  //const std::vector<Identifier>& rdoList = (*rioItr)->rdoList();

	  Identifier clIdentifier = (*rioItr)->identify();

	  int tmpID = tempmoduleid( clIdentifier );
	  if ( !tmpID ) {

	    REPORT_MESSAGE (MSG::ERROR) << "could not calculater tmpID for cluster"; 
	    continue; 

	  }

	  std::vector< const InDet::PixelCluster* > pixelClusterIdentifier; 
	  pixelClusterIdentifier.push_back ( (*rioItr) );
	  if ( m_rdoClusterMap.find(tmpID) == m_rdoClusterMap.end() ){

	    m_rdoClusterMap.insert(std::pair< int, std::vector<const InDet::PixelCluster*> >(tmpID,pixelClusterIdentifier) );

	  }
	  else{
	    m_rdoClusterMap[tmpID].push_back( (*rioItr) );

	  }
	} // Cluster Collection Loop
      } // cluster container loop 
    }// one per event

    if (  m_rdoClusterMap.find(rdoTmpID) != m_rdoClusterMap.end() ){

      std::vector< const InDet::PixelCluster *> cloi = m_rdoClusterMap[rdoTmpID];
      for( std::vector< const InDet::PixelCluster* >::iterator vIt = cloi.begin(); vIt!=cloi.end(); vIt++){

	const std::vector<Identifier>& rdoList = (*vIt)->rdoList();

	if ( find(rdoList.begin(),rdoList.end(),rdoId)!= rdoList.end() )
	  cluster = (*vIt);

	if ( cluster ) break;

      }//vector loop

    } // if module is in the map

    return cluster;
  }

  int PixelRDOClusterAssociationTool::tempmoduleid( Identifier id ){

    int intbuf = 0;
    int bec = (int)m_pixelId->barrel_ec(id);
    int bec1 = bec;
    int layer = (int)m_pixelId->layer_disk(id);
    int phi_module = (char)m_pixelId->phi_module(id);
    int eta_module = (char)m_pixelId->eta_module(id);
    int eta_module1 = eta_module;
    //int side = (int)m_pixelId->side(id);
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
    //sprintf(buf,"%i%i%i%i%i%i%i",firstPrefix,secondPrefix,side,bec1,layer,eta_module1,phi_module);
    sprintf(buf,"%i%i%i%i%i%i",firstPrefix,secondPrefix,bec1,layer,eta_module1,phi_module);
    //if(!(from_string<int>(intbuf, std::string(buf), std::dec))){
    if ( EOF == sscanf(buf, "%i", &intbuf) ){
      REPORT_MESSAGE (MSG::ERROR) <<" Cannot convert string to int...";
      return 0;
    }

    return intbuf;
  }

 
} // namespace D3PD
