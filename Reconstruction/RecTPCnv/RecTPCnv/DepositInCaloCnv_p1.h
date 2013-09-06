///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCaloCnv_p1.h 
// Header file for class DepositInCaloCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_DEPOSITINCALOCNV_P1_H 
#define RECTPCNV_DEPOSITINCALOCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/DepositInCalo_p1.h"

// Forward declaration
class MsgStream;
class DepositInCalo;

class DepositInCaloCnv_p1 : public T_AthenaPoolTPCnvBase<DepositInCalo, 
			                                 DepositInCalo_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  DepositInCaloCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c DepositInCalo
   *  from its persistent representation @c DepositInCalo_p1
   */
  virtual void persToTrans( const DepositInCalo_p1* persObj, 
                            DepositInCalo* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c DepositInCalo_p1
   *  from its transient representation @c DepositInCalo
   */
  virtual void transToPers( const DepositInCalo* transObj, 
                            DepositInCalo_p1* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline DepositInCaloCnv_p1::DepositInCaloCnv_p1()
{}

#endif //> RECTPCNV_DEPOSITINCALOCNV_P1_H
