/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/TrackGrade.h"
#include "GaudiKernel/MsgStream.h"
#include <utility>
#include <iostream>

namespace Analysis {

  TrackGrade::TrackGrade() {
    m_gradeNumber = -1;
    m_gradeName = "Undefined";
}
  
  TrackGrade::TrackGrade(int number,const std::string& name) {
    m_gradeNumber = number;
    m_gradeName = name;
  }
  
  TrackGrade::TrackGrade(const TrackGrade & rhs)
          : m_gradeNumber(rhs.m_gradeNumber),
            m_gradeName(rhs.m_gradeName)
  {
  }

  TrackGrade & TrackGrade::operator= (const TrackGrade& rhs)
  {
    if (this!=&rhs)
    {
      m_gradeName=rhs.m_gradeName;
      m_gradeNumber=rhs.m_gradeNumber;
    }
    return *this;
  }

  TrackGrade::~TrackGrade() {
  }
  
  const std::string & TrackGrade::gradeString() const {
    return m_gradeName;
  }
  
  int TrackGrade::gradeNumber() const {
    return m_gradeNumber;
  }
  
  TrackGrade::operator int () const {
    return m_gradeNumber;
  }

  TrackGrade::operator const std::string () const {
    return m_gradeName;
  }

  bool TrackGrade::operator== (const std::string & rhs) const
  {
    return (m_gradeName==rhs);
  }
      

  bool TrackGrade::operator== (const int & rhs) const
  {
    return (m_gradeNumber==rhs);
  }

  bool TrackGrade::operator== (const TrackGrade& rhs) const
  {
    if (&rhs == this) return true;

    if (rhs.m_gradeNumber == m_gradeNumber && 
        rhs.m_gradeName == m_gradeName) 
    {
      return true;
    }
    return false;
  }
}

