///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class ParticleJetCnv_p1 : public T_AthenaPoolTPCnvBase<Jet, ParticleJet_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ParticleJetCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c ParticleJet_p1
   */
  virtual void persToTrans( const ParticleJet_p1* persObj, 
                            Jet* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c ParticleJet_p1
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            ParticleJet_p1* persObj, 
                            MsgStream& msg );

  /// Init the converter so that it knows about its "parents" when it
  /// has to walk the lookup chain.
//   inline virtual void initPrivateConverters(AthenaPoolTopLevelTPCnvBase *topCnv)
//   {
//     m_taginfoCnv.setTopConverter(topCnv, 0);
//   }

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Hold onto the converter for the jet tag info objects
  //   typedef T_AthenaPoolTPPolyVectorCnv<std::vector<const JetTagInfoBase*>,
  //                                       std::vector<TPObjRef>,
  //                                       ITPConverterFor<JetTagInfoBase> > TagInfoCollectionCnv;

  //TagInfoCollectionCnv m_taginfoCnv;


}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ParticleJetCnv_p1::ParticleJetCnv_p1()
{
}



#endif //> JETEVENTTPCNV_JETCNV_P4_H
