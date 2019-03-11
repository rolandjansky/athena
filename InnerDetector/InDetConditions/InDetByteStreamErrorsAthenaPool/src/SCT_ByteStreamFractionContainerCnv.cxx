/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamFractionContainerCnv.cxx
 * @brief Generated implementation file which includes header files needed by SCT_ByteStreamFractionContainerCnv
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 */

#include "SCT_ByteStreamFractionContainerCnv.h"

SCT_ByteStreamFractionContainer_PERS*
SCT_ByteStreamFractionContainerCnv::createPersistent(SCT_ByteStreamFractionContainer* transCont) {
  MsgStream log(msgSvc(), "SCT_ByteStreamFractionContainerCnv");
  SCT_ByteStreamFractionContainer_PERS *persObj = m_TPConverter.createPersistent(transCont, log);
  return persObj; 
}

SCT_ByteStreamFractionContainer*
SCT_ByteStreamFractionContainerCnv::createTransient() {
  MsgStream log(msgSvc(), "SCT_ByteStreamFractionContainerCnv" );
  const pool::Guid p1_guid("EB75984C-F651-4F40-BA1C-9C2A0A558A55");
  if( compareClassGuid(p1_guid) ) {
    /** using auto_ptr ensures deletion of the persistent object */
    std::auto_ptr< SCT_ByteStreamFractionContainer_p1 > col_vect( poolReadObject< SCT_ByteStreamFractionContainer_p1 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
