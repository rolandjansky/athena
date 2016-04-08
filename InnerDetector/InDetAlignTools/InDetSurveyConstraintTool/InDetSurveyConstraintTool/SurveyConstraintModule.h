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
#ifndef SURVEYCONSTRAINTMODULE_HH
#define SURVEYCONSTRAINTMODULE_HH  

#include "Identifier/Identifier.h"
#include "InDetSurveyConstraintTool/SurveyConstraintPoint.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"


#include <vector>

class SurveyConstraintModule {

 public:
  enum ModuleStatus {Module=0,Stave};
  
  //constructor
  SurveyConstraintModule();                                                    //!< defaukt constructor
  SurveyConstraintModule(const Identifier&);                                   //!< constructor  
  SurveyConstraintModule(const Identifier&, bool);                             //!< constructor  
  SurveyConstraintModule(const SurveyConstraintModule&);                       //!< copy constructor
  
  SurveyConstraintModule &operator= (const SurveyConstraintModule &);          //!< assignment operator

  //destructor
  ~SurveyConstraintModule ();                                                  //!< destructor
  
  // get methods
  Amg::VectorX DOCA_Vector() const;
  Amg::MatrixX DOCA_Matrix() const;
  Identifier moduleID() const;
  bool isPixel();
  Amg::Transform3D get_globaltolocal();
  unsigned int nModulePoints();
  unsigned int nStavePoints();
  
  // Selectors (const)
  void getPoints( std::vector<SurveyConstraintPoint>&,
		  ModuleStatus mstat) const;

  // Modifiers
  void addStaveConstraintPoint( std::vector<SurveyConstraintPoint>&  cPoints );
  void addModuleConstraintPoint( const SurveyConstraintPoint& cPoint );
  void set_globaltolocal(Amg::Transform3D& globaltolocal);
  void set_DOCA_Vector(Amg::VectorX& DOCA_Vector);
  
 private:
  
  Amg::VectorX m_DOCA_Vector;
  Amg::MatrixX m_DOCA_Matrix;
  Identifier m_ModuleID;
  bool m_isPixel;
  Amg::Transform3D m_globaltolocal;
  std::vector<SurveyConstraintPoint> m_StavePoints;
  std::vector<SurveyConstraintPoint> m_ModulePoints;
};

  // inline
  inline Amg::VectorX SurveyConstraintModule::DOCA_Vector() const
  {
    return m_DOCA_Vector;
  }
  inline Amg::MatrixX SurveyConstraintModule::DOCA_Matrix() const
  {
    return m_DOCA_Matrix;
  }
  inline Identifier SurveyConstraintModule::moduleID() const
  {
    return m_ModuleID;
  }  
  inline bool SurveyConstraintModule::isPixel()
  {  
    return m_isPixel;
  }
  inline Amg::Transform3D SurveyConstraintModule::get_globaltolocal()
  {  
    return m_globaltolocal;
  }
  inline unsigned int SurveyConstraintModule::nModulePoints()
  {  
    return m_ModulePoints.size();
  }
  inline unsigned int SurveyConstraintModule::nStavePoints()
  {  
    return m_StavePoints.size();
  }

#endif
