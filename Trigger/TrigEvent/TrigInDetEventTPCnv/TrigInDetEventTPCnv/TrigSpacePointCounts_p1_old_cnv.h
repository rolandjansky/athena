// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Converter from TrigSpacePointCounts_p1_old to TrigSpacePointCounts_p1.
// $Id: TrigSpacePointCounts_p1_old_cnv.h,v 1.2 2009-04-01 22:08:44 salvator Exp $

#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_OLD_CNV_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_OLD_CNV_H

#include "RootConversions/TVirtualConverter.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1_old.h"

class TrigSpacePointCounts_p1_old_cnv
  : public TVirtualConverter_T<TrigSpacePointCounts_p1, TrigSpacePointCounts_p1_old>
{
public:
  virtual void Convert (TrigSpacePointCounts_p1* newobj,
                        const TrigSpacePointCounts_p1_old* oldobj);
};

#endif // not TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_OLD_CNV_H
