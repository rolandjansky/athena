/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_CSCRDOCONAINTER_H
#define MUONRDO_CSCRDOCONAINTER_H



#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataCollectionIdHash.h"
#include "MuonRDO/CscRawDataCollection_Cache.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/**
  This container provides access to collections of CSC RDOs and 
  a mechanism for recording them.
  @author Ketevi A. Assamagan
*/
class CscRawDataContainer
  : public IdentifiableContainer<CscRawDataCollection>
{
public:  
  CscRawDataContainer();
  CscRawDataContainer(unsigned int hashmax);
  CscRawDataContainer(CscRawDataCollection_Cache* cache);
  
  virtual ~CscRawDataContainer(); 
  
  typedef IdentifiableContainer<CscRawDataCollection> MyBase; 
  
  /// Insert a RDO
  void push_back(CscRawData* rawData);

  /// Convert identifier to idhash
  unsigned int idToHash(unsigned int id) const;

  /// class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}

private:

    static const CscRawDataCollectionIdHash& hashFcn();

};

CLASS_DEF(CscRawDataContainer,4183,0)

#endif



