///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBaseCnv_p1.h 
// Header file for class ParticleBaseCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEBASECNV_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLEBASECNV_P1_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p1.h"
#include "ParticleEvent/ParticleImpl.h"
#include "ParticleEvent/ParticleBase.h"

// Forward declaration
class MsgStream;

class ParticleBaseCnv_p1
  : public T_AthenaPoolTPCnvBase<ParticleEvent::Base, ParticleBase_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ParticleBaseCnv_p1();

  /** d-tor
   */
  virtual ~ParticleBaseCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c ParticleBase
   *  from its persistent representation @c ParticleBase_p1
   */
  virtual void persToTrans( const ParticleBase_p1* persObj, 
                            ParticleBase* transObj, 
                            MsgStream &msg ) final;

  /** Method creating the persistent representation @c ParticleBase_p1
   *  from its transient representation @c ParticleBase
   */
  virtual void transToPers( const ParticleBase* transObj, 
                            ParticleBase_p1* persObj, 
                            MsgStream &msg ) final;

  /** Method creating the transient representation of @c ParticleEvent::Base
   *  from its persistent representation @c ParticleBase_p1
   */
  virtual void persToTrans( const ParticleBase_p1* persObj, 
                            ParticleEvent::Base* transObj, 
                            MsgStream &msg ) override final;

  /** Method creating the persistent representation @c ParticleBase_p1
   *  from its transient representation @c ParticleEvent::ParticleBase
   */
  virtual void transToPers( const ParticleEvent::Base* transObj, 
                            ParticleBase_p1* persObj, 
                            MsgStream &msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ParticleBaseCnv_p1::ParticleBaseCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_PARTICLEBASECNV_P1_H
