/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sichargeddiodecollection.cxx
//   Implementation file for class SiChargedDiodeCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 3.0 09/07/2001 David Calvet
// 04-03-03 Revisited version. Davide Costanzo
///////////////////////////////////////////////////////////////////
// header file
#include "SiDigitization/SiChargedDiodeCollection.h"
// member classes
#include "SiDigitization/SiHelper.h"
#include "ReadoutGeometryBase/DetectorDesign.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

using namespace InDetDD;



SiChargedDiodeCollection::SiChargedDiodeCollection( ) :
  m_chargedDiodes(),
  m_sielement()
{
}

SiChargedDiodeCollection::SiChargedDiodeCollection(const InDetDD::SolidStateDetectorElementBase* sielement ) :
  m_chargedDiodes(),
  m_sielement(sielement)
{
}


SiChargedDiodeCollection::~SiChargedDiodeCollection()
{
}

// Clean up the collection
void SiChargedDiodeCollection::clear() {
  m_sielement = nullptr;
  m_chargedDiodes.erase(m_chargedDiodes.begin(), m_chargedDiodes.end() );
  m_orderedChargedDiodes.clear();
}



bool SiChargedDiodeCollection::AlreadyHit(const InDetDD::SiCellId & siId) {
  if(m_chargedDiodes.find(siId) == m_chargedDiodes.end() ) {
    return false;
  } else {
    return true;
  }
}

bool SiChargedDiodeCollection::AlreadyHit(const Identifier & id) {

  const SiCellId cellId        = m_sielement->cellIdFromIdentifier(id);
  return AlreadyHit(cellId);
}

SiChargedDiode * SiChargedDiodeCollection::find(const InDetDD::SiCellId & siId) {
  // get the compact Id to access the map
  SiChargedDiodeIterator theEl = m_chargedDiodes.find(siId);
  // if the diode exists return a pointer to it:
  if (theEl == m_chargedDiodes.end() ) return nullptr;
  else return &( (*theEl).second);
}		      

SiChargedDiode * SiChargedDiodeCollection::find(Identifier siId) {

  // Get the key for the map lookup
  const SiCellId cellId        = m_sielement->cellIdFromIdentifier(siId);
  return find(cellId);
}		      

void SiChargedDiodeCollection::order()
{
  for (auto& p : m_chargedDiodes) {
    m_orderedChargedDiodes.insert (&p.second);
  }
}
