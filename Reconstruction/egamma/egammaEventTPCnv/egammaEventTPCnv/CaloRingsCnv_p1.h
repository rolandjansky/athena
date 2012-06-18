///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloRingsCnv_p1.h 
// Header file for class CaloRingsCnv_p1
// Author: D.E.Ferreira de Lima<dferreir@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_CALORINGSCNV_P1_H 
#define RECTPCNV_CALORINGSCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "egammaEventTPCnv/CaloRings_p1.h"

// Forward declaration
class MsgStream;
class CaloRings;

class CaloRingsCnv_p1 : public T_AthenaPoolTPCnvBase<CaloRings, CaloRings_p1>
{

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  CaloRingsCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c CaloRings
   *  from its persistent representation @c CaloRings_p1
   */
  virtual void persToTrans( const CaloRings_p1* persObj, 
                            CaloRings* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c CaloRings_p1
   *  from its transient representation @c CaloRings
   */
  virtual void transToPers( const CaloRings* transObj, 
                            CaloRings_p1* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline CaloRingsCnv_p1::CaloRingsCnv_p1()
{}

#endif //> RECTPCNV_CALORINGSCNV_P1_H
