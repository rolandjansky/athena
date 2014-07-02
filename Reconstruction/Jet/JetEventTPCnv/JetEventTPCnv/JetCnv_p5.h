///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p5.h
// Header file for class JetCnv_p5
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCNV_P5_H
#define JETEVENTTPCNV_JETCNV_P5_H

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p5.h"
#include "JetEvent/JetTagInfoBase.h"

//PA Tool
#include "JetEventTPCnv/JetCnvProperties.h"

// Forward declaration
class MsgStream;
class Jet;


template <>
class JetConverterBase<Jet_p5> : public T_AthenaPoolTPCnvBase<Jet, Jet_p5> , JetCnvProperties
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  JetConverterBase() { };
  
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  
  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p5
   */
  virtual void persToTrans( const Jet_p5* persObj, 
                            Jet* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c Jet_p5
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p5* persObj, 
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
  
}; 

typedef JetConverterBase<Jet_p5> JetCnv_p5;

#endif //> JETEVENTTPCNV_JETCNV_P5_H
