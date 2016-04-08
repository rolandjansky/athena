/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p1.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p2.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p3.h"
#include "CaloCalibrationHitContainerCnv.h"
#include "CaloSimEventTPCnv/CaloCalibrationHit_p1.h"
#include "CaloSimEventTPCnv/CaloCalibrationHit_p2.h"


CaloCalibrationHitContainer_PERS* CaloCalibrationHitContainerCnv::createPersistent(CaloCalibrationHitContainer* transCont) {
    MsgStream mlog(messageService(), "CaloCalibrationHitContainerConverter" );
    CaloCalibrationHitContainerCnv_p3   converter;
    CaloCalibrationHitContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}


CaloCalibrationHitContainer* CaloCalibrationHitContainerCnv::createTransient() {
    MsgStream mlog(messageService(), "CaloCalibrationHitContainerConverter" );
    CaloCalibrationHitContainerCnv_p1   converter_p1;
    CaloCalibrationHitContainerCnv_p2   converter_p2;
    CaloCalibrationHitContainerCnv_p3   converter_p3;

    CaloCalibrationHitContainer       *trans_cont(0);
    static const pool::Guid  p1_guid("185FEF89-2350-4D1E-A1A7-9CB5A357232E");
    static const pool::Guid  p2_guid("4942B9D4-F545-4C68-BD2D-A8E5923C29F3");
    static const pool::Guid  p3_guid("D4E51325-2A55-41E4-B163-5224F2FA27CB");
    static const pool::Guid  p0_guid("33CDAED0-F472-47D2-8F28-27C6D6761F35");

    if( this->compareClassGuid(p3_guid)) {
      std::auto_ptr< CaloCalibrationHitContainer_p3 >   col_vect( this->poolReadObject< CaloCalibrationHitContainer_p3 >() );
      trans_cont = converter_p3.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p2_guid)) {
      std::auto_ptr< CaloCalibrationHitContainer_p2 >   col_vect( this->poolReadObject< CaloCalibrationHitContainer_p2 >() );
      trans_cont = converter_p2.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< CaloCalibrationHitContainer_p1 >   col_vect( this->poolReadObject< CaloCalibrationHitContainer_p1 >() );
        trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p0_guid)) {
        // old version from before TP separation, just return it
        trans_cont = this->poolReadObject<CaloCalibrationHitContainer>();
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
