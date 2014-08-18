/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetEvent_JetMomentMapCollection_H
#define JetEvent_JetMomentMapCollection_H
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"

#include "JetEvent/JetMomentMap.h"

class JetMomentMapCollection : public DataVector<JetMomentMap> {
 public:
  // constructor
  virtual ~JetMomentMapCollection() { };
};
CLASS_DEF( JetMomentMapCollection , 1341312035 , 1 )

SG_BASE(JetMomentMapCollection, DataVector<JetMomentMap>);

#endif
