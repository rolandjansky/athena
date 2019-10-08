/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ORIGINALAODCOUNTS_H
#define ORIGINALAODCOUNTS_H

class TFile;
namespace xAOD {
  class TEvent;
}

struct OriginalAodCounts {
  OriginalAodCounts();
  unsigned long long nEventsProcessed;
  double sumOfWeights;
  double sumOfWeightsSquared;
  int nIncomplete;
  OriginalAodCounts& operator+=(const OriginalAodCounts& a);
};
OriginalAodCounts operator+(const OriginalAodCounts& a,
                            const OriginalAodCounts& b);
OriginalAodCounts operator+=(const OriginalAodCounts& a,
                             const OriginalAodCounts& b);

OriginalAodCounts getOriginalAodCounts(TFile& file, xAOD::TEvent& event);

#endif
