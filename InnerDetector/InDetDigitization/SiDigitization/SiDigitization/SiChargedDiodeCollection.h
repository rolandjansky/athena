/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
//    - constructor modified to initialize wafer_id and IdHelper
//    - replaced <list> with <map> and use the compact id of the 
//      SiChargedDiode to map them.
//    - Inherit from Identifiable to enforce the identify() method
///////////////////////////////////////////////////////////////////
#ifndef SIDIGITIZATION_SICHARGEDDIODECOLLECTION_H
#define SIDIGITIZATION_SICHARGEDDIODECOLLECTION_H

// Base class
#include "Identifier/Identifiable.h"

// Data member classes
#include <unordered_map>
#include "SiDigitization/SiChargedDiode.h"
#include "Identifier/Identifier.h"
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.h"

// Input/output classes 
#include "InDetSimEvent/SiHit.h"

// STL includes
#include <atomic>
#include <set>
#include <memory>

class AtlasDetectorID;
namespace InDetDD{
  class DetectorDesign;
  class SiCellId;
}

#include "AthAllocators/ArenaPoolSTLAllocator.h"
#include "SiDigitization/SiHelper.h"

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



//
// A normal iteration over a SiChargedDiodeCollection will use
// the unordered_map, so the ordering is not defined.  The observed
// ordering can (and does) change depending on the compiler and library
// version used.  In some cases, though, we are sensitive to the
// order of iteration, for example in cases where we generate a random
// number for each element of the collection.  To get results which
// are identical across compilers, we need to instead do the iteration
// in a well-defined order.
//
// This can be done using the methods sortedBegin and sortedEnd instead
// of begin and end.  These work by maintaining a std::set of pointers
// to the diodes, sorted by the diode number.  In order to avoid paying
// the penalty for maintaining the sorted set when we don't need to, we only
// start maintaining it the first time that it's requested.
struct SiChargedDiodeOrderedSetCompare
{
  size_t operator() (const SiChargedDiode* a,
                     const SiChargedDiode* b) const
  {
    return a->diode().word() < b->diode().word();
  }
};



typedef std::set<SiChargedDiode*,
                 SiChargedDiodeOrderedSetCompare,
                 SG::ArenaPoolSTLAllocator<SiChargedDiode*> >
  SiChargedDiodeOrderedSet;
                 

// Iterator typedef to make it easier
typedef SiChargedDiodeOrderedSet::iterator SiChargedDiodeOrderedIterator;

class SiChargedDiodeCollection : Identifiable {
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:


  // Constructor with parameters:
  //  ref. to the detector element for this collection
  SiChargedDiodeCollection( );

  SiChargedDiodeCollection(const InDetDD::SolidStateDetectorElementBase* );


  // Destructor:
  ~SiChargedDiodeCollection();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // detector element:
  const InDetDD::SolidStateDetectorElementBase * element() const;

  // wafer identifier for this collection
  virtual Identifier identify() const override final;
  virtual IdentifierHash identifyHash() const override final;

  // id helper for this collection
  const AtlasDetectorID* id_helper();
  
  // detector design:
  const InDetDD::DetectorDesign &design() const;

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
  void setDetectorElement(const InDetDD::SolidStateDetectorElementBase *SiElement);

  // Add a new SiCharge to the collection
  // (add or merge in an existing SiChargedDiode):
  //   Diode which contains the new charge
  //   SiCharge to be added.
  template <class T> 
  void add(const InDetDD::SiCellId & diode, const T &charge);

  template <class T> 
  void emplace_charge(const InDetDD::SiCellId & diode, const T &charge);

  bool AlreadyHit(const InDetDD::SiCellId & siId);
  bool AlreadyHit(const Identifier & id);
  SiChargedDiodeIterator begin();
  SiChargedDiodeIterator end();
  SiChargedDiodeOrderedIterator orderedBegin();
  SiChargedDiodeOrderedIterator orderedEnd();
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
  void order();
  
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  SiChargedDiodeMap m_chargedDiodes; // list of SiChargedDiodes 
  SiChargedDiodeOrderedSet m_orderedChargedDiodes; // list of SiChargedDiodes 
  const InDetDD::SolidStateDetectorElementBase* m_sielement; // detector element
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

// Set the DetectorElement
inline void SiChargedDiodeCollection::setDetectorElement(const InDetDD::SolidStateDetectorElementBase *SiElement) 
{
  m_sielement=SiElement;
}

inline SiChargedDiodeMap &SiChargedDiodeCollection::chargedDiodes()
{
  return m_chargedDiodes;
}

// access to the element
inline const InDetDD::SolidStateDetectorElementBase *SiChargedDiodeCollection::element() const
{
  return m_sielement;
}

// access to the design
inline const InDetDD::DetectorDesign &SiChargedDiodeCollection::design() const
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

inline SiChargedDiodeOrderedIterator SiChargedDiodeCollection::orderedBegin() 
{
  if (m_orderedChargedDiodes.empty() && !m_chargedDiodes.empty()) {
    order();
  }
  return m_orderedChargedDiodes.begin();
}

inline SiChargedDiodeOrderedIterator SiChargedDiodeCollection::orderedEnd() 
{
  if (m_orderedChargedDiodes.empty() && !m_chargedDiodes.empty()) {
    order();
  }
  return m_orderedChargedDiodes.end();
}

inline bool SiChargedDiodeCollection::empty() const {
  return m_chargedDiodes.empty();
}

template<class T>
void SiChargedDiodeCollection::emplace_charge(const InDetDD::SiCellId & diode, const T& charge) 
{
    InDetDD::SiReadoutCellId roCell=design().readoutIdOfCell(diode);
    if (!roCell.isValid()) { // I don't think this can occur at this stage but cant hurt.
      MsgStream log(Athena::getMessageSvc(),"SiChargedDiodeCollection");
      log << MSG::FATAL << "Could not create SiReadoutCellId object !"<< endmsg;
    }
    // create a new charged diode
    SiChargedDiode chargedDiode(diode,roCell);
    // add the new charge to it
    chargedDiode.add(charge);
    if (charge.processType() == SiCharge::extraNoise) SiHelper::noise(chargedDiode,true);
    // add the new charged diode to the charged diode collection
    auto p = m_chargedDiodes.emplace(diode,chargedDiode);
    if (!m_orderedChargedDiodes.empty()) {
      m_orderedChargedDiodes.insert (&p.first->second);
    }
}

template <class T>
void SiChargedDiodeCollection::add(const InDetDD::SiCellId & diode,
				   const T & charge)
{
  // check the pointer is correct
  if (!diode.isValid()) return;
  SiChargedDiodeIterator the_diode = m_chargedDiodes.find(diode);
  if(the_diode != m_chargedDiodes.end()) the_diode->second.add(charge);
  else emplace_charge(diode, charge);
}

#endif // SIDIGITIZATION_SICHARGEDDIODECOLLECTION_H

