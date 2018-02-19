/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------------------------------
// Description:
//	Class SurveyConstraintPoint.  Descripts points in global coordinates
//      for the survey markers in 2 different alignments, current and survey.
//
// Author List:
//      Tobias Golling
//
//------------------------------------------------------------------------

#ifndef SURVEYCONSTRAINTPOINT_HH
#define SURVEYCONSTRAINTPOINT_HH

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <iostream>
using std::cout;
using std::endl;

class SurveyConstraintPoint {

//--------------------
// Instance Members --
//--------------------

public:
  SurveyConstraintPoint();
  // Constructors  
  SurveyConstraintPoint( const Amg::Vector3D& survey, 
			 const Amg::Vector3D& current );

  // Copy Constructor
  SurveyConstraintPoint( const SurveyConstraintPoint& cpoint );
  
  // Destructor
  virtual ~SurveyConstraintPoint( );
  
  // Operators  
  SurveyConstraintPoint& operator= ( const SurveyConstraintPoint& );
  bool operator==( const SurveyConstraintPoint& ) const;
  bool operator!=( const SurveyConstraintPoint& ) const;
  
  // Selectors (const)
  const Amg::Vector3D& survey() const { return m_survey; };
  const Amg::Vector3D& current() const { return m_current; };
  
// non-const
  Amg::Vector3D& survey() { return m_survey; };
  Amg::Vector3D& current() { return m_current; };
  
  // Modifiers
  void setSurvey( const Amg::Vector3D& point ) { 
    m_survey = point;
  }
 
  void setCurrent( const Amg::Vector3D& point ) { 
    m_current = point;
  }
// trasform both points
  void moveTo( const Amg::Transform3D& trans ) { 
    m_survey  = trans * m_survey;
    m_current = trans * m_current;
  }

  void moveFrom( const Amg::Transform3D& trans ) { 
    m_survey  =  (trans.inverse()) * m_survey ;
    m_current =  (trans.inverse()) * m_current;
  }

  void scaleZ( float scale ) {
    //float before = m_survey.z();
    float zs = m_survey.z() * scale;
    float zc = m_current.z() * scale;
    m_survey[2] = zs;
    m_current[2] = zc;
    //    cout << "scale, before, after, after = " << scale << "," << before << "," << zs << "," << m_survey.z() << endl;
  }

  
protected:

  // Helper functions

private:

  // Data members
  Amg::Vector3D m_survey;  //Points
  Amg::Vector3D m_current; //Points

};

#endif
