
#include "AthExUnittest/AthExUnittestTool.h"

AthExUnittestTool::AthExUnittestTool( const std::string& type,
				      const std::string& name,
				      const IInterface* parent,
				      bool unittest ) : 
  AthAlgTool( type, name, parent ), 
  m_nProperty( 0 ),
  m_enumProperty( IAthExUnittestTool::Val1 ),
  m_unittest( unittest ) {
  //example property declarations with default values
  declareProperty( "Property", m_nProperty = 3.0, 
                   "Please describe the property here" );
  declareProperty( "ENumProperty", m_enumProperty = Val1, 
                   "Please define enums inside your classes, not just in namespaces" );
}

StatusCode AthExUnittestTool::initialize() {
  if( not m_unittest ) {
    ATH_MSG_INFO( "Initializing " << name() << "..." );
    ATH_MSG_INFO( "Property = " << m_nProperty );
    ATH_MSG_INFO( "ENumProperty = " << m_enumProperty );
  }

  //Make use of the property values to configure the tool
  //Tools should be designed so that no method other than setProperty is called before initialize
  return StatusCode::SUCCESS;
}

double AthExUnittestTool::useTheProperty() {
  return m_nProperty;
}

