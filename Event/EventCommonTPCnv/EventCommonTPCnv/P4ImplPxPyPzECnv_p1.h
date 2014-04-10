///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplPxPyPzECnv_p1.h 
// Header file for class P4ImplPxPyPzECnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4IMPLPXPYPZECNV_P1_H 
#define EVENTCOMMONTPCNV_P4IMPLPXPYPZECNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/Lib/P4ImplPxPyPzE.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzE_p1.h"

// Forward declaration
class MsgStream;

// to get the same look'n feel (even if we are reusing the same _p1 class)
typedef P4PxPyPzE_p1 P4ImplPxPyPzE_p1;

class P4ImplPxPyPzECnv_p1 : public T_AthenaPoolTPCnvBase<
                                              P4ImplPxPyPzE, 
                                              P4ImplPxPyPzE_p1
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  P4ImplPxPyPzECnv_p1();

  /** Destructor: 
   */
  virtual ~P4ImplPxPyPzECnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c P4ImplPxPyPzE
   *  from its persistent representation @c P4ImplPxPyPzE_p1
   */
  virtual void persToTrans( const P4ImplPxPyPzE_p1* persObj, 
                            P4ImplPxPyPzE* transObj, 
                            MsgStream &msg );

  /** Method creating the persistent representation @c P4ImplPxPyPzE_p1
   *  from its transient representation @c P4PxPyPzE
   */
  virtual void transToPers( const P4ImplPxPyPzE* transObj, 
                            P4ImplPxPyPzE_p1* persObj, 
                            MsgStream &msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4ImplPxPyPzECnv_p1::P4ImplPxPyPzECnv_p1() 
{}

inline P4ImplPxPyPzECnv_p1::~P4ImplPxPyPzECnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_P4IMPLPXPYPZECNV_P1_H
