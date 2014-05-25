/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloAffectedRegionInfoVec_H
#define CaloAffectedRegionInfoVec_H

#include <vector>
#include "CaloConditions/CaloAffectedRegionInfo.h"

typedef std::vector<CaloAffectedRegionInfo>  CaloAffectedRegionInfoVec;

#ifndef CLIDSVC_CLASSDEF_H
# include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( CaloAffectedRegionInfoVec , 36372397, 1 )

#endif
