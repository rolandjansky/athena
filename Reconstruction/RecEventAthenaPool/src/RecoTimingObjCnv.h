/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_RECOTIMINGOBJCNV_H 
#define RECATHENAPOOL_RECOTIMINGOBJCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "RecEvent/RecoTimingObj.h"
#include "RecEventTPCnv/RecoTimingObj_p1.h"

class MsgStream;

typedef RecoTimingObj_p1  RecoTimingObj_PERS;

class RecoTimingObjCnv: public T_AthenaPoolCustomCnv< RecoTimingObj, RecoTimingObj_PERS >
{

  // make the factory for this converter our friend
  friend class CnvFactory<RecoTimingObjCnv>;
  
  // destructor
  virtual ~RecoTimingObjCnv() { delete m_msg; }; 
    
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:
  
  /** Create the converter from the service locator
   */
  RecoTimingObjCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual RecoTimingObj_PERS* createPersistent( RecoTimingObj* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual RecoTimingObj* createTransient();

 private:
  
  MsgStream *m_msg;
  
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline RecoTimingObjCnv::RecoTimingObjCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<RecoTimingObj, RecoTimingObj_PERS>(svcLocator),
  m_msg(0)
{}

#endif
