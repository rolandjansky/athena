/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_TRACKGRADESDEFINITION
#define JETTAGINFO_TRACKGRADESDEFINITION

#include "JetTagInfo/TrackGrade.h"

#include <vector>
#include <string>

namespace Analysis {
  
  class TrackGradesDefinition {
    
  public:
    
    TrackGradesDefinition();
    
    TrackGradesDefinition(std::vector<TrackGrade> &);
    virtual ~TrackGradesDefinition();
    
    TrackGradesDefinition(const TrackGradesDefinition &);

    /** assigenment operator */
    TrackGradesDefinition &operator= (const TrackGradesDefinition& rhs);
    
    const std::vector<TrackGrade> & getList() const;
    
    const TrackGrade * getGrade(const std::string &) const;

    const TrackGrade * getGrade(const int &) const;

    int grade(const std::string &) const;
    
    std::string grade(int) const;

    int numberOfGrades() const;
    
  private:
    
    std::vector<TrackGrade> m_gradeList;
  };
  
  inline const std::vector<TrackGrade> & TrackGradesDefinition::getList() const
  {
    return m_gradeList;
  }

  inline int TrackGradesDefinition::numberOfGrades() const
  {
    return m_gradeList.size();
  }
  

}
#endif // JETTAGINFO_TRACKGRADE
