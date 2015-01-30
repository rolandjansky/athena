/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETMOMENTMAPCNV_p6_H
#define JETEVENTTPCNV_JETMOMENTMAPCNV_p6_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetEventTPCnv/JetMomentMapConverterBase.h"
#include "JetEventTPCnv/JetMomentMap_p6.h"
#include "JetEvent/JetMomentMap.h"

#include <string>
#include <vector>

template <>
class JetMomentMapConverterBase<JetMomentMap_p6>
: public T_AthenaPoolTPCnvBase<JetMomentMap, JetMomentMap_p6>
{
 public: 
  
  JetMomentMapConverterBase() { };
  virtual ~JetMomentMapConverterBase() { };
  
  void persToTrans( const JetMomentMap_p6* persObj, 
		    JetMomentMap* transObj, 
		    MsgStream& reporter );
  
  /** Method creating the persistent representation @c JetMomentMap_p6
   *  from its transient representation @c JetMomentMap
   */
  void transToPers( const JetMomentMap* transObj,
		    JetMomentMap_p6* persObj, 
		    MsgStream& reporter );
  
 protected: 
};

typedef JetMomentMapConverterBase<JetMomentMap_p6> JetMomentMapCnv_p6;

#endif
