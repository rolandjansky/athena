// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Converter from TrigInDetTrackTruthMap_old to TrigInDetTrackTruthMap.
// $Id: TrigInDetTrackTruthMap_old_cnv.h,v 1.1 2008-08-19 04:18:21 ssnyder Exp $

#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_OLD_CNV
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_OLD_CNV

#include "RootConversions/TVirtualConverter.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old.h"

class TrigInDetTrackTruthMap_old_cnv
  : public TVirtualConverter_T<TrigInDetTrackTruthMap,
                               TrigInDetTrackTruthMap_old>
{
public:
  virtual void Convert (TrigInDetTrackTruthMap* newobj,
                        const TrigInDetTrackTruthMap_old* oldobj);
};


#endif // not TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_OLD_CNV

