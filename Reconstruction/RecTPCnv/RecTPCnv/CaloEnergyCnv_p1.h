///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloEnergyCnv_p1.h 
// Header file for class CaloEnergyCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_CALOENERGYCNV_P1_H 
#define RECTPCNV_CALOENERGYCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/CaloEnergy_p1.h"

// Forward declaration
class MsgStream;
class CaloEnergy;

class CaloEnergyCnv_p1 : public T_AthenaPoolTPCnvBase<CaloEnergy, 
			                              CaloEnergy_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  CaloEnergyCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c CaloEnergy
   *  from its persistent representation @c CaloEnergy_p1
   */
  virtual void persToTrans( const CaloEnergy_p1* persObj, 
                            CaloEnergy* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c CaloEnergy_p1
   *  from its transient representation @c CaloEnergy
   */
  virtual void transToPers( const CaloEnergy* transObj, 
                            CaloEnergy_p1* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline CaloEnergyCnv_p1::CaloEnergyCnv_p1()
{}

#endif //> RECTPCNV_CALOENERGYCNV_P1_H
