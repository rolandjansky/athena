/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CE222400350.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CE222400350.cm

//## begin module%3CE222400350.cp preserve=no
//## end module%3CE222400350.cp

//## Module: TemplateVolAction%3CE222400350; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/TemplateVolAction.cxx

//## begin module%3CE222400350.additionalIncludes preserve=no
//## end module%3CE222400350.additionalIncludes

//## begin module%3CE222400350.includes preserve=yes
//## end module%3CE222400350.includes

// TemplateVolAction
#include "GeoModelKernel/TemplateVolAction.h"
//## begin module%3CE222400350.additionalDeclarations preserve=yes
//## end module%3CE222400350.additionalDeclarations


// Class TemplateVolAction 

TemplateVolAction::TemplateVolAction ()
  //## begin TemplateVolAction::TemplateVolAction%3CE2391403B5.hasinit preserve=no
  //## end TemplateVolAction::TemplateVolAction%3CE2391403B5.hasinit
  //## begin TemplateVolAction::TemplateVolAction%3CE2391403B5.initialization preserve=yes
:GeoVolumeAction (GeoVolumeAction::TOP_DOWN)
  //## end TemplateVolAction::TemplateVolAction%3CE2391403B5.initialization
{
  //## begin TemplateVolAction::TemplateVolAction%3CE2391403B5.body preserve=yes
  //## end TemplateVolAction::TemplateVolAction%3CE2391403B5.body
}


TemplateVolAction::~TemplateVolAction()
{
  //## begin TemplateVolAction::~TemplateVolAction%3CE222400350_dest.body preserve=yes
  //## end TemplateVolAction::~TemplateVolAction%3CE222400350_dest.body
}



//## Other Operations (implementation)
void TemplateVolAction::handleVPhysVol (const GeoVPhysVol *)
{
  //## begin TemplateVolAction::handleVPhysVol%3CE2357D018D.body preserve=yes
  //## end TemplateVolAction::handleVPhysVol%3CE2357D018D.body
}

// Additional Declarations
  //## begin TemplateVolAction%3CE222400350.declarations preserve=yes
  //## end TemplateVolAction%3CE222400350.declarations

//## begin module%3CE222400350.epilog preserve=yes
//## end module%3CE222400350.epilog
