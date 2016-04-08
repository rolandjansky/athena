/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF52C70131.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF52C70131.cm

//## begin module%3CDF52C70131.cp preserve=no
//## end module%3CDF52C70131.cp

//## Module: GeoTraversalState%3CDF52C70131; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTraversalState.cxx

//## begin module%3CDF52C70131.additionalIncludes preserve=no
//## end module%3CDF52C70131.additionalIncludes

//## begin module%3CDF52C70131.includes preserve=yes
//## end module%3CDF52C70131.includes

// GeoTraversalState
#include "GeoModelKernel/GeoTraversalState.h"
//## begin module%3CDF52C70131.additionalDeclarations preserve=yes
//## end module%3CDF52C70131.additionalDeclarations


// Class GeoTraversalState 

GeoTraversalState::GeoTraversalState ()
  //## begin GeoTraversalState::GeoTraversalState%3CE2368201BA.hasinit preserve=no
  //## end GeoTraversalState::GeoTraversalState%3CE2368201BA.hasinit
  //## begin GeoTraversalState::GeoTraversalState%3CE2368201BA.initialization preserve=yes
  //## end GeoTraversalState::GeoTraversalState%3CE2368201BA.initialization
{
  //## begin GeoTraversalState::GeoTraversalState%3CE2368201BA.body preserve=yes
  //## end GeoTraversalState::GeoTraversalState%3CE2368201BA.body
}


GeoTraversalState::~GeoTraversalState()
{
  //## begin GeoTraversalState::~GeoTraversalState%3CDF52C70131_dest.body preserve=yes
  //## end GeoTraversalState::~GeoTraversalState%3CDF52C70131_dest.body
}



//## Other Operations (implementation)
const HepGeom::Transform3D & GeoTraversalState::getTransform () const
{
  //## begin GeoTraversalState::getTransform%3CE20A810113.body preserve=yes
  return m_transform;
  //## end GeoTraversalState::getTransform%3CE20A810113.body
}

const std::string & GeoTraversalState::getName () const
{
  //## begin GeoTraversalState::getName%3CEB1B1C0365.body preserve=yes
  return m_name;
  //## end GeoTraversalState::getName%3CEB1B1C0365.body
}

const HepGeom::Transform3D & GeoTraversalState::getDefTransform () const
{
  //## begin GeoTraversalState::getDefTransform%3CE20C070218.body preserve=yes
  return m_defTransform;
  //## end GeoTraversalState::getDefTransform%3CE20C070218.body
}

const std::string & GeoTraversalState::getAbsoluteName () const
{
  //## begin GeoTraversalState::getAbsoluteName%3CEB1B4E017C.body preserve=yes
  return m_absName;
  //## end GeoTraversalState::getAbsoluteName%3CEB1B4E017C.body
}

const HepGeom::Transform3D & GeoTraversalState::getDefAbsoluteTransform () const
{
  //## begin GeoTraversalState::getDefAbsoluteTransform%3CE20A9001B5.body preserve=yes
  return m_defAbsTransform;
  //## end GeoTraversalState::getDefAbsoluteTransform%3CE20A9001B5.body
}

const HepGeom::Transform3D & GeoTraversalState::getAbsoluteTransform () const
{
  //## begin GeoTraversalState::getAbsoluteTransform%3CE20C0701F0.body preserve=yes
  return m_absTransform;
  //## end GeoTraversalState::getAbsoluteTransform%3CE20C0701F0.body
}

void GeoTraversalState::setTransform (const HepGeom::Transform3D &transform)
{
  //## begin GeoTraversalState::setTransform%3CE21D9601BC.body preserve=yes
  m_transform = transform;
  m_absTransform = m_absTransformList.top () * transform;
  //## end GeoTraversalState::setTransform%3CE21D9601BC.body
}

void GeoTraversalState::setName (const std::string &name)
{
  //## begin GeoTraversalState::setName%3CEB1D6300B2.body preserve=yes
  m_name = name;
  m_absName = m_absNameList.top () + "/" + name;
  //## end GeoTraversalState::setName%3CEB1D6300B2.body
}

void GeoTraversalState::setDefTransform (const HepGeom::Transform3D &transform)
{
  //## begin GeoTraversalState::setDefTransform%3CE21DC501E2.body preserve=yes
  m_defTransform = transform;
  m_defAbsTransform = m_defAbsTransformList.top () * transform;
  //## end GeoTraversalState::setDefTransform%3CE21DC501E2.body
}

void GeoTraversalState::nextLevel (const GeoVPhysVol* pv)
{
  //## begin GeoTraversalState::nextLevel%3CE21DDB0392.body preserve=yes
  m_absNameList.push (m_absName);
  m_absTransformList.push (m_absTransform);
  m_defAbsTransformList.push (m_defAbsTransform);

  m_path.push(pv);
  //      
  // Reinitialize to identity.     
  //     

  m_absTransform = m_defAbsTransform = m_transform = m_defTransform =
    HepGeom::Transform3D ();


  //## end GeoTraversalState::nextLevel%3CE21DDB0392.body
}

void GeoTraversalState::previousLevel ()
{
  //## begin GeoTraversalState::previousLevel%3CE21F1C011D.body preserve=yes
  m_absTransform = m_absTransformList.top ();
  m_defAbsTransform = m_defAbsTransformList.top ();
  m_absTransformList.pop ();
  m_defAbsTransformList.pop ();
  m_absNameList.pop ();
  m_path.pop();
  m_transform = m_defTransform = HepGeom::Transform3D ();
  //## end GeoTraversalState::previousLevel%3CE21F1C011D.body
}

const GeoNodePath * GeoTraversalState::getPath () const
{
  //## begin GeoTraversalState::getPath%3CE22E2F0120.body preserve=yes
  return &m_path;
  //## end GeoTraversalState::getPath%3CE22E2F0120.body
}

void GeoTraversalState::setId (const Query<int> &id)
{
  //## begin GeoTraversalState::setId%3E2AD91A0290.body preserve=yes
	m_id=id;
  //## end GeoTraversalState::setId%3E2AD91A0290.body
}

const Query<int> GeoTraversalState::getId () const
{
  //## begin GeoTraversalState::getId%3E2AD9380289.body preserve=yes
	return m_id;
  //## end GeoTraversalState::getId%3E2AD9380289.body
}

// Additional Declarations
  //## begin GeoTraversalState%3CDF52C70131.declarations preserve=yes
  //## end GeoTraversalState%3CDF52C70131.declarations

//## begin module%3CDF52C70131.epilog preserve=yes
//## end module%3CDF52C70131.epilog
