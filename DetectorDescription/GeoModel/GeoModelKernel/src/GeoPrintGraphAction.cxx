/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPrintGraphAction.h"

GeoPrintGraphAction::GeoPrintGraphAction (std::ostream &o)
  : m_nameTag(nullptr)
  , m_serialDenominator(nullptr)
  , m_idTag(nullptr)
  , m_transformState(true)
  , m_volumeState(true)
  , m_nametagState(true)
  , m_serialDenominatorState(true)
  , m_serialTransformerState(true)
  , m_identifierState(true)
  , m_o(o)
  , m_indented(false)
{
}

GeoPrintGraphAction::~GeoPrintGraphAction()
{
}

void GeoPrintGraphAction::handleTransform (const GeoTransform *xform)
{
  m_pendingTransformList.push_back(xform);
  if (m_transformState) {
    indent();
    m_o << "TRANSFORM+";
  }
}

void GeoPrintGraphAction::handlePhysVol (const GeoPhysVol *vol)
{
  if (m_volumeState) {
    indent();
    m_o << "VOLUME(" << vol->getLogVol()->getName() <<")" << std::endl;
  }
  m_pendingTransformList.erase(m_pendingTransformList.begin(),m_pendingTransformList.end());
  m_indented=false;
}

void GeoPrintGraphAction::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  if (m_volumeState) {
    indent();
    m_o << "VOLUME(" << vol->getLogVol()->getName() <<")" << std::endl;
  }
  m_pendingTransformList.erase(m_pendingTransformList.begin(),m_pendingTransformList.end());
  m_indented=false;
}

void GeoPrintGraphAction::handleNameTag (const GeoNameTag *nameTag)
{
  if (m_nametagState) {
    indent();
    m_o << "NAMETAG: " << nameTag->getName() << "+";
  }
}

void GeoPrintGraphAction::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  if (m_nametagState) {
    indent();
    m_o << "SERIAL DENOMINATOR:: " << sD->getBaseName() << "+";
  }
}

void GeoPrintGraphAction::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  if (m_volumeState) {
    indent();
    m_o << sT->getNCopies() << " PARAMETERIZED VOLUMES(" << sT->getVolume()->getLogVol()->getName() << ")" << std::endl;
  }
  m_pendingTransformList.erase(m_pendingTransformList.begin(),m_pendingTransformList.end());
  m_indented=false;
}

void GeoPrintGraphAction::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  if (m_identifierState) {
    indent();
    m_o << "NAME: " << idTag->getIdentifier() << "+";
  }
}

void GeoPrintGraphAction::setNotification (Type type, bool state)
{
  if (type==TRANSFORM) {
    m_transformState=state;
  }
  else if (type==VOLUME) {
    m_volumeState=state; 
  }
  else if (type==NAMETAG) {
    m_nametagState=state;
  }
  else if (type==IDENTIFIERTAG) {
    m_identifierState=state;
  }
}

void GeoPrintGraphAction::indent ()
{
  if (!m_indented) {
    m_indented=true;
    for (size_t i=0;i<getPath()->getLength(); i++) {
      m_o << "   ";
    }
  }
}


