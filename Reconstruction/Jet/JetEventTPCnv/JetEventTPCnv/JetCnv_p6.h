///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p6.h
// Header file for class JetCnv_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCNV_P6_H
#define JETEVENTTPCNV_JETCNV_P6_H

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p6.h"
//#include "JetEvent/JetTagInfoBase.h"
class JetTagInfoBase;
//PA Tool
#include "JetEventTPCnv/JetCnvProperties.h"

// Forward declaration
class MsgStream;
class Jet;


template <>
class JetConverterBase<Jet_p6> : public T_AthenaPoolTPCnvConstBase<Jet, Jet_p6> , JetCnvProperties
{
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetConverterBase() { };
  
  
  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p6
   */
  virtual void persToTrans( const Jet_p6* persObj, 
                            Jet* transObj, 
                            MsgStream& msg ) const override;
  
  /** Method creating the persistent representation @c Jet_p6
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p6* persObj, 
                            MsgStream& msg ) const override;
  
  /// Init the converter so that it knows about its "parents" when it
  /// has to walk the lookup chain.
  inline
  virtual void initPrivateConverters(AthenaPoolTopLevelTPCnvBase *topCnv) override
  {
    m_taginfoCnv.setTopConverter(topCnv, 0);
    m_tagAssCnv.setTopConverter(topCnv,0);
  }
  
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

  
}; 

typedef JetConverterBase<Jet_p6> JetCnv_p6;

#endif //> JETEVENTTPCNV_JETCNV_P6_H
