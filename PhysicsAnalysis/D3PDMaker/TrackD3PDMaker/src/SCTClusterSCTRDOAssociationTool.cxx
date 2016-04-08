/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************/
// @author <yuriy.pylypchenko@cern.ch>                    /
// @date Oct, 2010                                        /
// @brief setting up an association of clusters to tracks /
//       represented by indices.                          /
//********************************************************/
 
 
#include "SCTClusterSCTRDOAssociationTool.h"
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


SCTClusterSCTRDOAssociationTool::SCTClusterSCTRDOAssociationTool (const std::string& type,
								const std::string& name,
								const IInterface* parent)
  : SCTClusterSCTRDOBase(type, name, parent),
    m_sctId(0),
    m_eventNumber(0),
    m_rdocont(0)
{
}
 

/// initialize method.
StatusCode SCTClusterSCTRDOAssociationTool::initialize()
{
 
  //return  SCTClusterSCTRDOBase::initialize();
  CHECK ( SCTClusterSCTRDOBase::initialize() );
  if (detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve SCT ID helper";
    return StatusCode::FAILURE;
  }

  m_eventNumber = -1;

  return StatusCode::SUCCESS;
}
/// booking btuple variables
StatusCode SCTClusterSCTRDOAssociationTool::book()
{
 
  CHECK( SCTClusterSCTRDOBase::book() );
  
  return StatusCode::SUCCESS;
}
StatusCode SCTClusterSCTRDOAssociationTool::invalidate(StatusCode sc){

  m_rdoItr = m_rdoEnd;
  return sc;
}

///
/// @brief Start the iteration for a new association.
/// @param p The object from which to associate.
///
StatusCode SCTClusterSCTRDOAssociationTool::reset (const  InDet::SCT_Cluster& p)
{

  m_sctRDOForAssociation.clear();

 
  int clusterModuleID = 0; 
  StatusCode sc = tempmoduleid(p.identify(), clusterModuleID);
  if ( sc.isFailure() ) REPORT_MESSAGE (MSG::ERROR)<< "Failed to calculate the tempID.";

  std::vector< Identifier> sctClusterIdentifier; 
  std::vector<Identifier>::iterator posItr;

  const std::vector<Identifier>& rdoList = p.rdoList();
  sctClusterIdentifier.insert(sctClusterIdentifier.end(),rdoList.begin(),rdoList.end());


  const xAOD::EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure())
    {
      REPORT_MESSAGE  (MSG::ERROR) << "Could not retrieve event info";
      return StatusCode::FAILURE;
    }
  int eventNumber = (int)eventInfo->eventNumber();

  if (m_eventNumber != eventNumber ){
    // For each event

    m_globalmapOfRDOsPerModule.clear();
    m_eventNumber = eventNumber;

    if(!evtStore()->contains<SCT_RDO_Container>("SCT_RDOs")){
      REPORT_MESSAGE (MSG::WARNING) << "RDOTESTASSOC: No SCT RDO container in StoreGate";
    }
    else {
      sc =  evtStore()->retrieve(m_rdocont,"SCT_RDOs");
      if(sc.isFailure()) {
	REPORT_MESSAGE (MSG::WARNING) << "Failed to retrieve SCT RDO container";
      }
    }
    
    if ( m_rdocont != 0){

    // get all the RIO_Collections in the container
    SCT_RDO_Container::const_iterator contIt = m_rdocont->begin();
       for( ; contIt != m_rdocont->end(); contIt++){
 
	 //get all the RDOs in the collection
	 //const SCT_RDO_Collection* SCT_Collection(*contIt);
         SCT_RDO_Collection::const_iterator rdoIt = (*contIt)->begin();
         for (; rdoIt != (*contIt)->end(); rdoIt++) {

           if (!(*rdoIt)) {
             REPORT_MESSAGE (MSG::WARNING) << "Null rdo....";
             continue;
           }

	   Identifier rdoId = (*rdoIt)->identify();

	   int rdoModuleID = 0; 
	   StatusCode sc2 = tempmoduleid (rdoId, rdoModuleID);
           if ( sc2.isFailure() ) REPORT_MESSAGE (MSG::WARNING)<<" Failed to calculate RDO tempID!";

	   std::map< Identifier, const SCT_RDORawData* > tempmap;
           tempmap.insert( std::pair< Identifier, const SCT_RDORawData*>( rdoId, (*rdoIt) ) );

	   if ( m_globalmapOfRDOsPerModule.find(rdoModuleID) == m_globalmapOfRDOsPerModule.end() )
	     m_globalmapOfRDOsPerModule.insert( std::pair< int, std::map< Identifier, const SCT_RDORawData*> >( rdoModuleID, tempmap ));
	   else {
	     if  ( m_globalmapOfRDOsPerModule[rdoModuleID].find(rdoId) == m_globalmapOfRDOsPerModule[rdoModuleID].end() )
	       m_globalmapOfRDOsPerModule[rdoModuleID].insert(std::pair<Identifier, const SCT_RDORawData*>(rdoId,(*rdoIt)));
	     else REPORT_MESSAGE (MSG::WARNING) << " More then one RDO per identifier!";
	   }
	   
	 } // collection
       } // container
    }
  //for each event
  }
  //For each cluster
    std::map< Identifier, const SCT_RDORawData*>::iterator mIt2 = m_globalmapOfRDOsPerModule[clusterModuleID].begin();
    for( ; mIt2!=m_globalmapOfRDOsPerModule[clusterModuleID].end(); mIt2++){
      Identifier rdoIdentity = mIt2->first;
      std::vector<Identifier>::iterator posIt =   find(sctClusterIdentifier.begin(),sctClusterIdentifier.end(),rdoIdentity);
      if ( posIt != sctClusterIdentifier.end() ){
	m_sctRDOForAssociation.push_back(mIt2->second);
      }
    }
    // end of global map loop 


  m_rdoItr = m_sctRDOForAssociation.begin(); //sct::vector<cont InDet::SCT_Cluster>::const_iterator 
  m_rdoEnd = m_sctRDOForAssociation.end(); 


  return StatusCode::SUCCESS;
}

/**
* @brief Return a pointer to the next element in the association.
*
* Return 0 when the association has been exhausted.
*/
const SCT_RDORawData* SCTClusterSCTRDOAssociationTool::next()
  {

    // reached the end
    if(m_rdoItr==m_rdoEnd){
      return 0;
    }
    else{
    }

    const SCT_RDORawData* RawData = *m_rdoItr;
       
    m_rdoItr++; // to go throuhg all the HitsOnTrack

    return RawData;

  }

  StatusCode SCTClusterSCTRDOAssociationTool::tempmoduleid( Identifier id, int& intbuf ){

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
    if(!(from_string<int>(intbuf, std::string(buf), std::dec))){
      //if ( EOF == sscanf(buf, "%i", &intbuf) ){
      REPORT_MESSAGE (MSG::ERROR) <<" Cannot convert string to int...";
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

 } // namespace D3PD
  
