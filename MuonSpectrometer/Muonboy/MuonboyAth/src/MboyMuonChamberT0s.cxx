/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonChamberT0s/ChamberT0s.h"

/////////////////////////////////////////////////////////
#include "MuonboyAth/MboyMuonChamberT0s.h"

#include "MuonRecToolInterfaces/IAdjustableT0Tool.h"
 
MboyMuonChamberT0s::MboyMuonChamberT0s(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
 p_IAdjustableT0Tool ( "AdjT0::AdjustableT0Tool" ) 
{
  p_MdtIdHelper = 0 ;
  
  declareProperty( "DoDbg"  , m_DoDbg = 0 ) ;

  declareProperty( "MuonChamberT0sLocation"  , m_MuonChamberT0sLocation = "MboyMuonChamberT0s" ) ;

  declareProperty("IAdjustableT0Tool" , p_IAdjustableT0Tool ) ;


}
MboyMuonChamberT0s::~MboyMuonChamberT0s(){}


// Initialize
StatusCode MboyMuonChamberT0s::initialize(){

  ATH_MSG_INFO( "Initialisation started	  " );


  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are        " );

  ATH_MSG_INFO( "= DoDbg                   " << m_DoDbg  );
  ATH_MSG_INFO( "= MuonChamberT0sLocation  " << m_MuonChamberT0sLocation  );

//Retrieve p_MdtIdHelper
  StatusCode sc = detStore()->retrieve(p_MdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve p_MdtIdHelper" );
    return( StatusCode::FAILURE );
  }

//Retrieve p_IAdjustableT0Tool
  if ( p_IAdjustableT0Tool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IAdjustableT0Tool );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "================================" );

  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

// Execute
StatusCode MboyMuonChamberT0s::execute(){


  if ( m_DoDbg == 1){
    ATH_MSG_INFO( "===============================================" );
    ATH_MSG_INFO( "= FILL " );
  }

  p_IAdjustableT0Tool->SetEnable();

  m_VectorOfIdentifiers.clear();
  std::vector< std::pair < Identifier, float > > t0s;
  int ItemMax = p_IAdjustableT0Tool->NberOfData();
//  ATH_MSG_INFO( "=ItemMax " << ItemMax );
  for (int Item=0; Item<ItemMax; Item++){

    int    stationName = 0  ;
    int    stationEta  = 0  ;
    int    stationPhi  = 0  ;
    double TShift      = 0. ;
    p_IAdjustableT0Tool->GetData(Item, stationName, stationEta, stationPhi, TShift ) ;

    bool check = true ;
    bool isValid = false ;
    Identifier aIdentifier = p_MdtIdHelper->elementID(stationName, stationEta, stationPhi, check, &isValid );

    if (!isValid){

      ATH_MSG_INFO( "============>p_MdtIdHelper->elementID gets false isValid FOR " );
      ATH_MSG_INFO( "=stationName " << p_MdtIdHelper->stationNameString(stationName)
                    << "             " << stationName
                    << " stationEta  " << stationEta
                    << " stationPhi  " << stationPhi
                    << " TShift      " << TShift    
                    );

    }else{

      
      if ( m_DoDbg == 1){
         ATH_MSG_INFO( " Filling with " << p_MdtIdHelper->stationNameString(stationName)
                      << " " << stationEta
                      << " " << stationPhi
                      << " => T0 = " << TShift    
                      << " ( " << stationName - p_MdtIdHelper-> stationName(aIdentifier) 
                      << " | " << stationEta  - p_MdtIdHelper-> stationEta (aIdentifier) 
                      << " | " << stationPhi  - p_MdtIdHelper-> stationPhi (aIdentifier) 
                      << " ) "
                      );

        m_VectorOfIdentifiers.push_back(aIdentifier);
      }                                  

      t0s.push_back( std::make_pair( aIdentifier, (float) TShift ) );
    }                                  

  }
  
  if ( m_DoDbg == 1){
    ATH_MSG_INFO( "===============================================" );
  }

  
  const Muon::ChamberT0s* pChamberT0s = new Muon::ChamberT0s( t0s );

  if ( m_DoDbg == 1){
    ATH_MSG_INFO( "===============================================" );
    ATH_MSG_INFO( "= CHECK BEFORE STORAGE " );
    DumpIt(pChamberT0s);             
    ATH_MSG_INFO( "===============================================" );
  }
      
  StatusCode sc = evtStore()->record(pChamberT0s, m_MuonChamberT0sLocation);
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot record pChamberT0s at " << m_MuonChamberT0sLocation );
    m_VectorOfIdentifiers.clear();
    return( StatusCode::FAILURE );
  }

  if ( m_DoDbg == 1){
    ATH_MSG_INFO( "===============================================" );
    ATH_MSG_INFO( "= CHECK AFTER STORAGE " );
    DumpItAfterStorage();             
    ATH_MSG_INFO( "===============================================" );
  }

  m_VectorOfIdentifiers.clear();
  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyMuonChamberT0s::finalize(){return StatusCode::SUCCESS;}

void MboyMuonChamberT0s::DumpIt(const Muon::ChamberT0s* pChamberT0s){

  std::vector<Identifier>::const_iterator it = m_VectorOfIdentifiers.begin();
  for ( ; it<m_VectorOfIdentifiers.end(); it++){
    Identifier aIdentifier = *it ;
    ATH_MSG_INFO( " Looking at "
                << " " << p_MdtIdHelper->stationNameString( p_MdtIdHelper-> stationName(aIdentifier) )
                << " " << p_MdtIdHelper-> stationEta (aIdentifier) 
                << " " << p_MdtIdHelper-> stationPhi (aIdentifier) );
                
    if ( pChamberT0s->haveChamber(aIdentifier) ){
      ATH_MSG_INFO( " => T0 = " << pChamberT0s->getT0(aIdentifier) 
                 );
    }else{
      ATH_MSG_INFO( " => UNKNOWN TO THE BATAILLON!!!!! "
                 );
    }

  }

}
   
void MboyMuonChamberT0s::DumpItAfterStorage(){

  const Muon::ChamberT0s* pChamberT0s ;
  StatusCode sc = evtStore()->retrieve(pChamberT0s,m_MuonChamberT0sLocation);
  if (sc.isFailure()) {
    ATH_MSG_INFO( " Collection NOT FOUND at  " << m_MuonChamberT0sLocation
                 );
    return ;
  }

  DumpIt(pChamberT0s);             

}
   
