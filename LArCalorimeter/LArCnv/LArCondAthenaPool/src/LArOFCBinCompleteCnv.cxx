/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArOFCBinCompleteCnv.cxx
 * @brief AthenaPool converter LArOFCBinComplete
 * @author Walter Lampl <walter.lampl@cern.ch>
 */

#include "LArOFCBinCompleteCnv.h"

LArOFCBinPersType* LArOFCBinCompleteCnv::createPersistent (LArOFCBinTransType* transObj) {
    MsgStream log(msgSvc(), "LArOFCBinCompleteCnv" ); 
    //log << MSG::DEBUG << "LArOFCBinComplete write" << endmsg;
    LArOFCBinPersType* persObj = m_TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArOFCBinTransType* LArOFCBinCompleteCnv::createTransient () {
  static const pool::Guid p1_guid("E7E2E993-4AF2-4782-9B8B-7668434CBAEC");
  if( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<LArOFCBinPersType> col_vect( poolReadObject<LArOFCBinPersType>() );
    MsgStream log(msgSvc(), "LArOFCBinCompleteCnv" ); 
    //log << MSG::INFO << "Reading LArOFCBinSubset_p1" << endmsg; 
    return m_TPconverter.createTransient( col_vect.get(), log );
  }
  throw std::runtime_error("Unsupported persistent version of LArOFCBinComplete");
}

    
