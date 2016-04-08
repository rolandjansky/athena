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
  const Amg::Vector3D& survey() const { return _survey; };
  const Amg::Vector3D& current() const { return _current; };
  
// non-const
  Amg::Vector3D& survey() { return _survey; };
  Amg::Vector3D& current() { return _current; };
  
  // Modifiers
  void setSurvey( const Amg::Vector3D& point ) { 
    _survey = point;
  }
 
  void setCurrent( const Amg::Vector3D& point ) { 
    _current = point;
  }
// trasform both points
  void moveTo( const Amg::Transform3D& trans ) { 
    _survey  = trans * _survey;
    _current = trans * _current;
  }

  void moveFrom( const Amg::Transform3D& trans ) { 
    _survey  =  (trans.inverse()) * _survey ;
    _current =  (trans.inverse()) * _current;
  }

  void scaleZ( float scale ) {
    //float before = _survey.z();
    float zs = _survey.z() * scale;
    float zc = _current.z() * scale;
    _survey[2] = zs;
    _current[2] = zc;
    //    cout << "scale, before, after, after = " << scale << "," << before << "," << zs << "," << _survey.z() << endl;
  }

  
protected:

  // Helper functions

private:

  // Data members
  Amg::Vector3D _survey;  //Points
  Amg::Vector3D _current; //Points

};

#endif
