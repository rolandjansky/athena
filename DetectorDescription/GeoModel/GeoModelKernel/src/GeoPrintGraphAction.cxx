/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3FB018ED0310.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3FB018ED0310.cm

//## begin module%3FB018ED0310.cp preserve=no
//## end module%3FB018ED0310.cp

//## Module: GeoPrintGraphAction%3FB018ED0310; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPrintGraphAction.cxx

//## begin module%3FB018ED0310.additionalIncludes preserve=no
//## end module%3FB018ED0310.additionalIncludes

//## begin module%3FB018ED0310.includes preserve=yes
//## end module%3FB018ED0310.includes

// GeoPrintGraphAction
#include "GeoModelKernel/GeoPrintGraphAction.h"
//## begin module%3FB018ED0310.additionalDeclarations preserve=yes
//## end module%3FB018ED0310.additionalDeclarations


// Class GeoPrintGraphAction 

GeoPrintGraphAction::GeoPrintGraphAction (std::ostream &o)
  //## begin GeoPrintGraphAction::GeoPrintGraphAction%3FB027EA0056.hasinit preserve=no
      : m_nameTag(NULL),
        m_serialDenominator(NULL),
        m_idTag(NULL),
        m_transformState(true),
        m_volumeState(true),
        m_nametagState(true),
        m_serialDenominatorState(true),
        m_serialTransformerState(true),
        m_identifierState(true),
        m_o(o),
        m_indented(false)
  //## end GeoPrintGraphAction::GeoPrintGraphAction%3FB027EA0056.hasinit
  //## begin GeoPrintGraphAction::GeoPrintGraphAction%3FB027EA0056.initialization preserve=yes
  //## end GeoPrintGraphAction::GeoPrintGraphAction%3FB027EA0056.initialization
{
  //## begin GeoPrintGraphAction::GeoPrintGraphAction%3FB027EA0056.body preserve=yes
  //## end GeoPrintGraphAction::GeoPrintGraphAction%3FB027EA0056.body
}


GeoPrintGraphAction::~GeoPrintGraphAction()
{
  //## begin GeoPrintGraphAction::~GeoPrintGraphAction%3FB018ED0310_dest.body preserve=yes
  //## end GeoPrintGraphAction::~GeoPrintGraphAction%3FB018ED0310_dest.body
}



//## Other Operations (implementation)
void GeoPrintGraphAction::handleTransform (const GeoTransform *xform)
{
  //## begin GeoPrintGraphAction::handleTransform%3FB027E903A8.body preserve=yes
  m_pendingTransformList.push_back(xform);
  if (m_transformState) {
    indent();
    m_o << "TRANSFORM+";
  }
  //## end GeoPrintGraphAction::handleTransform%3FB027E903A8.body
}

void GeoPrintGraphAction::handlePhysVol (const GeoPhysVol *vol)
{
  //## begin GeoPrintGraphAction::handlePhysVol%3FB027E903DA.body preserve=yes
  if (m_volumeState) {
    indent();
    m_o << "VOLUME(" << vol->getLogVol()->getName() <<")" << std::endl;
  }
  m_pendingTransformList.erase(m_pendingTransformList.begin(),m_pendingTransformList.end());
  m_indented=false;
  //## end GeoPrintGraphAction::handlePhysVol%3FB027E903DA.body
}

void GeoPrintGraphAction::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  //## begin GeoPrintGraphAction::handleFullPhysVol%3FB027EA0024.body preserve=yes
  if (m_volumeState) {
    indent();
    m_o << "VOLUME(" << vol->getLogVol()->getName() <<")" << std::endl;
  }
  m_pendingTransformList.erase(m_pendingTransformList.begin(),m_pendingTransformList.end());
  m_indented=false;
  //## end GeoPrintGraphAction::handleFullPhysVol%3FB027EA0024.body
}

void GeoPrintGraphAction::handleNameTag (const GeoNameTag *nameTag)
{
  //## begin GeoPrintGraphAction::handleNameTag%3FB027EA0129.body preserve=yes
  if (m_nametagState) {
    indent();
    m_o << "NAMETAG: " << nameTag->getName() << "+";
  }
  //## end GeoPrintGraphAction::handleNameTag%3FB027EA0129.body
}

void GeoPrintGraphAction::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  //## begin GeoPrintGraphAction::handleSerialDenominator%3FB027EA0151.body preserve=yes
  if (m_nametagState) {
    indent();
    m_o << "SERIAL DENOMINATOR:: " << sD->getBaseName() << "+";
  }
  //## end GeoPrintGraphAction::handleSerialDenominator%3FB027EA0151.body
}

void GeoPrintGraphAction::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  //## begin GeoPrintGraphAction::handleSerialTransformer%3FB027EA016F.body preserve=yes
  if (m_volumeState) {
    indent();
    m_o << sT->getNCopies() << " PARAMETERIZED VOLUMES(" << sT->getVolume()->getLogVol()->getName() << ")" << std::endl;
  }
  m_pendingTransformList.erase(m_pendingTransformList.begin(),m_pendingTransformList.end());
  m_indented=false;
  //## end GeoPrintGraphAction::handleSerialTransformer%3FB027EA016F.body
}

void GeoPrintGraphAction::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  //## begin GeoPrintGraphAction::handleIdentifierTag%3FB027EA0197.body preserve=yes
  if (m_identifierState) {
    indent();
    m_o << "NAME: " << idTag->getIdentifier() << "+";
  }
  //## end GeoPrintGraphAction::handleIdentifierTag%3FB027EA0197.body
}

void GeoPrintGraphAction::setNotification (Type type, bool state)
{
  //## begin GeoPrintGraphAction::setNotification%3FB029F700BF.body preserve=yes
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
 
  //## end GeoPrintGraphAction::setNotification%3FB029F700BF.body
}

void GeoPrintGraphAction::indent ()
{
  //## begin GeoPrintGraphAction::indent%3FD385CE0289.body preserve=yes
  if (!m_indented) {
    m_indented=true;
    for (size_t i=0;i<getPath()->getLength(); i++) {
      m_o << "   ";
    }
  }

  //## end GeoPrintGraphAction::indent%3FD385CE0289.body
}

// Additional Declarations
  //## begin GeoPrintGraphAction%3FB018ED0310.declarations preserve=yes
  //## end GeoPrintGraphAction%3FB018ED0310.declarations

//## begin module%3FB018ED0310.epilog preserve=yes
//## end module%3FB018ED0310.epilog
