/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaHitsCollectionHelper_H
#define AthenaHitsCollectionHelper_H

#include <string>
// Generic interface to a hit collection. it should be used to
// remove any dependency between sensitive detectors and the
// underlying storing system..
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGateSvc.h"

class AthenaHitsCollectionHelper {
public:
  AthenaHitsCollectionHelper();
  virtual ~AthenaHitsCollectionHelper();
  // following can't be virtual
  template <typename collectionType>
  void ExportCollection(collectionType* collection, bool allowMods=false)
  {
    if (!m_storeGate) initialize();
    StatusCode status = m_storeGate->record(collection,collection->Name(),allowMods);
    if (status.isFailure()) {
      std::cout
        << "Failed to record and lock HitCollection (" << collection->Name() << ") in StoreGate!"
        << std::endl;
    }
  }
  template <typename collectionType>
  const DataHandle< collectionType > RetrieveCollection(std::string name="")
  {
    if (!m_storeGate) initialize();
    const DataHandle< collectionType > collMDT;
    if (name.empty()) {
      StatusCode status = m_storeGate->retrieve( collMDT );
    } else {
      StatusCode status = m_storeGate->retrieve( collMDT , name);
    }
    return collMDT;
  }
  template <typename collectionType>
  DataHandle< collectionType > RetrieveNonconstCollection(const std::string name="")
  {
    if (!m_storeGate) initialize();
    DataHandle< collectionType > coll;
    if (name.empty()) {
      StatusCode status = m_storeGate->retrieve( coll );
    } else {
      if (m_storeGate->contains<collectionType>(name)) {
        StatusCode status = m_storeGate->retrieve( coll , name);
      }
    }
    if (!coll) {
      collectionType* newcoll=new collectionType(name);
      ExportCollection(newcoll,true);
      m_storeGate->retrieve( coll, name);
    }
    return coll;
  }
  template <typename collectionType>
  void SetConstCollection(collectionType* collection)
  {
    if (!m_storeGate) initialize();
    StatusCode status = m_storeGate->setConst(collection);
    if (status.isFailure()) {
      std::cout
        << "Failed to set HitCollection ("<< collection->Name() << ") const in StoreGate!"
        << std::endl;
    }
  }
private:
  StoreGateSvc*   m_storeGate;
  void initialize();      //!< Perform the (lazy) initialization
};

#endif
