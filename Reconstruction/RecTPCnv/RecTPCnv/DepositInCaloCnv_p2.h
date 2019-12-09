///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCaloCnv_p2.h 
// Header file for class DepositInCaloCnv_p2
// Author: Ketevi A. Assamagan
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_DEPOSITINCALOCNV_P2_H 
#define RECTPCNV_DEPOSITINCALOCNV_P2_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/DepositInCalo_p2.h"

// Forward declaration
class MsgStream;
class DepositInCalo;

class DepositInCaloCnv_p2 : public T_AthenaPoolTPCnvConstBase<DepositInCalo, 
                                                              DepositInCalo_p2>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  DepositInCaloCnv_p2();


  /** Method creating the transient representation of @c DepositInCalo
   *  from its persistent representation @c DepositInCalo_p2
   */
  virtual void persToTrans( const DepositInCalo_p2* persObj, 
                            DepositInCalo* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c DepositInCalo_p2
   *  from its transient representation @c DepositInCalo
   */
  virtual void transToPers( const DepositInCalo* transObj, 
                            DepositInCalo_p2* persObj, 
                            MsgStream& msg ) const override;
}; 


inline DepositInCaloCnv_p2::DepositInCaloCnv_p2()
{}

#endif //> RECTPCNV_DEPOSITINCALOCNV_P1_H
