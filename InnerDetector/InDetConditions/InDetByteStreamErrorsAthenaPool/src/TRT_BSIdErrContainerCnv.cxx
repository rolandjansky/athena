/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TRT_BSIdErrContainerCnv.cxx
 * @brief Generated implementation file which includes header files needed by TRT_BSIdErrContainerCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "TRT_BSIdErrContainerCnv.h"

TRT_BSIdErrContainer_PERS* 
TRT_BSIdErrContainerCnv::createPersistent(TRT_BSIdErrContainer* transCont) {
  MsgStream log(msgSvc(), "TRT_BSIdErrContainerCnv" );
  TRT_BSIdErrContainer_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
  return persObj; 
}

TRT_BSIdErrContainer* 
TRT_BSIdErrContainerCnv::createTransient() {
  MsgStream log(msgSvc(), "TRT_BSIdErrContainerCnv" );
  static const pool::Guid   p1_guid("26F44F1E-D1F5-43B3-93E9-09376AB37491");
  if( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< TRT_BSIdErrContainer_p1 > col_vect( poolReadObject< TRT_BSIdErrContainer_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
