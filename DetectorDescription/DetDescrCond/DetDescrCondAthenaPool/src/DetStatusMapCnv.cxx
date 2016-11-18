/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DetStatusMapCnv.cxx
 * @brief Implementation file for AthenaPool converter for DetStatusMap
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "DetStatusMapCnv.h"
#include "DetDescrCondTPCnv/DetStatusMapCnv_p1.h"

DetStatusMapCnv::DetStatusMapCnv(ISvcLocator* svcloc) 
:       
        DetStatusMapCnvBase(svcloc) 
{}


DetStatusMap_PERS* DetStatusMapCnv::createPersistent(DetStatusMap* transObj) {
    MsgStream log(msgSvc(), "DetStatusMapCnv" ); 
    DetStatusMapCnv_p1 cnv;
    DetStatusMap_PERS* persObj = cnv.createPersistent( transObj, log );
    log << MSG::DEBUG << "DetStatusMap write Success" << endmsg;
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetStatusMapCnv write p1" << endmsg;
    return persObj; 
}

DetStatusMap* DetStatusMapCnv::createTransient() {
    MsgStream log(msgSvc(), "DetStatusMapCnv" );
    static pool::Guid   p1_guid("AD99EE0F-DFD9-4734-8930-F5FC702C0BE8");
    static pool::Guid   p0_guid("CB1BFF00-64B1-4693-8035-19BB18867AF1");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetStatusMapCnv read p1" << endmsg;
        std::auto_ptr< DetStatusMap_p1 > col_vect( poolReadObject< DetStatusMap_p1 >() );
        DetStatusMapCnv_p1 cnv;
        return cnv.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // regular object from before TP separation, just return it
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "DetStatusMapCnv read p0" << endmsg;
        return this->poolReadObject<DetStatusMap>();
    } 
    throw std::runtime_error("Unsupported persistent version of DetStatusMap");
}

