///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplLEEtaPhiMCnv_p2.h 
// Header file for class P4ImplEEtaPhiMCnv_p2
// Author: I. Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4IMPLEETAPHIMCNV_P2_H 
#define EVENTCOMMONTPCNV_P4IMPLEETAPHIMCNV_P2_H 

// STL includes


// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/Lib/P4ImplEEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"

// Forward declaration
class MsgStream;

class P4ImplEEtaPhiMCnv_p2 : public T_AthenaPoolTPCnvConstBase<
                                              P4ImplEEtaPhiM, 
                                              P4EEtaPhiMFloat_p2
                                            >  
{ 
 public: 
  using base_class::transToPers; 
  using base_class::persToTrans;


  /** Default constructor: 
   */
  P4ImplEEtaPhiMCnv_p2();

  /** Destructor: 
   */
  virtual ~P4ImplEEtaPhiMCnv_p2();


  /** Method creating the transient representation of @c P4ImplEEtaPhiM
   *  from its persistent representation @c P4EEtaPhiMFloat_p2
   */
  virtual void persToTrans( const P4EEtaPhiMFloat_p2* persObj, 
                            P4ImplEEtaPhiM* transObj, 
                            MsgStream &log ) const override;

  /** Method creating the persistent representation @c P4EEtaPhiMFloat_p2
   *  from its transient representation @c P4ImplEEtaPhiM
   */
  virtual void transToPers( const P4ImplEEtaPhiM* transObj, 
                            P4EEtaPhiMFloat_p2* persObj, 
                            MsgStream &log ) const override;
}; 


inline P4ImplEEtaPhiMCnv_p2::P4ImplEEtaPhiMCnv_p2() 
{}

inline P4ImplEEtaPhiMCnv_p2::~P4ImplEEtaPhiMCnv_p2()
{}

#endif //> EVENTCOMMONTPCNV_P4IMPLEETAPHIMCNV_P2_H
