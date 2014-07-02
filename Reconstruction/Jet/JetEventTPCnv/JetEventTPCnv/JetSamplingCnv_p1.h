///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCnv_p1.h 
// Header file for class JetSamplingCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLINGCNV_P1_H 
#define JETEVENTTPCNV_JETSAMPLINGCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSampling_p1.h"

// Forward declaration
class MsgStream;
class JetSampling;

class JetSamplingCnv_p1 : public T_AthenaPoolTPCnvBase<JetSampling, 
                                                       JetSampling_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  JetSamplingCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c JetSampling
   *  from its persistent representation @c JetSampling_p1
   */
  virtual void persToTrans( const JetSampling_p1* pers, 
                            JetSampling* trans, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c JetSampling_p1
   *  from its transient representation @c JetSampling
   */
  virtual void transToPers( const JetSampling* trans, 
                            JetSampling_p1* pers, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  static HepLorentzVectorCnv_p1 m_hlvCnv;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline JetSamplingCnv_p1::JetSamplingCnv_p1()
{}

#endif //> JETEVENTTPCNV_JETSAMPLINGCNV_P1_H
