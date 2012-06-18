///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p3.h 
// Header file for class egammaCnv_p3
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACNV_P3_H 
#define EGAMMAEVENTTPCNV_EGAMMACNV_P3_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p3.h"

// Forward declaration
class MsgStream;
class egamma;

class egammaCnv_p3 : public T_AthenaPoolTPCnvBase<egamma, egamma_p3>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  egammaCnv_p3();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c egamma
   *  from its persistent representation @c egamma_p3
   */
  virtual void persToTrans( const egamma_p3* persObj, 
                            egamma* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c egamma_p3
   *  from its transient representation @c egamma
   */
  virtual void transToPers( const egamma* transObj, 
                            egamma_p3* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egammaCnv_p3::egammaCnv_p3()
{}

#endif //> EGAMMAEVENTTPCNV_EGAMMACNV_P3_H
