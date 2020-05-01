/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "IDCInDetBSErrContainerCnv.h"

IDCBSErrContainer_PERS* 
IDCInDetBSErrContainerCnv::createPersistent(IDCInDetBSErrContainer* transCont) {
  MsgStream log(msgSvc(), "IDCInDetBSErrContainer" );
  IDCBSErrContainer_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
  return persObj; 
}


IDCInDetBSErrContainer* 
IDCInDetBSErrContainerCnv::createTransient() {
  MsgStream log(msgSvc(), "IDC_BSErrContainerCnv" );
  const pool::Guid   p1_guid("84056f02-9d9f-4d0c-8d5a-c6c686f5cf76");
  if( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< InDetBSErrContainer64_p1 > col_vect( poolReadObject< InDetBSErrContainer64_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}


