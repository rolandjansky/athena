/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
 * @file LArSingleFloatConverter.h
 * @brief Base class for all converters for classes deriving from LArConditionsContainer<LArSingleFloatP>
 * @author Walter Lampl <walter.lampl@ cern.ch>
 * $Id: LArSingleFloatConverter.h,v 1.2 2008-04-10 14:34:13 wlampl Exp $
 */

#ifndef LARCONDTPCNV_LARSINGLEFLOATCNV_H
#define LARCONDTPCNV_LARSINGLEFLOATCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.h"
#include "LArRawConditions/LArSingleFloatP.h"
#include "LArCondTPCnv/LArSingleFloatSubsetCnv_p1.h"

template<class TransientClass>
class LArSingleFloatConverter : 
  public T_AthenaPoolCoolMultChanCnv<TransientClass, LArConditionsSubset<LArSingleFloatP>, LArSingleFloatSubset_p1> {
  friend class CnvFactory<LArSingleFloatConverter<TransientClass> >;

protected:
  typedef T_AthenaPoolCoolMultChanCnv< TransientClass, 
				       LArConditionsSubset < LArSingleFloatP > , 
				       LArSingleFloatSubset_p1 > cnvBase;

  LArSingleFloatConverter (ISvcLocator* svcloc) : cnvBase(svcloc) {};
  virtual LArSingleFloatSubset_p1* createPersistent(LArConditionsSubset<LArSingleFloatP>* transObj);

  /** Throws and exception if object is not a LArConditionsSubset<LArSingleFloatP> */
  virtual LArConditionsSubset<LArSingleFloatP>*  createTransient ();

  /** returns NULL if object is not a LArConditionsSubset<LArSingleFloatP> */
  LArConditionsSubset<LArSingleFloatP>*  createTransientSingleFloat ();


 private:
  LArSingleFloatSubsetCnv_p1 m_TPconverter;
};

template<class TransientClass>
LArSingleFloatSubset_p1* LArSingleFloatConverter<TransientClass>::createPersistent (LArConditionsSubset<LArSingleFloatP>* transObj) {
    MsgStream log(this->messageService(), "LArSingleFloatConverter" );
    LArSingleFloatSubset_p1* persObj = m_TPconverter.createPersistent( transObj, log );
    return persObj;
}

template<class TransientClass>
LArConditionsSubset<LArSingleFloatP>* LArSingleFloatConverter<TransientClass>::createTransientSingleFloat () {
  static pool::Guid p1_guid("85C3E380-4F5C-4F2B-81F0-D7C08A446800");
  if(this->compareClassGuid(p1_guid) ) {
    MsgStream log(this->messageService(), "LArSingleFloatCompleteCnv" );
    LArSingleFloatSubset_p1* p;
    LArConditionsSubset<LArSingleFloatP>* t;
    if(this->poolToObject(this->m_i_poolToken,p).isFailure() ) { // Token is passed via pointer member
      throw std::runtime_error("POOL read failed");
    }
    t=m_TPconverter.createTransient(p,log);
    delete p;
    return t;
  }
  return NULL;
}

template<class TransientClass>
LArConditionsSubset<LArSingleFloatP>* LArSingleFloatConverter<TransientClass>::createTransient() {
  LArConditionsSubset<LArSingleFloatP>* p=createTransientSingleFloat();
  if (p) return p;
  throw std::runtime_error("Unsupported persistent version of LArConditionsSubset<LArSingleFloat>");
}

#endif

