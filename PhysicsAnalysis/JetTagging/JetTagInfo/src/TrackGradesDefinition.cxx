/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/TrackGradesDefinition.h"
#include "GaudiKernel/MsgStream.h"
#include <utility>
#include <iostream>

namespace Analysis {

  TrackGradesDefinition::TrackGradesDefinition() {}
  
  TrackGradesDefinition::TrackGradesDefinition(std::vector<TrackGrade> & trackGradeList)
    : m_gradeList(trackGradeList)
  {}
  
  TrackGradesDefinition::~TrackGradesDefinition() {}

  TrackGradesDefinition::TrackGradesDefinition(const TrackGradesDefinition & rhs)
          : m_gradeList(rhs.m_gradeList)
  {}

  TrackGradesDefinition & TrackGradesDefinition::operator= (const TrackGradesDefinition& rhs)
  {
    if (this!=&rhs)
    {
      m_gradeList=rhs.m_gradeList;
    }
    return *this;
  }

  int TrackGradesDefinition::grade(const std::string & myString) const {

    for (const TrackGrade& grade : m_gradeList) {
      const std::string & gradeString=grade.gradeString();

      if (gradeString==myString) {
	return grade.gradeNumber();
      }
    }
    return -1;
  }

  
  std::string TrackGradesDefinition::grade(int myNumber) const {

    for (const TrackGrade& grade : m_gradeList) {
      int gradeNumber=grade.gradeNumber();

      if (gradeNumber==myNumber) {
	return grade.gradeString();
      }
    }
    return std::string("Undefined");
  }

    
  const TrackGrade * TrackGradesDefinition::getGrade(const std::string & myString) const
  {
    
    for (const TrackGrade& grade : m_gradeList) {
      const std::string & gradeString=grade.gradeString();

      if (gradeString==myString) {
	return &grade;
      }
    }
    return 0;
  }
  
  
  const TrackGrade * TrackGradesDefinition::getGrade(const int & myNumber) const
  {
    
    for (const TrackGrade& grade : m_gradeList) {
      int gradeNumber=grade.gradeNumber();

      if (gradeNumber==myNumber) {
	return &grade;
      }
    }
    return 0;
  }
  
}

