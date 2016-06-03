/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/GradedTrack.h"
#include "JetTagInfo/TrackGrade.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <sys/types.h>

#include "JetTagTools/ITrackGradeFactory.h"
#include "JetTagInfo/TrackGradesDefinition.h"

namespace Analysis {

  // GradedTrack::GradedTrack(const xAOD::TrackParticle* trk, TrackGrade grd) {
  GradedTrack::GradedTrack(const tracklink_t& trk, TrackGrade grd) :
    track(trk), grade(grd) {
    // this->track = trk;
    // this->grade = grd;
  }

  TrackGradePartition::TrackGradePartition() {
  }

  TrackGradePartition::TrackGradePartition(const std::string& definition,
                                           const ITrackGradeFactory& trackGradeFactory) {

    const TrackGradesDefinition & myTrackGradesDefinition = trackGradeFactory.getTrackGradesDefinition();
    
    // parse definition to extract grades separated by + sign:
    const std::string delim("+");
    std::string::size_type sPos, sEnd, sLen;
    sPos = definition.find_first_not_of(delim);
    while ( sPos != std::string::npos ) {
      sEnd = definition.find_first_of(delim, sPos);
      if(sEnd==std::string::npos) sEnd = definition.length();
      sLen = sEnd - sPos;
      std::string grade = definition.substr(sPos,sLen);
      // add new grade to this partition:
      const TrackGrade* tgrade(myTrackGradesDefinition.getGrade(grade));
      if (tgrade==0)
      {
        throw std::string("#BTAG# Error: the following grade ") + grade + std::string(" could not be found in the list from TrackGradeFactory ");
      }
      m_grades.push_back(TrackGrade(*tgrade));
      sPos = definition.find_first_not_of(delim, sEnd);
    }
  }

  TrackGradePartition::~TrackGradePartition() {
  }

  void TrackGradePartition::add(const TrackGrade& grade) {
    m_grades.push_back(grade);
  }

  const std::vector<TrackGrade>& TrackGradePartition::grades() const {
    return m_grades;
  }

  const std::string TrackGradePartition::suffix() const {
    std::string suf("");
    for(uint i=0;i<m_grades.size();i++) {
      if(0==i) {
        suf += m_grades[i].gradeString();
      } else {
        suf += ("_" + m_grades[i].gradeString());
      }
    }
    return suf;
  }

  int TrackGradePartition::size() const {
    return m_grades.size();
  }

  MsgStream& operator<<( MsgStream& out, const TrackGradePartition& part) {
    out << "#BTAG# GradePartition: [ ";
    for(int i=0;i<part.size();i++) out<<" "<<part.grades()[i].gradeString();
    out << " ]";
    return out;
  }
  std::ostream& operator<<( std::ostream& out, const TrackGradePartition& part) {
    out << "#BTAG# GradePartition: [ ";
    for(int i=0;i<part.size();i++) out<<" "<<part.grades()[i].gradeString();
    out << " ]";
    return out;
  }
}
