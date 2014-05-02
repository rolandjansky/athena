/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD25BA0113.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD25BA0113.cm

//## begin module%3CDD25BA0113.cp preserve=no
//## end module%3CDD25BA0113.cp

//## Module: GeoLogVol%3CDD25BA0113; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoLogVol.cxx

//## begin module%3CDD25BA0113.additionalIncludes preserve=no
//## end module%3CDD25BA0113.additionalIncludes

//## begin module%3CDD25BA0113.includes preserve=yes
//## end module%3CDD25BA0113.includes

// GeoLogVol
#include "GeoModelKernel/GeoLogVol.h"
//## begin module%3CDD25BA0113.additionalDeclarations preserve=yes
//## end module%3CDD25BA0113.additionalDeclarations


// Class GeoLogVol 

GeoLogVol::GeoLogVol (const std::string &Name, const GeoShape *Shape, const GeoMaterial *Material)
  //## begin GeoLogVol::GeoLogVol%3CDD2AB7008D.hasinit preserve=no
  //## end GeoLogVol::GeoLogVol%3CDD2AB7008D.hasinit
  //## begin GeoLogVol::GeoLogVol%3CDD2AB7008D.initialization preserve=yes
  :
_name (Name),
material (Material),
shape (Shape)
  //## end GeoLogVol::GeoLogVol%3CDD2AB7008D.initialization
{
  //## begin GeoLogVol::GeoLogVol%3CDD2AB7008D.body preserve=yes
  material->ref ();
  shape->ref ();
  //## end GeoLogVol::GeoLogVol%3CDD2AB7008D.body
}


GeoLogVol::~GeoLogVol()
{
  //## begin GeoLogVol::~GeoLogVol%3CDD25BA0113_dest.body preserve=yes
  material->unref ();
  shape->unref ();
  //## end GeoLogVol::~GeoLogVol%3CDD25BA0113_dest.body
}


// Additional Declarations
  //## begin GeoLogVol%3CDD25BA0113.declarations preserve=yes
  //## end GeoLogVol%3CDD25BA0113.declarations

//## begin module%3CDD25BA0113.epilog preserve=yes
//## end module%3CDD25BA0113.epilog
