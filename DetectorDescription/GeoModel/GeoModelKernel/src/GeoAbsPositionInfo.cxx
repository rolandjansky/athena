/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD8B7801FE.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD8B7801FE.cm

//## begin module%3CDD8B7801FE.cp preserve=no
//## end module%3CDD8B7801FE.cp

//## Module: GeoAbsPositionInfo%3CDD8B7801FE; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoAbsPositionInfo.cxx

//## begin module%3CDD8B7801FE.additionalIncludes preserve=no
//## end module%3CDD8B7801FE.additionalIncludes

//## begin module%3CDD8B7801FE.includes preserve=yes
//## end module%3CDD8B7801FE.includes

// GeoAbsPositionInfo
#include "GeoModelKernel/GeoAbsPositionInfo.h"
//## begin module%3CDD8B7801FE.additionalDeclarations preserve=yes
#include <memory>
#include "CLHEP/Geometry/Transform3D.h"
//## end module%3CDD8B7801FE.additionalDeclarations


// Class GeoAbsPositionInfo 

GeoAbsPositionInfo::GeoAbsPositionInfo()
  //## begin GeoAbsPositionInfo::GeoAbsPositionInfo%3CDD8B7801FE_const.hasinit preserve=no
      : m_absTransform(NULL),
        m_defAbsTransform(NULL)
  //## end GeoAbsPositionInfo::GeoAbsPositionInfo%3CDD8B7801FE_const.hasinit
  //## begin GeoAbsPositionInfo::GeoAbsPositionInfo%3CDD8B7801FE_const.initialization preserve=yes
  //## end GeoAbsPositionInfo::GeoAbsPositionInfo%3CDD8B7801FE_const.initialization
{
  //## begin GeoAbsPositionInfo::GeoAbsPositionInfo%3CDD8B7801FE_const.body preserve=yes
  //## end GeoAbsPositionInfo::GeoAbsPositionInfo%3CDD8B7801FE_const.body
}


GeoAbsPositionInfo::~GeoAbsPositionInfo()
{
  //## begin GeoAbsPositionInfo::~GeoAbsPositionInfo%3CDD8B7801FE_dest.body preserve=yes
  delete m_absTransform;
  delete m_defAbsTransform;
  //## end GeoAbsPositionInfo::~GeoAbsPositionInfo%3CDD8B7801FE_dest.body
}



//## Other Operations (implementation)
void GeoAbsPositionInfo::clearAbsTransform ()
{
  //## begin GeoAbsPositionInfo::clearAbsTransform%3CDE5FCD02C8.body preserve=yes
  delete m_absTransform;
  m_absTransform = NULL;
  //## end GeoAbsPositionInfo::clearAbsTransform%3CDE5FCD02C8.body
}

void GeoAbsPositionInfo::clearDefAbsTransform ()
{
  //## begin GeoAbsPositionInfo::clearDefAbsTransform%3CDE5FD8006B.body preserve=yes
  delete m_defAbsTransform;
  m_defAbsTransform = NULL;
  //## end GeoAbsPositionInfo::clearDefAbsTransform%3CDE5FD8006B.body
}

void GeoAbsPositionInfo::setAbsTransform (const HepGeom::Transform3D &  xform)
{
  //## begin GeoAbsPositionInfo::setAbsTransform%3CDE60550300.body preserve=yes
  if (m_absTransform)
    {
      (*m_absTransform) = xform;
    }
  else
    {
      m_absTransform = new HepGeom::Transform3D (xform);
    }
  //## end GeoAbsPositionInfo::setAbsTransform%3CDE60550300.body
}

void GeoAbsPositionInfo::setDefAbsTransform (const HepGeom::Transform3D &  xform)
{
  //## begin GeoAbsPositionInfo::setDefAbsTransform%3CDE60B902F0.body preserve=yes
  if (m_defAbsTransform)
    {
      (*m_defAbsTransform) = xform;
    }
  else
    {
      m_defAbsTransform = new HepGeom::Transform3D (xform);
    }
  //## end GeoAbsPositionInfo::setDefAbsTransform%3CDE60B902F0.body
}

// Additional Declarations
  //## begin GeoAbsPositionInfo%3CDD8B7801FE.declarations preserve=yes
  //## end GeoAbsPositionInfo%3CDD8B7801FE.declarations

//## begin module%3CDD8B7801FE.epilog preserve=yes
//## end module%3CDD8B7801FE.epilog
