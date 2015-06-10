/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEventTPCnv/LArHitContainerCnv_p1.h"
#include "LArSimEventTPCnv/LArHitContainerCnv_p2.h"
#include "LArHitContainerCnv.h"
#include "LArSimEventTPCnv/LArHit_p1.h"

LArHitContainer_PERS* LArHitContainerCnv::createPersistent(LArHitContainer* transCont) {
    MsgStream mlog(messageService(), "LArHitContainerConverter" );
    LArHitContainerCnv_p2   converter;
    LArHitContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}


LArHitContainer* LArHitContainerCnv::createTransient() {
    MsgStream mlog(messageService(), "LArHitContainerConverter" );
    LArHitContainerCnv_p1   converter_p1;
    LArHitContainerCnv_p2   converter_p2;

    LArHitContainer       *trans_cont(0);

    static const pool::Guid   p2_guid("1F1DE705-E0CE-4F0E-941A-C405CB2CD137");
    static const pool::Guid   p1_guid("ED1ECB80-B38C-46DE-94BF-22F9379796DB");
    static const pool::Guid   p0_guid("32703AED-CAA5-45ED-B804-8556900CA6B5");

    if( this->compareClassGuid(p2_guid)) {
        std::auto_ptr< LArHitContainer_p2 >   col_vect( this->poolReadObject< LArHitContainer_p2 >() );
        trans_cont = converter_p2.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< LArHitContainer_p1 >   col_vect( this->poolReadObject< LArHitContainer_p1 >() );
        trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p0_guid)) {
        // old version from before TP separation, just return it
        trans_cont = this->poolReadObject<LArHitContainer>();
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
