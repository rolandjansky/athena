/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ORIGINALAODCOUNTS_H
#define ORIGINALAODCOUNTS_H

namespace xAOD {
  class TEvent;
}
class StatusCode;



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

OriginalAodCounts getOriginalAodCounts(xAOD::TEvent& event);

// RECOMMENDED INTERFACE
StatusCode incrementAodCounts(xAOD::TEvent& event, OriginalAodCounts&);

#endif
