/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_INDETTRACKSUMMARY_H
#define TRK_INDETTRACKSUMMARY_H

#include "Identifier/Identifier.h"

class MsgStream;
class InDetTrackSummaryCnv_p1;
class TrackSummaryCnv_p2;

namespace InDet {
class InDetTrackSummaryHelperTool;
}

namespace Trk {

/**
    Detailed track summary for the indet system
*/
class InDetTrackSummary
{
public:
  friend class InDet::InDetTrackSummaryHelperTool;
  friend class ::InDetTrackSummaryCnv_p1;
  friend class ::TrackSummaryCnv_p2;

  /** default constructor */
  [[deprecated]]   InDetTrackSummary();
  [[deprecated]]   InDetTrackSummary(const InDetTrackSummary&) = default;
  [[deprecated]]   InDetTrackSummary(InDetTrackSummary&&) = default;
  [[deprecated]]   InDetTrackSummary& operator=(const InDetTrackSummary&) = default;
  [[deprecated]]   InDetTrackSummary& operator=(InDetTrackSummary&&) = default;
  /** destructor */
  ~InDetTrackSummary() = default;

  /** access to the vector of likelihoods (pion/kaon/proton) */
  [[deprecated]] const std::vector<float>& likelihoodsPixeldEdx() const
  {
    return m_likelihoodspixeldedx;
  }

  /** access to mass calculated using pixel dEdx */
  [[deprecated]] float massPixeldEdx() const { return m_massdedx; }

private:
  std::vector<float> m_likelihoodspixeldedx; //<! vector with likelihoods
  float m_massdedx; //<! mass calculated using pixel dEdx
};

/**output. This dumps the values of each of the possible summary enums*/
[[deprecated]] MsgStream&
operator<<(MsgStream& out, const InDetTrackSummary& trackSum);

/**output. This dumps the values of each of the possible summary enums*/
[[deprecated]] std::ostream&
operator<<(std::ostream& out, const InDetTrackSummary& trackSum);

}
#endif
