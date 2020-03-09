/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETMOMENTMAPCNV_P1_H
#define JETEVENTTPCNV_JETMOMENTMAPCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetEventTPCnv/JetMomentMapConverterBase.h"
#include "JetEventTPCnv/JetMomentMap_p1.h"
#include "JetEvent/JetMomentMap.h"

#include <string>
#include <vector>

template <>
class JetMomentMapConverterBase<JetMomentMap_p1>
: public T_AthenaPoolTPCnvConstBase<JetMomentMap, JetMomentMap_p1>
{
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  JetMomentMapConverterBase() { };
  virtual ~JetMomentMapConverterBase() { };
  
  void persToTrans( const JetMomentMap_p1* persObj, 
		    JetMomentMap* transObj, 
		    MsgStream& reporter ) const override;
  
  /** Method creating the persistent representation @c JetMomentMap_p1
   *  from its transient representation @c JetMomentMap
   */
  void transToPers( const JetMomentMap* transObj,
		    JetMomentMap_p1* persObj, 
		    MsgStream& reporter ) const override;
};

typedef JetMomentMapConverterBase<JetMomentMap_p1> JetMomentMapCnv_p1;

#endif
