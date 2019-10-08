///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCnv_p2.h 
// Header file for class JetSamplingCnv_p2
// Author: Belen.Salvachua@cern.ch
// Date: Dec 2008
//     Based on JetSamplingCnv_p1.h
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLINGCNV_P2_H 
#define JETEVENTTPCNV_JETSAMPLINGCNV_P2_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSampling_p2.h"

// Forward declaration
class MsgStream;
class JetSampling;

class JetSamplingCnv_p2 : public T_AthenaPoolTPCnvConstBase<JetSampling, 
                                                            JetSampling_p2>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetSamplingCnv_p2();


  /** Method creating the transient representation of @c JetSampling
   *  from its persistent representation @c JetSampling_p2
   */
  virtual void persToTrans( const JetSampling_p2* pers, 
                            JetSampling* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetSampling_p2
   *  from its transient representation @c JetSampling
   */
  virtual void transToPers( const JetSampling* trans, 
                            JetSampling_p2* pers, 
                            MsgStream& msg ) const override;

 protected: 
  static const HepLorentzVectorCnv_p1 m_hlvCnv;
}; 


inline JetSamplingCnv_p2::JetSamplingCnv_p2()
{}

#endif //> JETEVENTTPCNV_JETSAMPLINGCNV_P2_H
