/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CE3D68701E6.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CE3D68701E6.cm

//## begin module%3CE3D68701E6.cp preserve=no
//## end module%3CE3D68701E6.cp

//## Module: GeoClearAbsPosAction%3CE3D68701E6; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoClearAbsPosAction.cxx

//## begin module%3CE3D68701E6.additionalIncludes preserve=no
//## end module%3CE3D68701E6.additionalIncludes

//## begin module%3CE3D68701E6.includes preserve=yes
//## end module%3CE3D68701E6.includes

// GeoClearAbsPosAction
#include "GeoModelKernel/GeoClearAbsPosAction.h"
//## begin module%3CE3D68701E6.additionalDeclarations preserve=yes
//## end module%3CE3D68701E6.additionalDeclarations


// Class GeoClearAbsPosAction 

GeoClearAbsPosAction::GeoClearAbsPosAction()
  //## begin GeoClearAbsPosAction::GeoClearAbsPosAction%3CE3D68701E6_const.hasinit preserve=no
  //## end GeoClearAbsPosAction::GeoClearAbsPosAction%3CE3D68701E6_const.hasinit
  //## begin GeoClearAbsPosAction::GeoClearAbsPosAction%3CE3D68701E6_const.initialization preserve=yes
  //## end GeoClearAbsPosAction::GeoClearAbsPosAction%3CE3D68701E6_const.initialization
{
  //## begin GeoClearAbsPosAction::GeoClearAbsPosAction%3CE3D68701E6_const.body preserve=yes
  //## end GeoClearAbsPosAction::GeoClearAbsPosAction%3CE3D68701E6_const.body
}


GeoClearAbsPosAction::~GeoClearAbsPosAction()
{
  //## begin GeoClearAbsPosAction::~GeoClearAbsPosAction%3CE3D68701E6_dest.body preserve=yes
  //## end GeoClearAbsPosAction::~GeoClearAbsPosAction%3CE3D68701E6_dest.body
}



//## Other Operations (implementation)
void GeoClearAbsPosAction::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  //## begin GeoClearAbsPosAction::handleFullPhysVol%3CE3D721001A.body preserve=yes
  vol->clearPositionInfo ();
  //## end GeoClearAbsPosAction::handleFullPhysVol%3CE3D721001A.body
}

// Additional Declarations
  //## begin GeoClearAbsPosAction%3CE3D68701E6.declarations preserve=yes
  //## end GeoClearAbsPosAction%3CE3D68701E6.declarations

//## begin module%3CE3D68701E6.epilog preserve=yes
//## end module%3CE3D68701E6.epilog
