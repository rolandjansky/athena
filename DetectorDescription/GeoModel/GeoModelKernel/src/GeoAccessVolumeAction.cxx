/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoAccessVolumeAction.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include <string>

GeoAccessVolumeAction::GeoAccessVolumeAction(unsigned int Index, const GeoVAlignmentStore* store)
  : m_index(Index)
  , m_counter(0)
  , m_nameTag(nullptr)
  , m_serialDenominator(nullptr)
  , m_idTag(nullptr)
  , m_serialDenomPosition(0)
  , m_serialIdentifier(nullptr)
  , m_serialIdentPosition(0)
  , m_alignStore(store)
{
  setDepthLimit (1);
  m_pendingTransformList.reserve(1);
}

GeoAccessVolumeAction::~GeoAccessVolumeAction()
{
}

void GeoAccessVolumeAction::handleTransform (const GeoTransform *xform)
{
  m_pendingTransformList.push_back (xform);
}

void GeoAccessVolumeAction::handlePhysVol (const GeoPhysVol *vol)
{
  //      
  // Return if we have not got to any children:     
  //     
  if(getPath()->getLength()-1 == 0)
    return;
  //     
  // Otherwise check if the index equals the counter:     
  //     
  if(m_index == m_counter) {
    m_volume = vol;
    unsigned int listSize = m_pendingTransformList.size ();
    if(listSize == 0) {
      // do nothing.... 
    }
    else if(listSize == 1) {
      m_transform = m_pendingTransformList[0]->getTransform(m_alignStore);
      m_defTransform = m_pendingTransformList[0]->getDefTransform(m_alignStore);
    }
    else {
      for(unsigned int t = 0; t < m_pendingTransformList.size (); t++) {
	m_transform = m_transform * m_pendingTransformList[t]->getTransform(m_alignStore);
	m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(m_alignStore);
      }
    }
    terminate();
  }
  else {
    m_idTag   = nullptr;
    m_nameTag = nullptr;
    m_pendingTransformList.erase (m_pendingTransformList.begin (),
				  m_pendingTransformList.end ());
  }
  //     
  // Increment the counter:     
  //     
  m_counter++;
}

void GeoAccessVolumeAction::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  //      
  // Return if we have not got to any children:     
  //     
  if(getPath()->getLength()-1 == 0)
    return;
  //     
  // Otherwise check if the index equals the counter:     
  //     
  if(m_index == m_counter) {
    m_volume = vol;
    unsigned int listSize = m_pendingTransformList.size ();
    if(listSize == 0) {
      // do nothing.... 
    }
    else if(listSize == 1) {
      m_transform = m_pendingTransformList[0]->getTransform(m_alignStore);
      m_defTransform = m_pendingTransformList[0]->getDefTransform(m_alignStore);
    }
    else {
      for(unsigned int t = 0; t < m_pendingTransformList.size (); t++) {
	m_transform = m_transform * m_pendingTransformList[t]->getTransform(m_alignStore);
	m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(m_alignStore);
      }
    }
    terminate();
  }
  else {
    m_pendingTransformList.erase (m_pendingTransformList.begin (),
				  m_pendingTransformList.end ()); 
    m_nameTag = nullptr;
    m_idTag   = nullptr;
  }
  //     
  // Increment the counter:     
  //     
  m_counter++;
}

PVConstLink GeoAccessVolumeAction::getVolume () const
{
  return m_volume;
}

const HepGeom::Transform3D & GeoAccessVolumeAction::getTransform () const
{
  return m_transform;
}

const HepGeom::Transform3D & GeoAccessVolumeAction::getDefTransform () const
{
  return m_defTransform;
}

const std::string & GeoAccessVolumeAction::getName () const
{
  if(m_name.empty()) {
    if(m_nameTag) {
      m_name = m_nameTag->getName();
    }
    else if(m_serialDenominator) {
      m_name = m_serialDenominator->getBaseName() + std::to_string(m_index-m_serialDenomPosition);
    }
    else {
      m_name = "ANON";
    }
  }
  return m_name;
}

void GeoAccessVolumeAction::handleNameTag (const GeoNameTag *nameTag)
{
  m_nameTag = nameTag;
  m_serialDenominator = nullptr;
  m_serialDenomPosition = 0;
}

void GeoAccessVolumeAction::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  m_serialDenominator = sD;
  m_serialDenomPosition = m_counter;
}

void GeoAccessVolumeAction::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  //     
  // Otherwise check if the index equals the counter:     
  //     
  if(m_index < m_counter + sT->getNCopies()) {
    unsigned int copy = m_index - m_counter;
    m_volume = sT->getVolume();
    for(unsigned int t = 0; t < m_pendingTransformList.size (); t++) {
      m_transform = m_transform * m_pendingTransformList[t]->getTransform(m_alignStore);
      m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(m_alignStore);
    }
    m_transform    = m_transform    * sT->getTransform (copy);
    m_defTransform = m_defTransform * sT->getTransform (copy);
    m_counter += (copy + 1);
    terminate ();
  }
  else {
    m_idTag   = nullptr;
    m_nameTag = nullptr;
    m_pendingTransformList.erase(m_pendingTransformList.begin (),
				 m_pendingTransformList.end ());
    m_counter += sT->getNCopies();
  }
}

void GeoAccessVolumeAction::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  m_idTag = idTag;
  m_serialIdentifier = nullptr;
  m_serialIdentPosition = 0;
}

Query<int> GeoAccessVolumeAction::getId () const
{
  Query<int> id;
  if(m_idTag) {
    id = Query<int>(m_idTag->getIdentifier());
  }
  else if(m_serialIdentifier) {
    id = Query<int>(m_index - m_serialIdentPosition + m_serialIdentifier->getBaseId());
  }
  return id;
}

void GeoAccessVolumeAction::handleSerialIdentifier(const GeoSerialIdentifier *sI)
{
  m_serialIdentifier = sI;
  m_serialIdentPosition = m_counter;
}
