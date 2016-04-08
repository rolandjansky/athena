/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDDAEF8001F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDDAEF8001F.cm

//## begin module%3CDDAEF8001F.cp preserve=no
//## end module%3CDDAEF8001F.cp

//## Module: GeoVDetectorElement%3CDDAEF8001F; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVDetectorElement.cxx

//## begin module%3CDDAEF8001F.additionalIncludes preserve=no
//## end module%3CDDAEF8001F.additionalIncludes

//## begin module%3CDDAEF8001F.includes preserve=yes
//## end module%3CDDAEF8001F.includes

// GeoVDetectorElement
#include "GeoModelKernel/GeoVDetectorElement.h"
//## begin module%3CDDAEF8001F.additionalDeclarations preserve=yes
//## end module%3CDDAEF8001F.additionalDeclarations


// Class GeoVDetectorElement 

GeoVDetectorElement::GeoVDetectorElement (const GeoVFullPhysVol* fullPhysVol)
  //## begin GeoVDetectorElement::GeoVDetectorElement%3CDDB11202FE.hasinit preserve=no
  //## end GeoVDetectorElement::GeoVDetectorElement%3CDDB11202FE.hasinit
  //## begin GeoVDetectorElement::GeoVDetectorElement%3CDDB11202FE.initialization preserve=yes
  :m_materialGeom(fullPhysVol)
  //## end GeoVDetectorElement::GeoVDetectorElement%3CDDB11202FE.initialization
{
  //## begin GeoVDetectorElement::GeoVDetectorElement%3CDDB11202FE.body preserve=yes
  //## end GeoVDetectorElement::GeoVDetectorElement%3CDDB11202FE.body
}


GeoVDetectorElement::~GeoVDetectorElement()
{
  //## begin GeoVDetectorElement::~GeoVDetectorElement%3CDDAEF8001F_dest.body preserve=yes
  //## end GeoVDetectorElement::~GeoVDetectorElement%3CDDAEF8001F_dest.body
}


// Additional Declarations
  //## begin GeoVDetectorElement%3CDDAEF8001F.declarations preserve=yes
  //## end GeoVDetectorElement%3CDDAEF8001F.declarations

//## begin module%3CDDAEF8001F.epilog preserve=yes
//## end module%3CDDAEF8001F.epilog
