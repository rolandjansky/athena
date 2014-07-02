///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Forward declaration
class MsgStream;
class Jet;

class JetCnv_p4 : public T_AthenaPoolTPCnvBase<Jet, Jet_p4>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  JetCnv_p4();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p4
   */
  virtual void persToTrans( const Jet_p4* persObj, 
                            Jet* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c Jet_p4
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p4* persObj, 
                            MsgStream& msg );

  /// Init the converter so that it knows about its "parents" when it
  /// has to walk the lookup chain.
  inline virtual void initPrivateConverters(AthenaPoolTopLevelTPCnvBase *topCnv)
  {
    m_taginfoCnv.setTopConverter(topCnv, 0);
    m_tagAssCnv.setTopConverter(topCnv,0);
  }

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Hold onto the converter for the jet tag info objects
  typedef T_AthenaPoolTPPolyVectorCnv<std::vector<const JetTagInfoBase*>,
                                      std::vector<TPObjRef>,
                                      ITPConverterFor<JetTagInfoBase> > TagInfoCollectionCnv;

  TagInfoCollectionCnv m_taginfoCnv;

  /// Hold onto the converter for the jet assoctiaion objects
  typedef T_AthenaPoolTPPolyVectorCnv<std::vector<const JetAssociationBase*>,
				      std::vector<TPObjRef>,
				      ITPConverterFor<JetAssociationBase> > TagAssCollectionCnv;
  
  TagAssCollectionCnv m_tagAssCnv;

  // See persToTrans().
  bool m_badIndex;
  int m_nIndexTest;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> JETEVENTTPCNV_JETCNV_P4_H
