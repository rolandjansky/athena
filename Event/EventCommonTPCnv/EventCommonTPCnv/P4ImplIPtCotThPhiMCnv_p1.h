///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplIPtCotThPhiMCnv_p1.h 
// Header file for class P4ImplIPtCotThPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4IMPLIPTCOTTHPHIMCNV_P1_H 
#define EVENTCOMMONTPCNV_P4IMPLIPTCOTTHPHIMCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"

// Forward declaration
class MsgStream;

// to get the same look'n feel (even if we are reusing the same _p1 class)
typedef P4IPtCotThPhiM_p1 P4ImplIPtCotThPhiM_p1;

class P4ImplIPtCotThPhiMCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              P4ImplIPtCotThPhiM, 
                                              P4ImplIPtCotThPhiM_p1
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  P4ImplIPtCotThPhiMCnv_p1();

  /** Destructor: 
   */
  virtual ~P4ImplIPtCotThPhiMCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c P4ImplIPtCotThPhiM
   *  from its persistent representation @c P4ImplIPtCotThPhiM_p1
   */
  virtual void persToTrans( const P4ImplIPtCotThPhiM_p1* persObj, 
                            P4ImplIPtCotThPhiM* transObj, 
                            MsgStream &msg );

  /** Method creating the persistent representation @c P4ImplIPtCotThPhiM_p1
   *  from its transient representation @c P4ImplIPtCotThPhiM
   */
  virtual void transToPers( const P4ImplIPtCotThPhiM* transObj, 
                            P4ImplIPtCotThPhiM_p1* persObj, 
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

inline P4ImplIPtCotThPhiMCnv_p1::P4ImplIPtCotThPhiMCnv_p1() 
{}

inline P4ImplIPtCotThPhiMCnv_p1::~P4ImplIPtCotThPhiMCnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_P4IMPLIPTCOTTHPHIMCNV_P1_H
