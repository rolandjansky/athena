/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DetCondCFloatCnv.cxx
 * @brief Implementation file for AthenaPool converter for DetCondCFloat
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "DetCondCFloatCnv.h"
#include "DetDescrCondTPCnv/DetCondCFloatCnv_p1.h"

DetCondCFloatCnv::DetCondCFloatCnv(ISvcLocator* svcloc) 
:       
        DetCondCFloatCnvBase(svcloc) 
{}


DetCondCFloat_PERS* DetCondCFloatCnv::createPersistent(DetCondCFloat* transObj) {
    MsgStream log(msgSvc(), "DetCondCFloatCnv" ); 
    DetCondCFloatCnv_p1 cnv;
    DetCondCFloat_PERS* persObj = cnv.createPersistent( transObj, log );
    log << MSG::DEBUG << "DetCondCFloat write Success" << endmsg;
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetCondCFloatCnv write p1" << endmsg;
    return persObj; 
}

DetCondCFloat* DetCondCFloatCnv::createTransient() {
    MsgStream log(msgSvc(), "DetCondCFloatCnv" );
    static pool::Guid   p1_guid("7E19F4EE-540A-4FB0-AEED-0DC65C1DEFC6");
    static pool::Guid   p0_guid("DCE158E2-27A7-4A78-9778-D3E4D4F740CB");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetCondCFloatCnv read p1" << endmsg;
        std::auto_ptr< DetCondCFloat_p1 > col_vect( poolReadObject< DetCondCFloat_p1 >() );
        DetCondCFloatCnv_p1 cnv;
        return cnv.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // regular object from before TP separation, just return it
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetCondCFloatCnv read p0" << endmsg;
        return this->poolReadObject<DetCondCFloat>();
    } 
    throw std::runtime_error("Unsupported persistent version of DetCondCFloat");
}


StatusCode DetCondCFloatCnv::initialize()
{
    return DetCondCFloatCnvBase::initialize();
}
