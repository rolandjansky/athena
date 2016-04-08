/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3E21775800E1.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E21775800E1.cm

//## begin module%3E21775800E1.cp preserve=no
//## end module%3E21775800E1.cp

//## Module: GeoIdentifierTag%3E21775800E1; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoIdentifierTag.cxx

//## begin module%3E21775800E1.additionalIncludes preserve=no
//## end module%3E21775800E1.additionalIncludes

//## begin module%3E21775800E1.includes preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
//## end module%3E21775800E1.includes

// GeoIdentifierTag
#include "GeoModelKernel/GeoIdentifierTag.h"
//## begin module%3E21775800E1.additionalDeclarations preserve=yes
//## end module%3E21775800E1.additionalDeclarations


// Class GeoIdentifierTag 

GeoIdentifierTag::GeoIdentifierTag (int id)
  //## begin GeoIdentifierTag::GeoIdentifierTag%3E21795602D9.hasinit preserve=no
  //## end GeoIdentifierTag::GeoIdentifierTag%3E21795602D9.hasinit
  //## begin GeoIdentifierTag::GeoIdentifierTag%3E21795602D9.initialization preserve=yes
  :m_identifier(id)
  //## end GeoIdentifierTag::GeoIdentifierTag%3E21795602D9.initialization
{
  //## begin GeoIdentifierTag::GeoIdentifierTag%3E21795602D9.body preserve=yes
  //## end GeoIdentifierTag::GeoIdentifierTag%3E21795602D9.body
}


GeoIdentifierTag::~GeoIdentifierTag()
{
  //## begin GeoIdentifierTag::~GeoIdentifierTag%3E21775800E1_dest.body preserve=yes
  //## end GeoIdentifierTag::~GeoIdentifierTag%3E21775800E1_dest.body
}



//## Other Operations (implementation)
void GeoIdentifierTag::exec (GeoNodeAction *action) const
{
  //## begin GeoIdentifierTag::exec%3E217932038B.body preserve=yes
    action->handleIdentifierTag (this);
  //## end GeoIdentifierTag::exec%3E217932038B.body
}

// Additional Declarations
  //## begin GeoIdentifierTag%3E21775800E1.declarations preserve=yes
  //## end GeoIdentifierTag%3E21775800E1.declarations

//## begin module%3E21775800E1.epilog preserve=yes
//## end module%3E21775800E1.epilog
