///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p1.h 
// Header file for class egammaCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACNV_P1_H 
#define EGAMMAEVENTTPCNV_EGAMMACNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p1.h"

// Forward declaration
class MsgStream;
class egamma;

class egammaCnv_p1 : public T_AthenaPoolTPCnvBase<egamma, egamma_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  egammaCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c egamma
   *  from its persistent representation @c egamma_p1
   */
  virtual void persToTrans( const egamma_p1* persObj, 
                            egamma* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c egamma_p1
   *  from its transient representation @c egamma
   */
  virtual void transToPers( const egamma* transObj, 
                            egamma_p1* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egammaCnv_p1::egammaCnv_p1()
{}

#endif //> EGAMMAEVENTTPCNV_EGAMMACNV_P1_H
