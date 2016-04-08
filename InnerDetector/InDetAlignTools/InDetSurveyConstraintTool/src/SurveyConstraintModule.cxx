/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------------------------------
// Description:
//	Class SurveyConstraintModule:  Provides Bookkeeping for Survey 
// Constraint, Represents a single module
//
// Author List:
//	Tobias Golling
//------------------------------------------------------------------------
#include "InDetSurveyConstraintTool/SurveyConstraintModule.h"
using std::cout;
using std::endl;
//----------------
// Constructors --
//----------------

// default ctor
SurveyConstraintModule::SurveyConstraintModule():
  m_DOCA_Vector(6),
  m_DOCA_Matrix(6,6),
  m_ModuleID(0),
  m_isPixel(true)
{

}


// Constructor
SurveyConstraintModule::SurveyConstraintModule(const Identifier& moduleID):
  m_DOCA_Vector(6),
  m_DOCA_Matrix(6,6),
  m_ModuleID(moduleID),
  m_isPixel(true)
{

}


// Constructor
SurveyConstraintModule::SurveyConstraintModule(const Identifier& moduleID, bool isPixel):
  m_DOCA_Vector(6),
  m_DOCA_Matrix(6,6),
  m_ModuleID(moduleID),
  m_isPixel(isPixel)
{

}

//__________________________________________________________________________
// Copy Constructor
SurveyConstraintModule::SurveyConstraintModule(const SurveyConstraintModule& rhs):
  m_DOCA_Vector(6),
  m_DOCA_Matrix(6,6),
  m_ModuleID(0),
  m_isPixel(true)
{
  if (this!=&rhs) {
    (*this)=rhs;
  }
}

//__________________________________________________________________________
// Assignment Operator
SurveyConstraintModule& SurveyConstraintModule::operator= (const SurveyConstraintModule& rhs)
{
  if (this!=&rhs) { // protect against self assignment
    // delete old elements

    // initialize new elements

    // copy in new elements;
    m_DOCA_Vector = rhs.DOCA_Vector();
    m_DOCA_Matrix = rhs.DOCA_Matrix();
    m_ModuleID = rhs.moduleID();
  }

  return *this;
}
//__________________________________________________________________________
// Destructor
SurveyConstraintModule::~SurveyConstraintModule()
{

}

//-------------
// Selectors --
//-------------
void SurveyConstraintModule::getPoints(std::vector<SurveyConstraintPoint>& cPoints,
				       ModuleStatus mstat) const {
  cPoints.clear();
  //cout << "SurveyConstraintModule.getPoints(): m_ModulePoints.size() " << m_ModulePoints.size() << endl;
  //cout << "SurveyConstraintModule.getPoints(): m_Stavepoints.size() " << m_StavePoints.size() << endl;
  if(mstat == 0)
    for (unsigned int iPoint(0); iPoint < m_ModulePoints.size(); ++iPoint ) {
      cPoints.push_back( m_ModulePoints[iPoint] );
      //cout << "mstat = " << mstat << ", SurveyConstraintModule.getPoints(): cPoints.size() " << cPoints.size() << endl;
    }
  if(mstat == 1)
    for (unsigned int iPoint(0); iPoint < m_StavePoints.size(); ++iPoint ) {
      cPoints.push_back( m_StavePoints[iPoint] );
      //cout << "mstat = " << mstat << ", SurveyConstraintModule.getPoints(): cPoints.size() " << cPoints.size() << endl;
    }
}


// Modifiers
void SurveyConstraintModule::addStaveConstraintPoint( std::vector<SurveyConstraintPoint>&  cPoints ){
  for (unsigned int iPoint(0); iPoint < cPoints.size(); ++iPoint ) {
    m_StavePoints.push_back(cPoints[iPoint]);
    //cout << "SurveyConstraintModule.addStaveConstraintPoint(): m_Stavepoints.size() " << m_StavePoints.size() << endl;
  }
}

void SurveyConstraintModule::addModuleConstraintPoint( const SurveyConstraintPoint& cPoint ){
  m_ModulePoints.push_back(cPoint);
  //cout << "SurveyConstraintModule.addModuleConstraintPoint(): m_ModulePoints.size() " << m_ModulePoints.size() << endl;
}

void SurveyConstraintModule::set_globaltolocal(Amg::Transform3D& globaltolocal){
  m_globaltolocal = globaltolocal;}

void SurveyConstraintModule::set_DOCA_Vector(Amg::VectorX& DOCA_Vector){
  m_DOCA_Vector = DOCA_Vector;}
