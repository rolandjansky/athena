/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AlignableTransformCnv.cxx
 * @brief Implementation file for AthenaPool converter for AlignableTransform
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "AlignableTransformCnv.h"
#include "DetDescrCondTPCnv/AlignableTransformCnv_p1.h"

AlignableTransformCnv::AlignableTransformCnv(ISvcLocator* svcloc) 
:       
        AlignableTransformCnvBase(svcloc) 
{}


AlignableTransform_PERS* AlignableTransformCnv::createPersistent(AlignableTransform* transObj) {
    MsgStream log(msgSvc(), "AlignableTransformCnv" ); 
    AlignableTransformCnv_p1 cnv;
    AlignableTransform_PERS* persObj = cnv.createPersistent( transObj, log );
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "AlignableTransform write Success" << endmsg;
    return persObj; 
}

AlignableTransform* AlignableTransformCnv::createTransient() {
    MsgStream log(msgSvc(), "AlignableTransformCnv" );
    static pool::Guid   p1_guid("BA1A841C-8D92-45AE-9AD1-9AF7A1736844");
    static pool::Guid   p0_guid("E779C6B5-3F2A-473E-B35E-6CCB345E0665");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "AlignableTransform read p1" << endmsg;
        std::auto_ptr< AlignableTransform_p1 > col_vect( poolReadObject< AlignableTransform_p1 >() );
        AlignableTransformCnv_p1 cnv;
        return cnv.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // regular object from before TP separation, just return it
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "AlignableTransform read p0" << endmsg;
        return this->poolReadObject<AlignableTransform>();
    } 
    throw std::runtime_error("Unsupported persistent version of AlignableTransform");
}


StatusCode AlignableTransformCnv::initialize()
{
    return AlignableTransformCnvBase::initialize();
}
