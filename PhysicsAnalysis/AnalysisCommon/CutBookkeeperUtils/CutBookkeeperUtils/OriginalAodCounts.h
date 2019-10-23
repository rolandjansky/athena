/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ORIGINALAODCOUNTS_H
#define ORIGINALAODCOUNTS_H

namespace xAOD {
  class TEvent;
}
class StatusCode;

// configuration options
struct AodCountsConfig {
  AodCountsConfig();
  // empty structure, can add configuration here
};
inline const AodCountsConfig DEFAULT_AOD_COUNTS_CONFIG;

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

OriginalAodCounts getOriginalAodCounts(
  xAOD::TEvent& event,
  const AodCountsConfig& = DEFAULT_AOD_COUNTS_CONFIG);

#endif
