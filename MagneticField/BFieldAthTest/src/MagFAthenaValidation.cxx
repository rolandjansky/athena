/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  30/03/00
//
// The MagFAthenaValidation class
// is an Algorithm class to test MagFieldAthenaSvc
//
#include "BFieldAth/IMagFieldAthenaSvc.h"
#include "BFieldAthTest/MagFAthenaValidation.h"
 
MagFAthenaValidation::MagFAthenaValidation(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  p_MagFieldAthenaSvc("MagFieldAthenaSvc", name)
{

  declareProperty("MagFieldSvc", p_MagFieldAthenaSvc, "Magnet Field used by this algorithm") ;
  DoneDumpy01 = 0 ;
}
 
// Initialize
StatusCode MagFAthenaValidation::initialize(){

  StatusCode sc ;
  MsgStream mlog(messageService(), name());
//Set pointer on MagFieldAthenaSvc
 if (p_MagFieldAthenaSvc.retrieve().isFailure()){
    mlog << MSG::WARNING   << "Could not find MagFieldAthenaSvc" << endreq;
    return( StatusCode::FAILURE );
  }

  mlog << MSG::INFO << "Initialisation ended     " << endreq;
  return StatusCode::SUCCESS;
  
}
 
// Execute
StatusCode MagFAthenaValidation::execute(){

  MsgStream mlog(messageService(), name());
  mlog << MSG::INFO << "in execute()" << endreq;

  if ( DoneDumpy01==0 ) {
    DoneDumpy01 = 1 ;
    Dumpy01();
  }
  
  return StatusCode::SUCCESS;
  
}				
 
// Finalize
StatusCode MagFAthenaValidation::finalize(){

  MsgStream mlog(messageService(), name());
  mlog << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;

}
 
// Do Dumpy01
void MagFAthenaValidation::Dumpy01(){

  //*Access MagFieldAthena
  MagFieldAthena* pMagField= p_MagFieldAthenaSvc->GetMagFieldAthena();
  float XYZ[3]   ;
  float BGXYZ[12] ;
  XYZ[0] = 700 ;
  XYZ[1] = 600 ;
  XYZ[2] = 500 ;
  pMagField->field(XYZ,BGXYZ);
  MsgStream mlog(messageService(), name());
  mlog << MSG::INFO << " field in 500 500 500 " << BGXYZ << endreq;
  //
// notpossible need to be changed    FastField();
  //
}
