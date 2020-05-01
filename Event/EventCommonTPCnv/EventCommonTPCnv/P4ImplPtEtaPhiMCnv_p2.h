///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4ImplPtEtaPhiMCnv_p2.h 
// Header file for class P4ImplPtEtaPhiMCnv_p2
// Author: I. Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4IMPLPTETAPHIMCNV_P2_H 
#define EVENTCOMMONTPCNV_P4IMPLPTETAPHIMCNV_P2_H 

// STL includes


// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/Lib/P4ImplPtEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PtEtaPhiMFloat_p2.h"

// Forward declaration
class MsgStream;

class P4ImplPtEtaPhiMCnv_p2 : public T_AthenaPoolTPCnvConstBase<
                                              P4ImplPtEtaPhiM, 
                                              P4PtEtaPhiMFloat_p2
                                            >  
{ 
 public: 
  using base_class::transToPers; 
  using base_class::persToTrans;


  /** Default constructor: 
   */
  P4ImplPtEtaPhiMCnv_p2();

  /** Destructor: 
   */
  virtual ~P4ImplPtEtaPhiMCnv_p2();


  /** Method creating the transient representation of @c P4ImplPtEtaPhiM
   *  from its persistent representation @c P4PtEtaPhiMFloat_p2
   */
  virtual void persToTrans( const P4PtEtaPhiMFloat_p2* persObj, 
                            P4ImplPtEtaPhiM* transObj, 
                            MsgStream &log ) const override;

  /** Method creating the persistent representation @c P4PtEtaPhiMFloat_p2
   *  from its transient representation @c P4ImplPtEtaPhiM
   */
  virtual void transToPers( const P4ImplPtEtaPhiM* transObj, 
                            P4PtEtaPhiMFloat_p2* persObj, 
                            MsgStream &log ) const override;
}; 


inline P4ImplPtEtaPhiMCnv_p2::P4ImplPtEtaPhiMCnv_p2() 
{}

inline P4ImplPtEtaPhiMCnv_p2::~P4ImplPtEtaPhiMCnv_p2()
{}

#endif //> EVENTCOMMONTPCNV_P4IMPLPTETAPHIMCNV_P2_H
