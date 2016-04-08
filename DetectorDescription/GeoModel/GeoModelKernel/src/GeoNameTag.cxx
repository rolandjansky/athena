/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CEA34860084.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CEA34860084.cm

//## begin module%3CEA34860084.cp preserve=no
//## end module%3CEA34860084.cp

//## Module: GeoNameTag%3CEA34860084; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoNameTag.cxx

//## begin module%3CEA34860084.additionalIncludes preserve=no
//## end module%3CEA34860084.additionalIncludes

//## begin module%3CEA34860084.includes preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
//## end module%3CEA34860084.includes

// GeoNameTag
#include "GeoModelKernel/GeoNameTag.h"
//## begin module%3CEA34860084.additionalDeclarations preserve=yes
//## end module%3CEA34860084.additionalDeclarations


// Class GeoNameTag 

GeoNameTag::GeoNameTag (const std::string &BaseName)
  //## begin GeoNameTag::GeoNameTag%3CEA467D013E.hasinit preserve=no
  //## end GeoNameTag::GeoNameTag%3CEA467D013E.hasinit
  //## begin GeoNameTag::GeoNameTag%3CEA467D013E.initialization preserve=yes
:m_name (BaseName)
  //## end GeoNameTag::GeoNameTag%3CEA467D013E.initialization
{
  //## begin GeoNameTag::GeoNameTag%3CEA467D013E.body preserve=yes
  //## end GeoNameTag::GeoNameTag%3CEA467D013E.body
}


GeoNameTag::~GeoNameTag()
{
  //## begin GeoNameTag::~GeoNameTag%3CEA34860084_dest.body preserve=yes
  //## end GeoNameTag::~GeoNameTag%3CEA34860084_dest.body
}



//## Other Operations (implementation)
void GeoNameTag::exec (GeoNodeAction *action) const
{
  //## begin GeoNameTag::exec%3CEA44BC00AC.body preserve=yes
  action->handleNameTag (this);
  //## end GeoNameTag::exec%3CEA44BC00AC.body
}

// Additional Declarations
  //## begin GeoNameTag%3CEA34860084.declarations preserve=yes
  //## end GeoNameTag%3CEA34860084.declarations

//## begin module%3CEA34860084.epilog preserve=yes
//## end module%3CEA34860084.epilog
