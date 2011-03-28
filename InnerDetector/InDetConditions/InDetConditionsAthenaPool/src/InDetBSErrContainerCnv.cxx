/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBSErrContainerCnv.cxx
 * @brief Generated implementation file which includes header files needed by InDetBSErrContainerCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "InDetBSErrContainerCnv.h"

InDetBSErrContainer_PERS* 
InDetBSErrContainerCnv::createPersistent(InDetBSErrContainer* transCont) {
  MsgStream log(messageService(), "InDetBSErrContainerCnv" );
  InDetBSErrContainer_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
  return persObj; 
}

InDetBSErrContainer* 
InDetBSErrContainerCnv::createTransient() {
  MsgStream log(messageService(), "InDetBSErrContainerCnv" );
  static pool::Guid   p1_guid("FA64DC17-D07E-4305-9B21-18C64F1B4C47");
  if( compareClassGuid(p1_guid) ) {
    /** using auto_ptr ensures deletion of the persistent object */
    std::auto_ptr< InDetBSErrContainer_p1 > col_vect( poolReadObject< InDetBSErrContainer_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
