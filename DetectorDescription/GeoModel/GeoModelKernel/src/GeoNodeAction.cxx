/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF50A901F3.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF50A901F3.cm

//## begin module%3CDF50A901F3.cp preserve=no
//## end module%3CDF50A901F3.cp

//## Module: GeoNodeAction%3CDF50A901F3; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoNodeAction.cxx

//## begin module%3CDF50A901F3.additionalIncludes preserve=no
//## end module%3CDF50A901F3.additionalIncludes

//## begin module%3CDF50A901F3.includes preserve=yes
//## end module%3CDF50A901F3.includes

// GeoNodeAction
#include "GeoModelKernel/GeoNodeAction.h"
//## begin module%3CDF50A901F3.additionalDeclarations preserve=yes
//## end module%3CDF50A901F3.additionalDeclarations


// Class GeoNodeAction 

GeoNodeAction::GeoNodeAction()
  //## begin GeoNodeAction::GeoNodeAction%3CDF50A901F3_const.hasinit preserve=no
  //## end GeoNodeAction::GeoNodeAction%3CDF50A901F3_const.hasinit
  //## begin GeoNodeAction::GeoNodeAction%3CDF50A901F3_const.initialization preserve=yes
:m_terminate (false)
  //## end GeoNodeAction::GeoNodeAction%3CDF50A901F3_const.initialization
{
  //## begin GeoNodeAction::GeoNodeAction%3CDF50A901F3_const.body preserve=yes
  //## end GeoNodeAction::GeoNodeAction%3CDF50A901F3_const.body
}


GeoNodeAction::~GeoNodeAction()
{
  //## begin GeoNodeAction::~GeoNodeAction%3CDF50A901F3_dest.body preserve=yes
  //## end GeoNodeAction::~GeoNodeAction%3CDF50A901F3_dest.body
}



//## Other Operations (implementation)
void GeoNodeAction::handleNode (const GeoGraphNode *)
{
  //## begin GeoNodeAction::handleNode%3CDF578701F8.body preserve=yes
  // Do nothing...    
  //## end GeoNodeAction::handleNode%3CDF578701F8.body
}

void GeoNodeAction::handleTransform (const GeoTransform *)
{
  //## begin GeoNodeAction::handleTransform%3CDF585C01CC.body preserve=yes
  // Do nothing...    
  //## end GeoNodeAction::handleTransform%3CDF585C01CC.body
}

void GeoNodeAction::handlePhysVol (const GeoPhysVol *)
{
  //## begin GeoNodeAction::handlePhysVol%3CDF5952028E.body preserve=yes
  //## end GeoNodeAction::handlePhysVol%3CDF5952028E.body
}

void GeoNodeAction::handleFullPhysVol (const GeoFullPhysVol *)
{
  //## begin GeoNodeAction::handleFullPhysVol%3CE0C90A020C.body preserve=yes
  //## end GeoNodeAction::handleFullPhysVol%3CE0C90A020C.body
}

GeoNodePath* GeoNodeAction::getPath ()
{
  //## begin GeoNodeAction::getPath%3CE10FA40340.body preserve=yes
  return &m_path;
  //## end GeoNodeAction::getPath%3CE10FA40340.body
}

Query<unsigned int>  GeoNodeAction::getDepthLimit ()
{
  //## begin GeoNodeAction::getDepthLimit%3CE1111F024B.body preserve=yes
  return m_depth;
  //## end GeoNodeAction::getDepthLimit%3CE1111F024B.body
}

void GeoNodeAction::terminate ()
{
  //## begin GeoNodeAction::terminate%3CE12B130348.body preserve=yes
  m_terminate = true;
  //## end GeoNodeAction::terminate%3CE12B130348.body
}

bool GeoNodeAction::shouldTerminate () const
{
  //## begin GeoNodeAction::shouldTerminate%3CE12B1C00AC.body preserve=yes
  return m_terminate;
  //## end GeoNodeAction::shouldTerminate%3CE12B1C00AC.body
}

void GeoNodeAction::handleNameTag (const GeoNameTag *)
{
  //## begin GeoNodeAction::handleNameTag%3CEA483D005C.body preserve=yes
  //## end GeoNodeAction::handleNameTag%3CEA483D005C.body
}

void GeoNodeAction::handleSerialDenominator (const GeoSerialDenominator *)
{
  //## begin GeoNodeAction::handleSerialDenominator%3CEA4842008B.body preserve=yes
  //## end GeoNodeAction::handleSerialDenominator%3CEA4842008B.body
}

void GeoNodeAction::setDepthLimit (unsigned int limit)
{
  //## begin GeoNodeAction::setDepthLimit%3CFB2BE301B2.body preserve=yes
  m_depth = limit;
  //## end GeoNodeAction::setDepthLimit%3CFB2BE301B2.body
}

void GeoNodeAction::clearDepthLimit ()
{
  //## begin GeoNodeAction::clearDepthLimit%3CFB2C2A02A5.body preserve=yes
  m_depth = Query < unsigned int >();
  //## end GeoNodeAction::clearDepthLimit%3CFB2C2A02A5.body
}

void GeoNodeAction::handleSerialTransformer (const GeoSerialTransformer  *)
{
  //## begin GeoNodeAction::handleSerialTransformer%3D208C590370.body preserve=yes
  //## end GeoNodeAction::handleSerialTransformer%3D208C590370.body
}

void GeoNodeAction::handleIdentifierTag (const GeoIdentifierTag *)
{
  //## begin GeoNodeAction::handleIdentifierTag%3E217DC1036B.body preserve=yes
  //## end GeoNodeAction::handleIdentifierTag%3E217DC1036B.body
}

// Additional Declarations
  //## begin GeoNodeAction%3CDF50A901F3.declarations preserve=yes
void GeoNodeAction::handleSerialIdentifier(const GeoSerialIdentifier *)
{
}
  //## end GeoNodeAction%3CDF50A901F3.declarations
//## begin module%3CDF50A901F3.epilog preserve=yes
//## end module%3CDF50A901F3.epilog
