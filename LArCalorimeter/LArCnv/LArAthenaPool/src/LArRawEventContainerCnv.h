/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArRawEventContainerCnv_H
#define LArRawEventContainerCnv_H

// templated LArRawEventContainerCnv, used for T/P separation
// author R.Seuster <seuster@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

template<class T, class P, class C>
class LArRawEventContainerCnv : public T_AthenaPoolCustomCnv<T, P>
{
public:
  LArRawEventContainerCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv<T, P>( svcloc) {}
  
 protected:
  virtual P*  createPersistent(T* transCont);
  virtual T*  createTransient ();
  
  C m_Converter;
};

#endif
