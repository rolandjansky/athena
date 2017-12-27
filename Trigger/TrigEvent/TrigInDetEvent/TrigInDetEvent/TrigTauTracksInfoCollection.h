/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRACKSINFOCOLLECTION_H
#define TRIGTAUTRACKSINFOCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "AthenaKernel/BaseInfo.h"

class TrigTauTracksInfoCollection : public DataVector<TrigTauTracksInfo> { }; 

CLASS_DEF( TrigTauTracksInfoCollection , 1090546955 , 1 )

SG_BASE(TrigTauTracksInfoCollection, DataVector<TrigTauTracksInfo>);

#endif
