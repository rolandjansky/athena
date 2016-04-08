/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF59FE02DC.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF59FE02DC.cm

//## begin module%3CDF59FE02DC.cp preserve=no
//## end module%3CDF59FE02DC.cp

//## Module: GeoCountVolAction%3CDF59FE02DC; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoCountVolAction.cxx

//## begin module%3CDF59FE02DC.additionalIncludes preserve=no
//## end module%3CDF59FE02DC.additionalIncludes

//## begin module%3CDF59FE02DC.includes preserve=yes
//## end module%3CDF59FE02DC.includes

// GeoCountVolAction
#include "GeoModelKernel/GeoCountVolAction.h"
//## begin module%3CDF59FE02DC.additionalDeclarations preserve=yes
//## end module%3CDF59FE02DC.additionalDeclarations


// Class GeoCountVolAction 

GeoCountVolAction::GeoCountVolAction ()
  //## begin GeoCountVolAction::GeoCountVolAction%3CDF5B0F031A.hasinit preserve=no
  //## end GeoCountVolAction::GeoCountVolAction%3CDF5B0F031A.hasinit
  //## begin GeoCountVolAction::GeoCountVolAction%3CDF5B0F031A.initialization preserve=yes
:m_count (0)
  //## end GeoCountVolAction::GeoCountVolAction%3CDF5B0F031A.initialization
{
  //## begin GeoCountVolAction::GeoCountVolAction%3CDF5B0F031A.body preserve=yes
  setDepthLimit (1);
  //## end GeoCountVolAction::GeoCountVolAction%3CDF5B0F031A.body
}


GeoCountVolAction::~GeoCountVolAction()
{
  //## begin GeoCountVolAction::~GeoCountVolAction%3CDF59FE02DC_dest.body preserve=yes
  //## end GeoCountVolAction::~GeoCountVolAction%3CDF59FE02DC_dest.body
}



//## Other Operations (implementation)
void GeoCountVolAction::handlePhysVol (const GeoPhysVol *)
{
  //## begin GeoCountVolAction::handlePhysVol%3CE0CFC4014A.body preserve=yes
  //    
  // Do not count the top volume, this action counts only children!   
  //   
  if (getPath ()->getLength () > 1)
    m_count++;
  //## end GeoCountVolAction::handlePhysVol%3CE0CFC4014A.body
}

void GeoCountVolAction::handleFullPhysVol (const GeoFullPhysVol *)
{
  //## begin GeoCountVolAction::handleFullPhysVol%3CE0CFC4015E.body preserve=yes
  //   
  // Do not count the top volume, this action counts only children!   
  //   
  if (getPath ()->getLength () > 1)
    m_count++;
  //## end GeoCountVolAction::handleFullPhysVol%3CE0CFC4015E.body
}

void GeoCountVolAction::handleSerialTransformer (const GeoSerialTransformer  *st)
{
  //## begin GeoCountVolAction::handleSerialTransformer%3D208CA50301.body preserve=yes
  m_count += st->getNCopies ();
  //## end GeoCountVolAction::handleSerialTransformer%3D208CA50301.body
}

// Additional Declarations
  //## begin GeoCountVolAction%3CDF59FE02DC.declarations preserve=yes
  //## end GeoCountVolAction%3CDF59FE02DC.declarations

//## begin module%3CDF59FE02DC.epilog preserve=yes
//## end module%3CDF59FE02DC.epilog
