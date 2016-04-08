/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIRECATHENAPOOL_HITRTINFO_H
#define HIRECATHENAPOOL_HITRTINFO_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "HIGlobal/HITRTInfo.h"
#include "HIRecTPCnv/HITRTInfo_p1.h"

class MsgStream;

typedef HITRTInfo_p1  HITRTInfo_PERS;

class HITRTInfoCnv: public T_AthenaPoolCustomCnv< HITRTInfo, HITRTInfo_PERS >
{

  // make the factory for this converter our friend
  friend class CnvFactory<HITRTInfoCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:
  
  /** Create the converter from the service locator
   */
  HITRTInfoCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual HITRTInfo_PERS* createPersistent( HITRTInfo* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual HITRTInfo* createTransient();

 private:
  
  MsgStream *m_msg;
  
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline HITRTInfoCnv::HITRTInfoCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<HITRTInfo, HITRTInfo_PERS>(svcLocator),
  m_msg(0)
{}

#endif
