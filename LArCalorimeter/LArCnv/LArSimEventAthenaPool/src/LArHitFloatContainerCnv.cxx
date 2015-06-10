/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEventTPCnv/LArHitFloatContainerCnv_p1.h"
#include "LArSimEventTPCnv/LArHitFloatContainerCnv_p2.h"
#include "LArHitFloatContainerCnv.h"
#include "LArSimEvent/LArHitContainer.h"

LArHitContainer_PERS* LArHitFloatContainerCnv::createPersistent(LArHitFloatContainer* transCont) {
    MsgStream mlog(messageService(), "LArHitContainerConverter" );
    LArHitFloatContainerCnv_p2   converter;
    LArHitContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}


LArHitFloatContainer* LArHitFloatContainerCnv::createTransient() {
    MsgStream mlog(messageService(), "LArHitFloatContainerConverter" );
    LArHitFloatContainerCnv_p1   converter_p1;
    LArHitFloatContainerCnv_p2   converter_p2;

    LArHitFloatContainer       *trans_cont(0);

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
        // old version from before TP separation  not supported
        std::auto_ptr<LArHitContainer> double_cont (this->poolReadObject<LArHitContainer>() );
        trans_cont = this->copyLArHitToFloat(double_cont.get());
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}

LArHitFloatContainer* LArHitFloatContainerCnv::copyLArHitToFloat(const LArHitContainer* double_cont) {
  
  LArHitFloatContainer* float_cont = new LArHitFloatContainer();
  float_cont->clear();
  float_cont->reserve(double_cont->size());

  LArHitContainer::const_iterator it1 = double_cont->begin();
  LArHitContainer::const_iterator it2 = double_cont->end();
  for (;it1 != it2; it1++) {
     LArHit* double_hit = (*it1);
     float energy = (float)(double_hit->energy());
     float time   = (float)(double_hit->time());
     LArHitFloat float_hit(double_hit->cellID(),energy,time);
     float_cont->push_back(float_hit);
  }

  return float_cont;

}
