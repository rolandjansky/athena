///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileHitVectorCnv_p1.cxx 
// Implementation file for class TileHitVectorCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileSimEventTPCnv/TileHitVectorCnv_p1.h"

TileHitVector* TileHitVectorCnv_p1::createTransient(const TileHitVector_p1* persObj, MsgStream &log) {

  const TileHitVector_p1::HitVector& pvec = persObj->getVector();
  
  // the only reason why we reimplement this method is that we want to be sure
  // that correct size is reserved for the vector directly in the constructor
  TileHitVector* trans = new TileHitVector(persObj->name(), pvec.size());
  trans->resize(pvec.size());
  
  // since we reimplement this method anyhow, let's call directly persToTrans method of
  // the converter instead of doing this through few calls in base class
  TileHitCnv_p1 element_cnv;
  TileHitVector::iterator it;
  TileHitVector_p1::const_iterator ip;

  for (it = trans->begin(), ip = pvec.begin(); ip != pvec.end(); ++ip, ++it) {
    element_cnv.persToTrans( &*ip, &*it, log );
  }

  
  return trans;
}
