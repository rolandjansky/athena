///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleJetCnv_p1.h 
// Header file for class ParticleJetCnv_p1
// Author: P.A. Delsart
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_PARTICLEJETJETCNV_P4_H 
#define JETEVENTTPCNV_PARTICLEJETJETCNV_P4_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/ParticleJet_p1.h"
#include "JetEvent/JetTagInfoBase.h"

// Forward declaration
class MsgStream;
class Jet;

class ParticleJetCnv_p1 : public T_AthenaPoolTPCnvConstBase<Jet, ParticleJet_p1>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  ParticleJetCnv_p1();


  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c ParticleJet_p1
   */
  virtual void persToTrans( const ParticleJet_p1* persObj, 
                            Jet* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c ParticleJet_p1
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            ParticleJet_p1* persObj, 
                            MsgStream& msg ) const override;
}; 


inline ParticleJetCnv_p1::ParticleJetCnv_p1()
{
}



#endif //> JETEVENTTPCNV_JETCNV_P4_H
