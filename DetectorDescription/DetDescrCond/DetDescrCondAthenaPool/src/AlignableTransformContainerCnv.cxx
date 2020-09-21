/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AlignableTransformContainerCnv.cxx
 * @brief Generated implementation file which includes header files needed by AlignableTransformContainerCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "AlignableTransformContainerCnv.h"
#include "DetDescrCondTPCnv/AlignableTransformCnv_p1.h"


AlignableTransform_PERS* AlignableTransformContainerCnv::createPersistent(AlignableTransform_TRANS* transObj) {
    MsgStream log(msgSvc(), "AlignableTransformContainerCnv" ); 
    AlignableTransformCnv_p1 cnv;
    AlignableTransform_PERS* persObj = cnv.createPersistent( transObj, log );
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "AlignableTransformContainer write a new AlignableTransform_PERS " << endmsg;
    return persObj; 
}

AlignableTransform_TRANS* AlignableTransformContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "AlignableTransformContainerCnv" );
    static const pool::Guid   p1_guid("BA1A841C-8D92-45AE-9AD1-9AF7A1736844");
    static const pool::Guid   p0_guid("E779C6B5-3F2A-473E-B35E-6CCB345E0665");
    if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "AlignableTransformContainer read p1" << endmsg;
        std::unique_ptr< AlignableTransform_p1 > col_vect( poolReadObject< AlignableTransform_p1 >() );
        AlignableTransformCnv_p1 cnv;
        return cnv.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // regular object from before TP separation, just return it
        if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "AlignableTransformContainer read p0" << endmsg;
        return this->poolReadObject<AlignableTransform>();
    } 
    throw std::runtime_error("Unsupported persistent version of AlignableTransformContainer");
}


StatusCode AlignableTransformContainerCnv::initialize()
{
    return AlignableTransformContainerCnvBase::initialize();
}

