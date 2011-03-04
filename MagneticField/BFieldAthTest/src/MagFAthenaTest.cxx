/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  15/06/00
//
// The MagFAthenaTest class
// is an Algorithm class to test MagFieldAthenaSvc
//
#include "BFieldAth/IMagFieldAthenaSvc.h"
#include "BFieldAthTest/MagFAthenaTest.h"
#include "BFieldAthTest/MagFAthenaTest03.h"

MagFAthenaTest::MagFAthenaTest(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  p_MagFieldAthenaSvc("MagFieldAthenaSvc", name)
{

//Sub Algs
  declareProperty("MenbersName",m_MenbersName);
  declareProperty("MenbersType",m_MenbersType);
  declareProperty("MagFieldSvc", p_MagFieldAthenaSvc, "Magnet Field used by this algorithm") ;

}
MagFAthenaTest::~MagFAthenaTest(){}
 
StatusCode MagFAthenaTest::initialize()
{
  StatusCode sc ;
  MsgStream mlog(messageService(), name());

  mlog << MSG::INFO << "================================" << endreq;
  mlog << MSG::INFO << "=Proprieties are     " << endreq;
  mlog << MSG::INFO << "= Menbers:              " << endreq;
  for (unsigned int iAlgo=0; iAlgo<m_MenbersName.size() ; iAlgo++){
    mlog << MSG::INFO << " of Type=" << m_MenbersType[iAlgo] << " of Name=" << m_MenbersName[iAlgo]
                      << endreq;
  }
  mlog << MSG::INFO << "================================" << endreq;

//Set pointer on MagFieldAthenaSvc
 if (p_MagFieldAthenaSvc.retrieve().isFailure()){
    mlog << MSG::WARNING   << "Could not find MagFieldAthenaSvc" << endreq;
    return( StatusCode::FAILURE );
  }

// Setup callback to be notified when magnetic field is ready
//   if (StatusCode::SUCCESS==p_MagFieldAthenaSvc->regFcn(
//           &MagFAthenaTest::callback,this)) {
//   	 mlog << MSG::INFO << "Booked callback for " << name() << endreq;
//    } else {
//       mlog << MSG::ERROR << "Could not book callback for " << name () << endreq;
//       return StatusCode::FAILURE;
//    }


//Access Basic Muon Svc - now via callback
  // SetAccessBasicSvc();

  for (unsigned int iAlgo=0; iAlgo<m_MenbersName.size() ; iAlgo++){
    Algorithm* AlgoPointer;
    sc = createSubAlgorithm(m_MenbersType[iAlgo],m_MenbersName[iAlgo] ,AlgoPointer);
    if (sc.isFailure()){
      mlog << MSG::FATAL << "Unable to create SubAlgorithm" 
                         << " of Type=" << m_MenbersType[iAlgo] << " of Name=" << m_MenbersName[iAlgo]
                         << endreq;
      return StatusCode::FAILURE;
    }
    m_ListOfMenbers.push_back(AlgoPointer);
  }

  mlog << MSG::INFO << "Initialisation ended     " << endreq;
  return StatusCode::SUCCESS;

}
 
StatusCode MagFAthenaTest::finalize(){return StatusCode::SUCCESS;}

StatusCode MagFAthenaTest::execute()
{

  MsgStream mlog( messageService(), name() );
  StatusCode sc;
  
//Access Basic Muon Svc
// this is now done from the callback function
  // SetAccessBasicSvc();

//Loop on SubAlgs
  const std::size_t NberOfAlgo = m_ListOfMenbers.size();
  for ( std::size_t iAlgo=0; iAlgo<NberOfAlgo; iAlgo++ ) {
    sc = m_ListOfMenbers[iAlgo]->sysExecute();
    if (sc.isFailure()){
      mlog << MSG::FATAL << "execute () of SubAlgo " 
                         << " of Type=" << m_MenbersType[iAlgo] << " of Name=" << m_MenbersName[iAlgo]
                         << " failed "
                         << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;

}

// Set Access to Basic Muon Svc
// void MagFAthenaTest::SetAccessBasicSvc(){

// //*Access MagFieldAthena
//    MagFieldAthena* pMagFieldAthena=p_MagFieldAthenaSvc->GetMagFieldAthena();
//    MagFieldAccess::SetAbstractMagneticField(pMagFieldAthena);
//}

// StatusCode MagFAthenaTest::callback( IOVSVC_CALLBACK_ARGS ) {
//   MsgStream mlog( messageService(), name() );
//   mlog << MSG::INFO << "MagFAthenaTest callback was called!" << endreq;
//   // get the field pointer and do setup
//   MagFieldAthena* pMagFieldAthena=
//     p_MagFieldAthenaSvc->GetUpdatedMagFieldAthena();
//    MagFieldAccess::SetAbstractMagneticField(pMagFieldAthena);
//   return StatusCode::SUCCESS;
// }


