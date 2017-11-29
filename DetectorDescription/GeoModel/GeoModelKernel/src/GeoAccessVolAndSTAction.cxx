/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoAccessVolAndSTAction.h"

#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include <string>

GeoAccessVolAndSTAction::GeoAccessVolAndSTAction(unsigned int index)
  : m_volume(0)
  , m_serialTransformer(0)
  , m_index(index)
  , m_counter(0)
  , m_nameTag(0)
  , m_serialDenominator(0)
  , m_idTag(0)
  , m_serialDenomPosition(0)
{
  setDepthLimit(1);
}

GeoAccessVolAndSTAction::~GeoAccessVolAndSTAction()
{
}

void GeoAccessVolAndSTAction::handleTransform(const GeoTransform *xform)
{
  m_pendingTransformList.push_back (xform);
}

void GeoAccessVolAndSTAction::handlePhysVol(const GeoPhysVol *vol)
{
  // Return if we have not got any children     
  if(getPath()->getLength()-1 == 0)
    return;

  // Otherwise check if the index equals the counter:     
  if(m_index == m_counter) {
    m_volume = vol;
    m_serialTransformer = 0;
    unsigned int listSize = m_pendingTransformList.size();

    if(listSize == 0) {
      // do nothing.... 
    }
    else if(listSize == 1) {
      m_transform = m_pendingTransformList[0]->getTransform(nullptr);
      m_defTransform = m_pendingTransformList[0]->getDefTransform(nullptr);
    }
    else {
      for(unsigned int t=0; t<m_pendingTransformList.size(); t++) {
	m_transform = m_transform * m_pendingTransformList[t]->getTransform(nullptr);
	m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(nullptr);
      }
    }
    terminate ();
  }
  else {
    m_idTag   = 0;
    m_nameTag = 0;
    m_pendingTransformList.erase(m_pendingTransformList.begin(),
				 m_pendingTransformList.end());
  }

  m_counter++;
}

void GeoAccessVolAndSTAction::handleFullPhysVol(const GeoFullPhysVol *vol)
{
  // Return if we have not got any children     
  if(getPath()->getLength()-1 == 0)
    return;

  // Otherwise check if the index equals the counter:     
  if(m_index == m_counter)
  {
    m_volume = vol;
    m_serialTransformer = 0;

    unsigned int listSize = m_pendingTransformList.size();
    if(listSize == 0) {
      // do nothing.... 
    }
    else if(listSize == 1) {
      m_transform = m_pendingTransformList[0]->getTransform(nullptr);
      m_defTransform = m_pendingTransformList[0]->getDefTransform(nullptr);
    }
    else {
      for(unsigned int t = 0; t < m_pendingTransformList.size (); t++) {
	m_transform = m_transform * m_pendingTransformList[t]->getTransform(nullptr);
	m_defTransform = m_defTransform * m_pendingTransformList[t]->getDefTransform(nullptr);
      }
    }
    terminate();
  }
  else {
    m_pendingTransformList.erase (m_pendingTransformList.begin(),
				  m_pendingTransformList.end()); 
    m_nameTag = 0;
    m_idTag   = 0;
  }
  
  m_counter++;
}

PVConstLink GeoAccessVolAndSTAction::getVolume() const
{
  return m_volume;
}

const HepGeom::Transform3D & GeoAccessVolAndSTAction::getTransform() const
{
  return m_transform;
}

const HepGeom::Transform3D & GeoAccessVolAndSTAction::getDefTransform() const
{
  return m_defTransform;
}

const std::string & GeoAccessVolAndSTAction::getName() const
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

void GeoAccessVolAndSTAction::handleNameTag(const GeoNameTag *nameTag)
{
  m_nameTag = nameTag;
  m_serialDenominator = 0;
  m_serialDenomPosition = 0;
}

void GeoAccessVolAndSTAction::handleSerialDenominator(const GeoSerialDenominator *sD)
{
  m_serialDenominator = sD;
  m_serialDenomPosition = m_counter;
}

void GeoAccessVolAndSTAction::handleSerialTransformer(const GeoSerialTransformer  *sT)
{
  m_pendingTransformList.erase(m_pendingTransformList.begin(),
			      m_pendingTransformList.end());

  if(m_index == m_counter) {
    m_volume = 0;
    m_serialTransformer = sT;
    terminate();
  }
  else {
    m_idTag   = 0;
    m_nameTag = 0;
    m_counter++;
  }
}

const GeoSerialTransformer* GeoAccessVolAndSTAction::getSerialTransformer() const
{
  return m_serialTransformer;
}

void GeoAccessVolAndSTAction::handleIdentifierTag(const GeoIdentifierTag *idTag)
{
  m_idTag = idTag;
}

Query<unsigned int> GeoAccessVolAndSTAction::getId() const
{
  if(m_idTag) {
    return Query<unsigned int>(m_idTag->getIdentifier());
  }
  else {
    return Query<unsigned int>();
  }
}




