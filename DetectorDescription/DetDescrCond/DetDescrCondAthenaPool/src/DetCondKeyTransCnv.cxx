/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DetCondKeyTransCnv.cxx
 * @brief Implementation file for AthenaPool converter for DetCondKeyTrans
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "DetCondKeyTransCnv.h"
#include "DetDescrCondTPCnv/DetCondKeyTransCnv_p1.h"

DetCondKeyTransCnv::DetCondKeyTransCnv(ISvcLocator* svcloc) 
:       
        DetCondKeyTransCnvBase(svcloc) 
{}


DetCondKeyTrans_PERS* DetCondKeyTransCnv::createPersistent(DetCondKeyTrans* transObj) {
    MsgStream log(msgSvc(), "DetCondKeyTransCnv" ); 
    DetCondKeyTransCnv_p1 cnv;
    DetCondKeyTrans_PERS* persObj = cnv.createPersistent( transObj, log );
    log << MSG::DEBUG << "DetCondKeyTrans write Success" << endmsg;
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetCondKeyTransCnv write p1" << endmsg;
    return persObj; 
}

DetCondKeyTrans* DetCondKeyTransCnv::createTransient() {
    MsgStream log(msgSvc(), "DetCondKeyTransCnv" );
    static pool::Guid   p1_guid("0FBC4B54-5F6B-4A5B-A1C7-B7EB5DD124E5");
    static pool::Guid   p0_guid("A6DA54AC-D54E-4B6C-8489-C6D015277AD0");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetCondKeyTransCnv read p1" << endmsg;
        std::auto_ptr< DetCondKeyTrans_p1 > col_vect( poolReadObject< DetCondKeyTrans_p1 >() );
        DetCondKeyTransCnv_p1 cnv;
        return cnv.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // regular object from before TP separation, just return it
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetCondKeyTransCnv read p0" << endmsg;
        return this->poolReadObject<DetCondKeyTrans>();
    } 
    throw std::runtime_error("Unsupported persistent version of DetCondKeyTrans");
}
