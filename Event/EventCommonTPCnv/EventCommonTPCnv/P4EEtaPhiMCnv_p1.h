///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4EEtaPhiMCnv_p1.h 
// Header file for class P4EEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4EETAPHIMCNV_P1_H 
#define EVENTCOMMONTPCNV_P4EETAPHIMCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/P4EEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"

// Forward declaration
class MsgStream;

class P4EEtaPhiMCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              P4EEtaPhiM, 
                                              P4EEtaPhiM_p1
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  P4EEtaPhiMCnv_p1();

  /** Destructor: 
   */
  virtual ~P4EEtaPhiMCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c P4EEtaPhiM
   *  from its persistent representation @c P4EEtaPhiM_p1
   */
  virtual void persToTrans( const P4EEtaPhiM_p1* persObj, 
                            P4EEtaPhiM* transObj, 
                            MsgStream &log );

  /** Method creating the persistent representation @c P4EEtaPhiM_p1
   *  from its transient representation @c P4EEtaPhiM
   */
  virtual void transToPers( const P4EEtaPhiM* transObj, 
                            P4EEtaPhiM_p1* persObj, 
                            MsgStream &log );

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

inline P4EEtaPhiMCnv_p1::P4EEtaPhiMCnv_p1() 
{}

inline P4EEtaPhiMCnv_p1::~P4EEtaPhiMCnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_P4EETAPHIMCNV_P1_H
