/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

using namespace InDetDD;



SiChargedDiodeCollection::SiChargedDiodeCollection( ) :
  m_chargedDiodes(),
  m_sielement()
{
}

SiChargedDiodeCollection::SiChargedDiodeCollection(const InDetDD::SiDetectorElement* sielement ) :
  m_chargedDiodes(),
  m_sielement(sielement)
{
}


SiChargedDiodeCollection::~SiChargedDiodeCollection()
{
}

// Clean up the collection
void SiChargedDiodeCollection::clear() {
  m_sielement = 0;
  m_chargedDiodes.erase(m_chargedDiodes.begin(), m_chargedDiodes.end() );
}

// Add a new SiCharge to the collection 
void SiChargedDiodeCollection::add(const SiCellId & diode,
				   const SiCharge & charge)
{
  // check the pointer is correct
  if (!diode.isValid()) return;

  // find this diode in the charged diode collection
  // find it by id in the map. 
  //

  SiChargedDiodeIterator the_diode = m_chargedDiodes.find(diode);

  if(the_diode != m_chargedDiodes.end() ) {
    // Add to existing charge
    (*the_diode).second.add(charge);
  } else {
    // if the new diode has not been found in the collection
    // get the read out cell from the design. 
    //
    SiReadoutCellId roCell=design().readoutIdOfCell(diode);
    if (!roCell.isValid()) { // I don't think this can occur at this stage but cant hurt.
      MsgStream log(Athena::getMessageSvc(),"SiChargedDiodeCollection");
      log << MSG::FATAL << "Could not create SiReadoutCellId object !"<< endmsg;
    }
    // create a new charged diode
    SiChargedDiode chargedDiode(m_allocator, diode,roCell);
    // add the new charge to it
    chargedDiode.add(charge);
    if (charge.processType() == SiCharge::extraNoise) SiHelper::noise(chargedDiode,true);
    // add the new charged diode to the charged diode collection
    m_chargedDiodes.insert(std::map<SiCellId,SiChargedDiode>::value_type(diode,chargedDiode));
  }
}

// Add a new SiTotalCharge to the collection 
void SiChargedDiodeCollection::add(const SiCellId & diode,
				   const SiTotalCharge & totcharge)
{
  // check the pointer is correct
  if (!diode.isValid()) return;

  // find this diode in the charged diode collection
  // find it by id in the map. 
  //

  SiChargedDiodeIterator the_diode = m_chargedDiodes.find(diode);

  if(the_diode != m_chargedDiodes.end() ) {
    // Add to existing charge
    (*the_diode).second.add(totcharge);
  } else {
    // if the new diode has not been found in the collection
    // get the read out cell from the design. 
    //
    SiReadoutCellId roCell=design().readoutIdOfCell(diode);
    if (!roCell.isValid()) {  // I don't think this can occur at this stage but cant hurt.
      MsgStream log(Athena::getMessageSvc(),"SiChargedDiodeCollection");
      log << MSG::FATAL << "Could not create SiReadoutCellId object !"<< endmsg;
    }
    // create a new charged diode
    SiChargedDiode chargedDiode(m_allocator, diode,roCell);
    // add the new charge to it
    chargedDiode.add(totcharge);
    // add the new charged diode to the charged diode collection
    m_chargedDiodes.insert(std::map<SiCellId,SiChargedDiode>::value_type(diode,chargedDiode));
  }
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
  if (theEl == m_chargedDiodes.end() ) return NULL;
  else return &( (*theEl).second);
}		      

SiChargedDiode * SiChargedDiodeCollection::find(Identifier siId) {

  // Get the key for the map lookup
  const SiCellId cellId        = m_sielement->cellIdFromIdentifier(siId);
  return find(cellId);
}		      
