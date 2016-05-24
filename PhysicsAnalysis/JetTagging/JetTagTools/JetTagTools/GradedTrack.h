/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_GRADEDTRACK
#define JETTAGTOOLS_GRADEDTRACK

/******************************************************
    @class GradedTrack
********************************************************/

#include "JetTagInfo/TrackGrade.h"
#include <string>
#include <utility>
#include <vector>
#include <iostream>

//#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

class MsgStream;
//namespace xAOD { class TrackParticle; }

namespace Analysis {

class ITrackGradeFactory;

typedef ElementLink<xAOD::TrackParticleContainer> tracklink_t;

class GradedTrack {
 public:

  // GradedTrack(const xAOD::TrackParticle*, TrackGrade);
  GradedTrack(const tracklink_t&, TrackGrade);
  // const xAOD::TrackParticle* track;
  const tracklink_t& track;
  TrackGrade grade;
};
//MsgStream& operator<<(MsgStream& out, const GradedTrack&);
//std::ostream& operator<<(std::ostream& out, const GradedTrack&);


class TrackGradePartition {
 public:
  TrackGradePartition();
  TrackGradePartition(const std::string&,const ITrackGradeFactory&);
  ~TrackGradePartition();
  void add(const TrackGrade&);
  const std::vector<TrackGrade>& grades() const;
  const std::string suffix() const;
  int size() const;
 private:
  std::vector<TrackGrade> m_grades;
};

MsgStream& operator<<(MsgStream& out, const TrackGradePartition&);
std::ostream& operator<<(std::ostream& out, const TrackGradePartition&);


}
#endif // JETTAGTOOLS_GRADEDTRACK
