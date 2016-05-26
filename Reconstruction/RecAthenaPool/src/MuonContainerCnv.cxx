///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv.cxx 
// Implementation file for class MuonContainerCnv
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecAthenaPool includes
#include "MuonContainerCnv.h"
#include "muonEvent/MuonCaloEnergyContainer.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode MuonContainerCnv::initialize()
{

  MsgStream log(messageService(), "MuonContainerCnv");
  StatusCode sc = T_AthenaPoolCustomCnv<Analysis::MuonContainer,MuonContainer_PERS>::initialize();
  if (!sc.isSuccess()) {
     log << MSG::ERROR << "Can not initialize Converter base class." << endreq;
     return(sc);
  }

  // retrieve the storegate service
  if ( !m_storeGate.retrieve().isSuccess() ) {
     log << MSG::ERROR
	 << "Unable to retrieve pointer to StoreGateSvc"
	 << endreq;
     return StatusCode::FAILURE;
  }

  m_cnv_p1.setEventStore( m_storeGate );
  m_cnv_p1.setKey( m_muonCaloEnergyContainerName );
  m_cnv_p2.setEventStore( m_storeGate );
  m_cnv_p2.setKey( m_muonCaloEnergyContainerName );

  return StatusCode::SUCCESS;
}

MuonContainer_PERS* 
MuonContainerCnv::createPersistent( Analysis::MuonContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "MuonContainerCnv" );

  MuonContainerCnv_p6 cnv;
  MuonContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

Analysis::MuonContainer* MuonContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "MuonContainerConverter" );

  Analysis::MuonContainer *transObj = 0;

  static pool::Guid tr_guid("E5B86159-2270-4770-A412-078D09F12215");
  static pool::Guid p1_guid("2B37C58D-4FBF-46F3-AB5C-B1408DBB99BE");
  static pool::Guid p2_guid("AB37C580-4FBF-51F3-F85D-014F8DBBE9BC");
  static pool::Guid p3_guid("FB88C580-EFBF-5CDE-EEED-AABF8DBCE9EE");
  static pool::Guid p4_guid("01887544-EFBF-E099-1116-0051F2678B09");
  static pool::Guid p5_guid("45887564-EABA-EBC9-DE1A-2459F21780CC");
  static pool::Guid p6_guid("56E6FC8D-E6B3-4FFB-83A2-A9AEF6E35FF3");

  if ( compareClassGuid(p6_guid) ) {

    std::unique_ptr<MuonContainer_p6> persObj(poolReadObject<MuonContainer_p6>());
    transObj = m_cnv_p6.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p5_guid) ) {

    std::unique_ptr<MuonContainer_p5> persObj(poolReadObject<MuonContainer_p5>());
    transObj = m_cnv_p5.createTransient( persObj.get(), msg );

  }  else if ( compareClassGuid(p4_guid) ) {

    std::unique_ptr<MuonContainer_p4> persObj(poolReadObject<MuonContainer_p4>());
    transObj = m_cnv_p4.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p3_guid) ) {

    std::unique_ptr<MuonContainer_p3> persObj(poolReadObject<MuonContainer_p3>());
    transObj = m_cnv_p3.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p2_guid) ) {

    if ( !m_storeGate->contains<MuonCaloEnergyContainer>( m_muonCaloEnergyContainerName ) ) {
       MuonCaloEnergyContainer * cont = new MuonCaloEnergyContainer();
       if ( m_storeGate->record( cont, m_muonCaloEnergyContainerName ).isFailure() ) {
          msg << MSG::WARNING << "Not able to recod MuonCaloEnergyContainer in StoreGate " << endreq;
       }
    }
 
    std::unique_ptr<MuonContainer_p2> persObj( poolReadObject<MuonContainer_p2>() );
    transObj = m_cnv_p2.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p1_guid) ) {

    if ( !m_storeGate->contains<MuonCaloEnergyContainer>( m_muonCaloEnergyContainerName ) ) {
       MuonCaloEnergyContainer * cont = new MuonCaloEnergyContainer();
       if ( m_storeGate->record( cont, m_muonCaloEnergyContainerName ).isFailure() ) {
          msg << MSG::WARNING << "Not able to recod MuonCaloEnergyContainer in StoreGate " << endreq;
       }
    }

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MuonContainer_p1> persObj( poolReadObject<MuonContainer_p1>() );
    transObj = m_cnv_p1.createTransient( persObj.get(), msg );

 } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<Analysis::MuonContainer>();

  } else {
    throw std::runtime_error("Unsupported persistent version of MuonContainer");
  }

  return transObj;
}
