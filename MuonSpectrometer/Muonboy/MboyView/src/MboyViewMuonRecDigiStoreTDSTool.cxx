/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewMuonRecDigiStoreTDSTool.h"

#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

static const InterfaceID IID_IMboyViewMuonRecDigiStoreTDSTool("MboyViewMuonRecDigiStoreTDSTool", 1, 0);

const InterfaceID& MboyViewMuonRecDigiStoreTDSTool::interfaceID()
{
  return IID_IMboyViewMuonRecDigiStoreTDSTool;
}

MboyViewMuonRecDigiStoreTDSTool::MboyViewMuonRecDigiStoreTDSTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewMuonRecDigiStoreTDSTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyViewMuonRecDigiStoreTDSTool::~MboyViewMuonRecDigiStoreTDSTool(){}

// Initialize
StatusCode MboyViewMuonRecDigiStoreTDSTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewMuonRecDigiStoreTDSTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewMuonRecDigiStoreTDSTool::ViewMuonRecDigiStoreTDS(
               std::string MuonRecDigiStoreTDSLocation,
               int ViewAthIds,
               int PrintRSLine,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<MuonRecDigiStoreTDS>(MuonRecDigiStoreTDSLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << MuonRecDigiStoreTDSLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const DataHandle<MuonRecDigiStoreTDS> pStoreDigitsTDS ;
  sc = evtStore()->retrieve(pStoreDigitsTDS,MuonRecDigiStoreTDSLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " pStoreDigitsTDS not found at " << MuonRecDigiStoreTDSLocation ) ;
    }
    return( StatusCode::SUCCESS);
  }
  const MuonRecDigiStoreAth* pStoreDigits = &(pStoreDigitsTDS->m_MuonRecDigiStore);

//Dump
  pStoreDigits->PrintDigitsSums(pOut);
  if (ViewAthIds == 0) pStoreDigits->PrintDigits(pOut);
  if (ViewAthIds == 1) pStoreDigits->PrintDigitsAthIds(pOut);
  if (PrintRSLine == 1) pStoreDigits->PrintDigitsRsLine(pOut);

  return StatusCode::SUCCESS;

}
