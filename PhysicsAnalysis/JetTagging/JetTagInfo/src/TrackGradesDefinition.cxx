/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    std::vector<TrackGrade>::const_iterator begin=m_gradeList.begin();
    std::vector<TrackGrade>::const_iterator end=m_gradeList.end();

    for (std::vector<TrackGrade>::const_iterator iter=begin;iter!=end;
	 iter++) {

      const std::string & gradeString=(*iter).gradeString();

      if (gradeString==myString) {
	return (*iter).gradeNumber();
      }
    }
    return -1;
  }

  
  std::string TrackGradesDefinition::grade(int myNumber) const {

    std::vector<TrackGrade>::const_iterator begin=m_gradeList.begin();
    std::vector<TrackGrade>::const_iterator end=m_gradeList.end();

    for (std::vector<TrackGrade>::const_iterator iter=begin;iter!=end;
	 iter++) {

      int gradeNumber=(*iter).gradeNumber();

      if (gradeNumber==myNumber) {
	return (*iter).gradeString();
      }
    }
    return std::string("Undefined");
  }

    
  const TrackGrade * TrackGradesDefinition::getGrade(const std::string & myString) const
  {
    
    std::vector<TrackGrade>::const_iterator begin=m_gradeList.begin();
    std::vector<TrackGrade>::const_iterator end=m_gradeList.end();
    
    for (std::vector<TrackGrade>::const_iterator iter=begin;iter!=end;
	 iter++) {

      const std::string & gradeString=(*iter).gradeString();

      if (gradeString==myString) {
	return &(*iter);
      }
    }
    return 0;
  }
  
  
  const TrackGrade * TrackGradesDefinition::getGrade(const int & myNumber) const
  {
    
    std::vector<TrackGrade>::const_iterator begin=m_gradeList.begin();
    std::vector<TrackGrade>::const_iterator end=m_gradeList.end();
    
    for (std::vector<TrackGrade>::const_iterator iter=begin;iter!=end;
	 iter++) {

      int gradeNumber=(*iter).gradeNumber();

      if (gradeNumber==myNumber) {
	return &(*iter);
      }
    }
    return 0;
  }
  
}

