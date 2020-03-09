///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p4.h 
// Header file for class JetCnv_p4
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCNV_P4_H 
#define JETEVENTTPCNV_JETCNV_P4_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p4.h"
#include "JetEvent/JetTagInfoBase.h"
#include <atomic>

// Forward declaration
class MsgStream;
class Jet;

class JetCnv_p4 : public T_AthenaPoolTPCnvConstBase<Jet, Jet_p4>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCnv_p4();


  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p4
   */
  virtual void persToTrans( const Jet_p4* persObj, 
                            Jet* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Jet_p4
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p4* persObj, 
                            MsgStream& msg ) const override;

  /// Init the converter so that it knows about its "parents" when it
  /// has to walk the lookup chain.
  inline virtual void initPrivateConverters(AthenaPoolTopLevelTPCnvBase *topCnv)
    override
  {
    m_taginfoCnv.setTopConverter(topCnv, 0);
    m_tagAssCnv.setTopConverter(topCnv,0);
  }

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Hold onto the converter for the jet tag info objects
  typedef T_AthenaPoolTPPolyVectorCnvConst<std::vector<const JetTagInfoBase*>,
                                           std::vector<TPObjRef>,
                                           ITPConverterFor<JetTagInfoBase> > TagInfoCollectionCnv;

  TagInfoCollectionCnv m_taginfoCnv;

  /// Hold onto the converter for the jet assoctiaion objects
  typedef T_AthenaPoolTPPolyVectorCnvConst<std::vector<const JetAssociationBase*>,
                                           std::vector<TPObjRef>,
                                           ITPConverterFor<JetAssociationBase> > TagAssCollectionCnv;
  
  TagAssCollectionCnv m_tagAssCnv;

  // See persToTrans().
  mutable std::atomic<bool> m_badIndex;
  mutable std::atomic<int> m_nIndexTest;

}; 


#endif //> JETEVENTTPCNV_JETCNV_P4_H
