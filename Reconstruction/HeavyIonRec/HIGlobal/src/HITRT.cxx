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
  AthAlgorithm(name,pSvcLocator),
  m_isData(false),
  m_initialized(false),
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
  m_initialized = true;
  ATH_CHECK( m_sumSvc.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode  HITRT::finalize()
{
  // Code entered here will be executed once at the end of the program run
  // m_log << MSG::DEBUG << name() << " finalize()" << endreq;
  return StatusCode::SUCCESS;
}

//================ Execution =================================================

StatusCode HITRT::FillTRTNumber(vector<int> &occupancyPerPart ) {
  occupancyPerPart.resize(6);
  
    const TRT_ID *TRTHelper = nullptr;
    ATH_CHECK( detStore()->retrieve(TRTHelper,"TRT_ID") );
    
    int n(0);
    int n_ds(0);
    int n_b(0);
    int n_ec(0);
    int n_ec_A(0);
    int n_ec_C(0);
    const TRT_RDO_Container* ev = nullptr; // container of all TRT hits
    ATH_CHECK( evtStore()->retrieve(ev, "TRT_RDOs") );

    for (TRT_RDO_Container::const_iterator rdoIt = ev->begin(); rdoIt != ev->end(); rdoIt++) {
      const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
      if (TRTCollection==0) continue;
      for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) {
	
	Identifier id = (*trtIt)->identify();
	int side=TRTHelper->barrel_ec(id); // barrel, endcap
	
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

    
    occupancyPerPart[0]=n;
    occupancyPerPart[1]=n_ds;
    occupancyPerPart[2]=n_b;
    occupancyPerPart[3]=n_ec;
    occupancyPerPart[4]=n_ec_A;
    occupancyPerPart[5]=n_ec_C;
    
    // }
  return StatusCode::SUCCESS;
}


StatusCode  HITRT::execute()
{
  if( !m_initialized) 
    return StatusCode::FAILURE;

  std::vector<int> occupancyPerPart;
  ATH_CHECK( FillTRTNumber(occupancyPerPart) );
  HITRTInfo *hcd = new HITRTInfo(occupancyPerPart);
  ATH_MSG_WARNING( "Shulga hcd->GetAllStraws() = "<< hcd->GetAllStraws() );
  ATH_CHECK( evtStore()->record( hcd,"HITRT" ) );

  return StatusCode::SUCCESS;
}

//============================================================================
