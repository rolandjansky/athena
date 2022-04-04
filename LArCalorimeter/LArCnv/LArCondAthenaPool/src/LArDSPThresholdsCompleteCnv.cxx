/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDSPThresholdsCompleteCnv.cxx
 * @brief AthenaPool converter LArDSPThresholdsComplete
 * @author Walter Lampl (@cern.ch)
 */

#include "LArDSPThresholdsCompleteCnv.h"
#include "LArCondTPCnv/LArDSPThresholdsSubsetCnv_p1.h"

static const LArDSPThresholdsSubsetCnv_p1   TPconverter;

LArDSPThresholdsSubset_p1*
LArDSPThresholdsCompleteCnv::createPersistent (LArDSPThrTransType* transObj) {
    MsgStream log(msgSvc(), "LArDSPThresholdsCompleteCnv" ); 
    //log << MSG::DEBUG << "LArDSPThresholdsComplete write" << endmsg;
    LArDSPThrPersType* persObj = TPconverter.createPersistentConst( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArDSPThresholdsP>*
LArDSPThresholdsCompleteCnv::createTransient () {
    static const pool::Guid   p0_guid("09607438-09CC-4E40-A1E2-23F0B021DF3D");
    if( compareClassGuid(p0_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArDSPThresholdsSubset_p1 > col_vect( poolReadObject< LArDSPThresholdsSubset_p1 >() );
        MsgStream log(msgSvc(), "LArDSPThresholdsCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArDSPThresholdsSubset_p1" << endmsg; 
        return TPconverter.createTransientConst( col_vect.get(), log );
    }
    throw std::runtime_error("Unsupported persistent version of LArDSPThresholdsCompleteCnv");
}
