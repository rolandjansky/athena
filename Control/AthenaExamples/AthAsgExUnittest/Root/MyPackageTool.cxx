
//
//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// MyPackage includes
#include "AthAsgExUnittest/MyPackageTool.h"

MyPackageTool::MyPackageTool( const std::string& name ) : asg::AsgTool( name ) {
  //example property declarations with default values
  declareProperty( "Property", m_nProperty = 3.0,
                   "Please describe the property here" );
  declareProperty( "ENumProperty", m_enumProperty = Val1,
                   "Please define enums inside your classes, not just in namespaces" );
}

StatusCode MyPackageTool::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << "..." );
    ATH_MSG_INFO( "Property = " << m_nProperty );
  //
  //Make use of the property values to configure the tool
  //Tools should be designed so that no method other than setProperty is called before initialize
  //
  return StatusCode::SUCCESS;
}

double MyPackageTool::useTheProperty() {
  return m_nProperty;
}
