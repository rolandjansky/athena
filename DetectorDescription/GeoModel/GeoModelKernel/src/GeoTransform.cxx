/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDDBC1B00ED.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDDBC1B00ED.cm

//## begin module%3CDDBC1B00ED.cp preserve=no
//## end module%3CDDBC1B00ED.cp

//## Module: GeoTransform%3CDDBC1B00ED; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTransform.cxx

//## begin module%3CDDBC1B00ED.additionalIncludes preserve=no
//## end module%3CDDBC1B00ED.additionalIncludes

//## begin module%3CDDBC1B00ED.includes preserve=yes
//## end module%3CDDBC1B00ED.includes

// GeoTransform
#include "GeoModelKernel/GeoTransform.h"
//## begin module%3CDDBC1B00ED.additionalDeclarations preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
//## end module%3CDDBC1B00ED.additionalDeclarations


// Class GeoTransform 

GeoTransform::GeoTransform (const HepGeom::Transform3D &Transform)
  //## begin GeoTransform::GeoTransform%3CDE1F350127.hasinit preserve=no
  //## end GeoTransform::GeoTransform%3CDE1F350127.hasinit
  //## begin GeoTransform::GeoTransform%3CDE1F350127.initialization preserve=yes
:m_transform (Transform)
  //## end GeoTransform::GeoTransform%3CDE1F350127.initialization
{
  //## begin GeoTransform::GeoTransform%3CDE1F350127.body preserve=yes
  //## end GeoTransform::GeoTransform%3CDE1F350127.body
}


GeoTransform::~GeoTransform()
{
  //## begin GeoTransform::~GeoTransform%3CDDBC1B00ED_dest.body preserve=yes
  //## end GeoTransform::~GeoTransform%3CDDBC1B00ED_dest.body
}



//## Other Operations (implementation)
HepGeom::Transform3D GeoTransform::getTransform () const
{
  //## begin GeoTransform::getTransform%3CDE1C3B01CB.body preserve=yes
  return m_transform;
  //## end GeoTransform::getTransform%3CDE1C3B01CB.body
}

HepGeom::Transform3D GeoTransform::getDefTransform () const
{
  //## begin GeoTransform::getDefTransform%3CDE1D0E014D.body preserve=yes
  return m_transform;
  //## end GeoTransform::getDefTransform%3CDE1D0E014D.body
}

const HepGeom::Transform3D * GeoTransform::getDelta () const
{
  //## begin GeoTransform::getDelta%3CDE1D1102F6.body preserve=yes
  return NULL;
  //## end GeoTransform::getDelta%3CDE1D1102F6.body
}

void GeoTransform::exec (GeoNodeAction *action) const
{
  //## begin GeoTransform::exec%3CDF56E70216.body preserve=yes
  action->handleTransform (this);
  //## end GeoTransform::exec%3CDF56E70216.body
}

// Additional Declarations
  //## begin GeoTransform%3CDDBC1B00ED.declarations preserve=yes
  //## end GeoTransform%3CDDBC1B00ED.declarations

//## begin module%3CDDBC1B00ED.epilog preserve=yes
//## end module%3CDDBC1B00ED.epilog
