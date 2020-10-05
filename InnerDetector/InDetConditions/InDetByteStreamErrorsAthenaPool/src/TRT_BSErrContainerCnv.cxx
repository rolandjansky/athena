/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TRT_BSErrContainerCnv.cxx
 * @brief Generated implementation file which includes header files needed by TRT_BSErrContainerCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "TRT_BSErrContainerCnv.h"

#include <iostream>


TRT_BSErrContainer_PERS* 
TRT_BSErrContainerCnv::createPersistent(TRT_BSErrContainer* transCont) {
  MsgStream log(msgSvc(), "TRT_BSErrContainerCnv" );
  TRT_BSErrContainer_PERS *persObj = m_TPConverter.createPersistent( transCont, log );
  return persObj; 
}

TRT_BSErrContainer* 
TRT_BSErrContainerCnv::createTransient() {
  MsgStream log(msgSvc(), "TRT_BSErrContainerCnv" ); 
  static const pool::Guid   p1_guid("D461AC01-02CA-4A9E-886B-24EC14309121");
  static const pool::Guid   p2_guid("A815E78C-BB68-4CA5-9B95-739E9B47043A");
  
  if( compareClassGuid(p2_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr< TRT_BSErrContainer_p2 > col_vect( poolReadObject< TRT_BSErrContainer_p2 >() );
    return m_TPConverter.createTransient( col_vect.get(), log );
  }

  if( compareClassGuid(p1_guid) ) {
    log << MSG::ERROR << "TRT_BSErrContainer_p1 not supported any more. Recording empty container." << endmsg;
    return new TRT_BSErrContainer();
  }
  
  throw std::runtime_error("Unsupported persistent version of Data Collection");
}
