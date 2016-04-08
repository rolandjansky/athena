/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CEA349303CC.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CEA349303CC.cm

//## begin module%3CEA349303CC.cp preserve=no
//## end module%3CEA349303CC.cp

//## Module: GeoSerialDenominator%3CEA349303CC; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoSerialDenominator.cxx

//## begin module%3CEA349303CC.additionalIncludes preserve=no
//## end module%3CEA349303CC.additionalIncludes

//## begin module%3CEA349303CC.includes preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
//## end module%3CEA349303CC.includes

// GeoSerialDenominator
#include "GeoModelKernel/GeoSerialDenominator.h"
//## begin module%3CEA349303CC.additionalDeclarations preserve=yes
//## end module%3CEA349303CC.additionalDeclarations


// Class GeoSerialDenominator 

GeoSerialDenominator::GeoSerialDenominator (const std::string &BaseName)
  //## begin GeoSerialDenominator::GeoSerialDenominator%3CEA45B1016D.hasinit preserve=no
  //## end GeoSerialDenominator::GeoSerialDenominator%3CEA45B1016D.hasinit
  //## begin GeoSerialDenominator::GeoSerialDenominator%3CEA45B1016D.initialization preserve=yes
:
m_baseName (BaseName)
  //## end GeoSerialDenominator::GeoSerialDenominator%3CEA45B1016D.initialization
{
  //## begin GeoSerialDenominator::GeoSerialDenominator%3CEA45B1016D.body preserve=yes
  //## end GeoSerialDenominator::GeoSerialDenominator%3CEA45B1016D.body
}


GeoSerialDenominator::~GeoSerialDenominator()
{
  //## begin GeoSerialDenominator::~GeoSerialDenominator%3CEA349303CC_dest.body preserve=yes
  //## end GeoSerialDenominator::~GeoSerialDenominator%3CEA349303CC_dest.body
}



//## Other Operations (implementation)
void GeoSerialDenominator::exec (GeoNodeAction *action) const
{
  //## begin GeoSerialDenominator::exec%3CEA44D600BE.body preserve=yes
  action->handleSerialDenominator (this);
  //## end GeoSerialDenominator::exec%3CEA44D600BE.body
}

// Additional Declarations
  //## begin GeoSerialDenominator%3CEA349303CC.declarations preserve=yes
  //## end GeoSerialDenominator%3CEA349303CC.declarations

//## begin module%3CEA349303CC.epilog preserve=yes
//## end module%3CEA349303CC.epilog
