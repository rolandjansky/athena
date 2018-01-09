/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiChargedDiodeCollection.h
//   Header file for class SiChargedDiodeCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class used to store the list of SiChargedDiode objects for one
//  module (Pixel) or half module (SCT) and one event. 
///////////////////////////////////////////////////////////////////
// Version 3.0 09/07/2001 David Calvet
// Davide Costanzo. Revisited version. 04-03-03
//    - added a Identifier wafer_id to the private members
//    - added a IdHelper to the private members
//    - constructor modified to initialiaze wafer_id and IdHelper
//    - replaced <list> with <map> and use the compact id of the 
//      SiChargedDiode to map them.
//    - Inherit from Identifiable to enforce the identify() method
///////////////////////////////////////////////////////////////////
#ifndef SIDIGITIZATION_SICHARGEDDIODECOLLECTION_H
#define SIDIGITIZATION_SICHARGEDDIODECOLLECTION_H

// Base class
#include "Identifier/Identifiable.h"

// Data member classes
#include <map>
#include <unordered_map>
#include "SiDigitization/SiChargedDiode.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// Input/output classes 
#include "InDetSimEvent/SiHit.h"
class AtlasDetectorID;
namespace InDetDD{
  class SiDetectorElement;
  class SiDetectorDesign;
  class SiCellId;
}

#include "AthAllocators/ArenaPoolSTLAllocator.h"

struct SiChargedDiodeHash
{
  size_t operator() (const InDetDD::SiCellId & id) const
  {
    return id.word();
  }
};

typedef std::unordered_map<InDetDD::SiCellId,
                           SiChargedDiode,
                           SiChargedDiodeHash,
                           std::equal_to<InDetDD::SiCellId>,
                           SG::ArenaPoolSTLAllocator<
                            std::pair<const InDetDD::SiCellId, SiChargedDiode> > >
  SiChargedDiodeMap;

// Iterator typedef to make it easier
typedef SiChargedDiodeMap::iterator SiChargedDiodeIterator;

class SiChargedDiodeCollection : Identifiable {
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:


  // Constructor with parameters:
  //  ref. to the detector element for this collection
  SiChargedDiodeCollection( );

  SiChargedDiodeCollection(const InDetDD::SiDetectorElement* );


  // Destructor:
  ~SiChargedDiodeCollection();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // detector element:
  const InDetDD::SiDetectorElement * element() const;

  // wafer identifier for this collection
  virtual Identifier identify() const override final;
  virtual IdentifierHash identifyHash() const override final;

  // id helper for this collection
  const AtlasDetectorID* id_helper();
  
  // detector design:
  const InDetDD::SiDetectorDesign &design() const;

  // translation from SiReadoutCellId to Identifier
  Identifier getId(const InDetDD::SiCellId& id) const 
  { 
    return (element()->identifierFromCellId(id)); 
  }

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // clean up:
  void clear();

  // read/write access to the collection:
  SiChargedDiodeMap &chargedDiodes();

  // Set the SiDetectorElement
  void setDetectorElement(const InDetDD::SiDetectorElement *SiElement);

  // Add a new SiCharge to the collection
  // (add or merge in an existing SiChargedDiode):
  //   Diode which contains the new charge
  //   SiCharge to be added. 
  void add(const InDetDD::SiCellId & diode, const SiCharge &charge);
  void add(const InDetDD::SiCellId & diode, const SiTotalCharge &totcharge);

  bool AlreadyHit(const InDetDD::SiCellId & siId);
  bool AlreadyHit(const Identifier & id);
  SiChargedDiodeIterator begin();
  SiChargedDiodeIterator end();
  bool empty() const; // Test if there is anything in the collection.

  // return a Charged diode given its CellId, NULL if doesn't exist
  SiChargedDiode * find(const InDetDD::SiCellId & siId);
  // return a Charged diode given its identifier, NULL if doesn't exist
  SiChargedDiode * find(Identifier);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
 private:
  SiChargedDiodeCollection(const SiChargedDiodeCollection&);
  SiChargedDiodeCollection &operator=(const SiChargedDiodeCollection&);
  
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  //NB m_allocator should always be declared before m_chargedDiodes in
  //the intialization list.  If the allocator is declared after
  //m_chargedDiodes, when the collection is destroyed, the allocator
  //will be destroyed (and the memory it manages freed) before the
  //SiChargedDiodeMap.  This will cause a crash unless the
  //SiChargedDiodeMap is empty.
  SiTotalCharge::alloc_t m_allocator; 
  SiChargedDiodeMap m_chargedDiodes; // list of SiChargedDiodes 
  mutable const InDetDD::SiDetectorElement *m_sielement; // detector element
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

// Set the DetectorElement
inline void SiChargedDiodeCollection::setDetectorElement(const InDetDD::SiDetectorElement *SiElement) 
{
  m_sielement=SiElement;
}

inline SiChargedDiodeMap &SiChargedDiodeCollection::chargedDiodes()
{
  return m_chargedDiodes;
}

// access to the element
inline const InDetDD::SiDetectorElement *SiChargedDiodeCollection::element() const
{
  return m_sielement;
}

// access to the design
inline const InDetDD::SiDetectorDesign &SiChargedDiodeCollection::design() const
{
  return m_sielement->design();
}

// Get the Identifier of the collection
inline Identifier SiChargedDiodeCollection::identify() const
{
  return m_sielement->identify();
}

// Get the Identifier of the collection
inline IdentifierHash SiChargedDiodeCollection::identifyHash() const
{
  return m_sielement->identifyHash();
}

// Get the Id Helper
inline const AtlasDetectorID* SiChargedDiodeCollection::id_helper()
{   
  return m_sielement->getIdHelper();
}


inline SiChargedDiodeIterator SiChargedDiodeCollection::begin() 
{
  return m_chargedDiodes.begin();
}

inline SiChargedDiodeIterator SiChargedDiodeCollection::end() 
{
  return m_chargedDiodes.end();
}

inline bool SiChargedDiodeCollection::empty() const {
  return m_chargedDiodes.empty();
}



#endif // SIDIGITIZATION_SICHARGEDDIODECOLLECTION_H

