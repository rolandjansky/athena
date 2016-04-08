/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTRKINFO_ALIGNTRKCTR_H
#define INDETALIGNTRKINFO_ALIGNTRKCTR_H
// AlignTrkContainer.h - a container of AlignTrk from InDetAlignTrkInfo
// Richard Hawkings, started 13/4/04

// magic copied from iPatRecEvent/iPatTrackContainer.h
#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
#include "DataModel/DataVector.h"
#include "InDetAlignTrkInfo/AlignTrk.h"

class AlignTrkContainer : public DataVector<AlignTrk> {

public:
  AlignTrkContainer();
  ~AlignTrkContainer();

  void print() const;
};

CLASS_DEF( AlignTrkContainer , 1337332732 , 1 )

#endif // INDETALIGNTRKINFO_ALIGNTRKCTR_H
