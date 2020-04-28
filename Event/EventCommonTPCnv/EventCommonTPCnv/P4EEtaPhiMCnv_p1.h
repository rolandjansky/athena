///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4EEtaPhiMCnv_p1.h 
// Header file for class P4EEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_P4EETAPHIMCNV_P1_H 
#define EVENTCOMMONTPCNV_P4EETAPHIMCNV_P1_H 

// STL includes


// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// FourMom includes
#include "FourMom/P4EEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"

// Forward declaration
class MsgStream;

class P4EEtaPhiMCnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                              P4EEtaPhiM, 
                                              P4EEtaPhiM_p1
                                            >  
{ 
 public:
  using base_class::transToPers; 
  using base_class::persToTrans;


  /** Default constructor: 
   */
  P4EEtaPhiMCnv_p1();

  /** Destructor: 
   */
  virtual ~P4EEtaPhiMCnv_p1();


  /** Method creating the transient representation of @c P4EEtaPhiM
   *  from its persistent representation @c P4EEtaPhiM_p1
   */
  virtual void persToTrans( const P4EEtaPhiM_p1* persObj, 
                            P4EEtaPhiM* transObj, 
                            MsgStream &log ) const override;

  /** Method creating the persistent representation @c P4EEtaPhiM_p1
   *  from its transient representation @c P4EEtaPhiM
   */
  virtual void transToPers( const P4EEtaPhiM* transObj, 
                            P4EEtaPhiM_p1* persObj, 
                            MsgStream &log ) const override;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline P4EEtaPhiMCnv_p1::P4EEtaPhiMCnv_p1() 
{}

inline P4EEtaPhiMCnv_p1::~P4EEtaPhiMCnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_P4EETAPHIMCNV_P1_H
