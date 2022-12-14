///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4PxPyPzECnv_p1.h 
// Header file for class P4PxPyPzECnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4PXPYPZECNV_P1_H 
#define EVENTCOMMONTPCNV_P4PXPYPZECNV_P1_H 

// STL includes


// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/P4PxPyPzE.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzE_p1.h"

// Forward declaration
class MsgStream;

class P4PxPyPzECnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                              P4PxPyPzE, 
                                              P4PxPyPzE_p1
                                            >  
{ 
 public: 
  using base_class::transToPers; 
  using base_class::persToTrans;


  /** Default constructor: 
   */
  P4PxPyPzECnv_p1();

  /** Destructor: 
   */
  virtual ~P4PxPyPzECnv_p1();


  /** Method creating the transient representation of @c P4PxPyPzE
   *  from its persistent representation @c P4PxPyPzE_p1
   */
  virtual void persToTrans( const P4PxPyPzE_p1* persObj, 
                            P4PxPyPzE* transObj, 
                            MsgStream &log ) const override;

  /** Method creating the persistent representation @c P4PxPyPzE_p1
   *  from its transient representation @c P4PxPyPzE
   */
  virtual void transToPers( const P4PxPyPzE* transObj, 
                            P4PxPyPzE_p1* persObj, 
                            MsgStream &log ) const override;
}; 


inline P4PxPyPzECnv_p1::P4PxPyPzECnv_p1() 
{}

inline P4PxPyPzECnv_p1::~P4PxPyPzECnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_P4PXPYPZECNV_P1_H
