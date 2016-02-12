/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "RecTPCnv/MuonCaloEnergyContainerCnv_p1.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"
#include "RecTPCnv/CaloEnergyCnv_p2.h"

// RecAthenaPool includes
#include "MuonCaloEnergyContainerCnv.h"



MuonCaloEnergyContainerCnv::MuonCaloEnergyContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv< MuonCaloEnergyContainer, 
			 MuonCaloEnergyContainer_PERS
			 >(svcLocator)
{
  CaloEnergyCnv_p2::registerStreamerConverter();
}

MuonCaloEnergyContainer_PERS* 
MuonCaloEnergyContainerCnv::createPersistent( MuonCaloEnergyContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "MuonCaloEnergyContainerCnv" );

  MuonCaloEnergyContainerCnv_p1 cnv;
  MuonCaloEnergyContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

MuonCaloEnergyContainer* MuonCaloEnergyContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "MuonCaloEnergyContainerConverter" );
  MuonCaloEnergyContainer *transObj = 0;

  static pool::Guid p1_guid("DB3DC5CD-FF0F-01FE-4550-7BCE8DBB9968");
  static pool::Guid tlp1_guid("E60630E0-FAA1-4658-9BDD-8BB977C4200B");
  
  if ( compareClassGuid(tlp1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MuonCaloEnergyContainer_tlp1> persObj( poolReadObject<MuonCaloEnergyContainer_tlp1>() );
    MuonCaloEnergyContainerCnv_tlp1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  }
  else if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MuonCaloEnergyContainer_p1> persObj( poolReadObject<MuonCaloEnergyContainer_p1>() );
    MuonCaloEnergyContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else {
    throw std::runtime_error("Unsupported persistent version of MuonCaloEnergyContainer");
  }

  return transObj;
}
