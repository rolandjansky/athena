/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDDBC2403CB.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDDBC2403CB.cm

//## begin module%3CDDBC2403CB.cp preserve=no
//## end module%3CDDBC2403CB.cp

//## Module: GeoAlignableTransform%3CDDBC2403CB; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoAlignableTransform.cxx

//## begin module%3CDDBC2403CB.additionalIncludes preserve=no
//## end module%3CDDBC2403CB.additionalIncludes

//## begin module%3CDDBC2403CB.includes preserve=yes
#include "GeoModelKernel/GeoClearAbsPosAction.h"
//## end module%3CDDBC2403CB.includes

// GeoAlignableTransform
#include "GeoModelKernel/GeoAlignableTransform.h"
//## begin module%3CDDBC2403CB.additionalDeclarations preserve=yes
//## end module%3CDDBC2403CB.additionalDeclarations

#include "GeoSelClearAbsPosAction.h"
#include <set>

// Class GeoAlignableTransform 

GeoAlignableTransform::GeoAlignableTransform (const HepGeom::Transform3D &Transform)
  //## begin GeoAlignableTransform::GeoAlignableTransform%3CDE1FCE0285.hasinit preserve=no
  //## end GeoAlignableTransform::GeoAlignableTransform%3CDE1FCE0285.hasinit
  //## begin GeoAlignableTransform::GeoAlignableTransform%3CDE1FCE0285.initialization preserve=yes
  :
GeoTransform (Transform),
m_delta (NULL)
  //## end GeoAlignableTransform::GeoAlignableTransform%3CDE1FCE0285.initialization
{
  //## begin GeoAlignableTransform::GeoAlignableTransform%3CDE1FCE0285.body preserve=yes
  //## end GeoAlignableTransform::GeoAlignableTransform%3CDE1FCE0285.body
}


GeoAlignableTransform::~GeoAlignableTransform()
{
  //## begin GeoAlignableTransform::~GeoAlignableTransform%3CDDBC2403CB_dest.body preserve=yes
  delete m_delta;
  //## end GeoAlignableTransform::~GeoAlignableTransform%3CDDBC2403CB_dest.body
}



//## Other Operations (implementation)
HepGeom::Transform3D GeoAlignableTransform::getTransform () const
{
  //## begin GeoAlignableTransform::getTransform%3CDE250E00CF.body preserve=yes
  if (m_delta)
    {
      return GeoTransform::getTransform () * (*m_delta);
    }
  else
    {
      return GeoTransform::getTransform ();
    }
  //## end GeoAlignableTransform::getTransform%3CDE250E00CF.body
}

const HepGeom::Transform3D * GeoAlignableTransform::getDelta () const
{
  //## begin GeoAlignableTransform::getDelta%3CDE1FCE0253.body preserve=yes
  return m_delta;
  //## end GeoAlignableTransform::getDelta%3CDE1FCE0253.body
}

void GeoAlignableTransform::setDelta (const HepGeom::Transform3D &Delta)
{
  //## begin GeoAlignableTransform::setDelta%3CDE23E70157.body preserve=yes
  if(m_delta && (*m_delta == Delta)) return;

  std::set<GeoGraphNode*> uniqueParents;

  if (!m_delta)
    {
      m_delta = new HepGeom::Transform3D (Delta);
    }
  else
    {
      (*m_delta) = Delta;
    }
  for (size_t p = 0; p < m_parentList.size (); p++)
    {
      GeoGraphNode* parent = m_parentList[p];
      if(uniqueParents.find(parent)==uniqueParents.end()) {
	GeoSelClearAbsPosAction cc(this);
	parent->exec (&cc);
	uniqueParents.insert(parent);
      }
    }
  //## end GeoAlignableTransform::setDelta%3CDE23E70157.body
}

void GeoAlignableTransform::clearDelta ()
{
  //## begin GeoAlignableTransform::clearDelta%3CDE23ED014C.body preserve=yes
  std::set<GeoGraphNode*> uniqueParents;

  delete m_delta;
  m_delta = NULL;
  for (size_t p = 0; p < m_parentList.size (); p++)
    {
      GeoGraphNode* parent = m_parentList[p];
      if(uniqueParents.find(parent)==uniqueParents.end()) {
	GeoSelClearAbsPosAction cc(this);
	parent->exec (&cc);
	uniqueParents.insert(parent);
      }
    }
  //## end GeoAlignableTransform::clearDelta%3CDE23ED014C.body
}

void GeoAlignableTransform::dockTo (GeoVPhysVol* parent)
{
  //## begin GeoAlignableTransform::dockTo%3CE3DA4700CE.body preserve=yes
  m_parentList.push_back (parent);
  //## end GeoAlignableTransform::dockTo%3CE3DA4700CE.body
}

// Additional Declarations
  //## begin GeoAlignableTransform%3CDDBC2403CB.declarations preserve=yes
  //## end GeoAlignableTransform%3CDDBC2403CB.declarations

//## begin module%3CDDBC2403CB.epilog preserve=yes
//## end module%3CDDBC2403CB.epilog
