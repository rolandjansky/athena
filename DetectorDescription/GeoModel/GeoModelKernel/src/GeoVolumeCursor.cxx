/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include <string>

GeoVolumeCursor::GeoVolumeCursor (PVConstLink parent, GeoVAlignmentStore* store)
  : m_parent(parent)
  , m_majorIndex(0)
  , m_minorIndex(0)
  , m_minorLimit(0)
  , m_serialTransformer(nullptr)
  , m_nameTag(nullptr)
  , m_serialDenominator(nullptr)
  , m_idTag(nullptr)
  , m_serialDenomPosition(0)
  , m_serialIdentifier(nullptr)
  , m_serialIdentPosition(0)
  , m_volCount(0)
  , m_hasAlignTrans(false)
  , m_alignStore(store)
{
  setDepthLimit(0);
  next();
}

GeoVolumeCursor::~GeoVolumeCursor()
{
}

void GeoVolumeCursor::next() {
  resuscitate();
  
  if (m_minorLimit) { // We are doing a serial transformer
    m_minorIndex++;
    if (m_minorIndex==m_minorLimit) {  
      m_minorIndex=0;
      m_minorLimit=0;
      m_majorIndex++;
      next();
    }
    else {
      m_volCount++;
    }              
  }
  else {           // We are not doing a serial transformer
    m_idTag             = nullptr;
    m_nameTag           = nullptr;
    m_serialTransformer = nullptr;
    m_pendingTransformList.erase (m_pendingTransformList.begin (),
				  m_pendingTransformList.end ());
    m_hasAlignTrans = false;
    m_volume=nullptr;
    
    int N = m_parent->getNChildNodes();
    if(N==0) return;
    const GeoGraphNode * const *node  = m_parent->getChildNode(m_majorIndex);
    const GeoGraphNode * const *back  = m_parent->getChildNode(N-1);
    const GeoGraphNode * const *end   = back+1;
    while (node!=end) {
      (*node)->exec(this);
      if (m_minorLimit) { // We have stepped into ST
      }
      else {            // We have not stepped into ST.
	node++;
	m_majorIndex++;
      }
      if (m_terminate) {
	m_volCount++;
	break;
      }
    }
  }
}

void GeoVolumeCursor::resuscitate() {
  m_terminate = false;
}

bool GeoVolumeCursor::atEnd() const {
  return !m_volume;
}

void GeoVolumeCursor::handleTransform (const GeoTransform *xform)
{
  m_pendingTransformList.push_back (xform);
  if(dynamic_cast<const GeoAlignableTransform*>(xform))
    m_hasAlignTrans = true;
}

void GeoVolumeCursor::handlePhysVol (const GeoPhysVol *vol)
{

  m_volume = vol;
  unsigned int listSize = m_pendingTransformList.size ();
  if (listSize == 0) {
    m_transform    = HepGeom::Transform3D();
    m_defTransform = HepGeom::Transform3D();
  }
  else {
    m_transform = m_pendingTransformList[0]->getTransform(m_alignStore);
    m_defTransform = m_pendingTransformList[0]->getDefTransform(m_alignStore);
    for (unsigned int t = 1; t < m_pendingTransformList.size (); t++) {
      m_transform    = m_transform    * m_pendingTransformList[t]->getTransform(m_alignStore);
      m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(m_alignStore);
    }
  }
  terminate ();
}

void GeoVolumeCursor::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  m_volume = vol;
  unsigned int listSize = m_pendingTransformList.size ();
  if (listSize == 0) {
    m_transform    = HepGeom::Transform3D();
    m_defTransform = HepGeom::Transform3D();
  }
  else {
    m_transform = m_pendingTransformList[0]->getTransform(m_alignStore);
    m_defTransform = m_pendingTransformList[0]->getDefTransform(m_alignStore);
    for (unsigned int t = 1; t < m_pendingTransformList.size (); t++) {
      m_transform    = m_transform    * m_pendingTransformList[t]->getTransform(m_alignStore);
      m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(m_alignStore);
    }
  }
  terminate ();
}

void GeoVolumeCursor::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  
  m_minorLimit = sT->getNCopies();
  m_minorIndex = 0;
  m_serialTransformer = sT;
  m_volume = sT->getVolume();
  
  unsigned int listSize = m_pendingTransformList.size ();
  if (listSize == 0) {
    m_transform    = HepGeom::Transform3D();
    m_defTransform = HepGeom::Transform3D();
  }
  else {
    m_transform = m_pendingTransformList[0]->getTransform(m_alignStore);
    m_defTransform = m_pendingTransformList[0]->getDefTransform(m_alignStore);
    for (unsigned int t = 1; t < m_pendingTransformList.size (); t++) {
      m_transform    = m_transform    * m_pendingTransformList[t]->getTransform(m_alignStore);
      m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(m_alignStore);
    }
  }
  terminate ();
}

void GeoVolumeCursor::handleNameTag (const GeoNameTag *nameTag)
{
  m_nameTag = nameTag;
  m_serialDenominator = nullptr;
  m_serialDenomPosition = 0;
}

void GeoVolumeCursor::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  m_serialDenominator = sD;
  m_serialDenomPosition = m_volCount;
}


void GeoVolumeCursor::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  m_idTag = idTag;
  m_serialIdentifier = nullptr;
  m_serialIdentPosition = m_volCount;
}

void GeoVolumeCursor::handleSerialIdentifier(const GeoSerialIdentifier *sI)
{
  m_serialIdentifier = sI;
  m_serialIdentPosition =m_volCount;
}

PVConstLink GeoVolumeCursor::getVolume () const
{
  return m_volume;
}

HepGeom::Transform3D GeoVolumeCursor::getTransform () const
{
  if (m_minorLimit) {
    return m_transform*m_serialTransformer->getTransform(m_minorIndex);
  }
  else {
    return m_transform;
  }
}

HepGeom::Transform3D GeoVolumeCursor::getDefTransform () const
{
  if (m_minorLimit) {
    return m_transform*m_serialTransformer->getTransform(m_minorIndex);
  }
  else {
    return m_defTransform;
  }
}

std::string  GeoVolumeCursor::getName () const
{
  std::string name;
  if(m_nameTag) {
    name = m_nameTag->getName();
  }
  else if(m_serialDenominator) {
    name = m_serialDenominator->getBaseName() + std::to_string((int)(m_volCount-m_serialDenomPosition-1));
  }
  else {
    name = "ANON";
  }
  return name;
}

Query<int> GeoVolumeCursor::getId () const
{
  Query<int> id;
  if (m_idTag) {
    id = Query<int> (m_idTag->getIdentifier ());
  }
  else if (m_serialIdentifier) {
    id = Query<int> (m_volCount - m_serialIdentPosition - 1 + m_serialIdentifier->getBaseId());
  }
  return id;
}

bool GeoVolumeCursor::hasAlignableTransform() const
{
  return m_hasAlignTrans;
}
