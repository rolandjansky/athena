/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_L1TOPORDOCOLLECTION_H
#define L1TOPORDO_L1TOPORDOCOLLECTION_H

#include "L1TopoRDO/L1TopoRDO.h"
#include "DataModel/DataVector.h"
#include "SGTools/BaseInfo.h"

//! Container of L1TopoRDOs (standard Athena boilerplate)
class L1TopoRDOCollection : public DataVector<L1TopoRDO> {

 public:
  L1TopoRDOCollection();
  ~L1TopoRDOCollection();

};

CLASS_DEF( L1TopoRDOCollection , 1235837073 , 1 )

SG_BASE( L1TopoRDOCollection , DataVector<L1TopoRDO> );

#endif // L1TOPORDO_L1TOPORDOCOLLECTION_H
