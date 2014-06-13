/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HITRT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HITRT.h"
#include "HIGlobal/HITRTInfo.h"
#include "GeneratorObjects/HijingEventParams.h"

#include "StoreGate/StoreGateSvc.h"
#include "DetDescrCondTools/ICoolHistSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include "CaloEvent/CaloCellContainer.h"
//#include "InDetPrepRawData/SiClusterContainer.h"

#include "TMath.h"
#include "TH1F.h"

// EDM include(s):
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include "InDetRawData/TRT_RDO_Container.h"

#include "InDetIdentifier/TRT_ID.h"

// Local include(s):
#include <vector>
#include <iostream>
#include <sstream>

using std::vector;
using std::cout; using std::endl;

HITRT::HITRT(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_sumSvc("TRT_StrawStatusSummarySvc",name)
{
    declareProperty("InDetTRTStrawStatusSummarySvc",m_sumSvc);

}

 HITRT::~HITRT()
{
  // destructor
  // deletes histograms to avoid memory leak

  //delete m_calibfile;
  //delete m_b_perch;
  //delete m_nw_perch;
  //delete m_ncoll_perch;
  //delete m_calocell_energy_perch;
  //delete m_number_of_siclusters_perch;
}


StatusCode HITRT::initialize()
{
    // Call the initialization of the base class:

  MsgStream log(msgSvc(),name());

  m_initialized = true;

  // retrieve the StoreGate Service (delete if not needed)
  StatusCode sc= service("StoreGateSvc",m_sgSvc);
  sc = m_sumSvc.retrieve();

  //  return StatusCode::SUCCESS;
  return sc;
}


StatusCode  HITRT::finalize()
{
  // Code entered here will be executed once at the end of the program run
  // m_log << MSG::DEBUG << name() << " finalize()" << endreq;
  return StatusCode::SUCCESS;
}

//================ Execution =================================================

StatusCode HITRT::FillTRTNumber(vector<int> &m_OccupancyPerPart ) {
  //if( m_writeBasicInfo ) {
  MsgStream log(msgSvc(),name());
  
  StatusCode sc;
  //const EventInfo *eventInfo = 0;
  //StatusCode sc = evtStore()->retrieve(eventInfo);

  m_OccupancyPerPart.resize(6);
  
  detStore = 0;
  sc = service("DetectorStore",detStore);
  //evtStore = 0;
  //sc = service("EventStore",evtStore);
  //sc = evtStore().retrieve();

    const TRT_ID *m_TRTHelper;
    detStore->retrieve(m_TRTHelper,"TRT_ID");
    
    int n(0);
    int n_ds(0);
    int n_b(0);
    int n_ec(0);
    int n_ec_A(0);
    int n_ec_C(0);
    const TRT_RDO_Container* ev; // container of all TRT hits
    //sc = evtStore()->retrieve(ev, "TRT_RDOs");
    sc = m_sgSvc->retrieve(ev, "TRT_RDOs");
    if ( sc.isFailure() ) {
      log << (MSG::ERROR) << "no TRT_RDO container available " << endreq;
      return sc;
    }

    for (TRT_RDO_Container::const_iterator rdoIt = ev->begin(); rdoIt != ev->end(); rdoIt++) {
      const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
      if (TRTCollection==0) continue;
      for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) {
	
	Identifier id = (*trtIt)->identify();
	int side=m_TRTHelper->barrel_ec(id); // barrel, endcap
	
	if (m_sumSvc->get_status(id)){ 
	  n_ds++; 
	  continue;
	}
	
	n++;
	
	if (abs(side)==1) { // barrel
	  n_b++;
	}else if (abs(side)==2) { // endcap
	  n_ec++;
	  if(side==2)n_ec_A++;//2 endcap A
	  if(side==-2)n_ec_C++;//2 endcap C
	}
      }
    }


    /*
    m_OccupancyPerPart.push_back(n);
    m_OccupancyPerPart.push_back(n_ds);
    m_OccupancyPerPart.push_back(n_b);
    m_OccupancyPerPart.push_back(n_ec);
    m_OccupancyPerPart.push_back(n_ec_A);
    m_OccupancyPerPart.push_back(n_ec_C);
    */
    
    m_OccupancyPerPart[0]=n;
    m_OccupancyPerPart[1]=n_ds;
    m_OccupancyPerPart[2]=n_b;
    m_OccupancyPerPart[3]=n_ec;
    m_OccupancyPerPart[4]=n_ec_A;
    m_OccupancyPerPart[5]=n_ec_C;
    
    // }
  return StatusCode::SUCCESS;
}


StatusCode  HITRT::execute()
{
  // Creates HITRTInfo object and saves it in SG
  //cout << " HITRT::execute() " << endl;
  MsgStream log(msgSvc(),name());

  if( !m_initialized) 
    return StatusCode::FAILURE;

  StatusCode sc;
  
  HITRTInfo *hcd;

  sc=FillTRTNumber(m_OccupancyPerPart);

  hcd = new HITRTInfo(m_OccupancyPerPart);

  //Put HITRT to Store Gate
  log << (MSG::WARNING) << "Shulga hcd->GetAllStraws() = "<< hcd->GetAllStraws() << endreq;

  sc = m_sgSvc->record( hcd,"HITRT" );


  return sc;
}

//============================================================================================
