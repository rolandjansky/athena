/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Description:
//	Class SurveyConstraintPoint
//
// Author List:
//	Tobias Golling
//
//------------------------------------------------------------------------
#include "InDetSurveyConstraintTool/SurveyConstraintPoint.h"

//----------------
// Constructors --
//----------------

SurveyConstraintPoint::SurveyConstraintPoint( ) {
}


SurveyConstraintPoint::SurveyConstraintPoint( const Amg::Vector3D& survey, 
					      const Amg::Vector3D& current )
  :_survey(survey),
   _current(current){
}

// Copy Constructor
SurveyConstraintPoint::SurveyConstraintPoint( const SurveyConstraintPoint& other )
  :_survey(other.survey()),
   _current(other.current()){
}
  
//--------------
// Destructor --
//--------------

SurveyConstraintPoint::~SurveyConstraintPoint( ){
}

//-------------
// Operators --
//-------------

SurveyConstraintPoint& 
SurveyConstraintPoint::operator= ( const SurveyConstraintPoint& rhs){
  if(&rhs != this){
    _survey = rhs.survey();
    _current = rhs.current();
  }
  return *this;
}   
  
bool 
SurveyConstraintPoint::operator==( const SurveyConstraintPoint& rhs ) const{
  return this == &rhs;
}

bool 
SurveyConstraintPoint::operator!=( const SurveyConstraintPoint& rhs ) const{
  return this != &rhs;
}
