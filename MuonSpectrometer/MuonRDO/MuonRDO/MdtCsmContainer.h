/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MDTCSMCONTAINER_H
#define MUONRDO_MDTCSMCONTAINER_H


#include <vector>
#include <string>
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmIdHash.h"
#include "SGTools/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/** This container provides acces to the MDT RDOs
 @author Stefano Rosati, Mar 2003
*/
class MdtCsmContainer
   :public IdentifiableContainer<MdtCsm> {
public:  
   MdtCsmContainer() ; 
   MdtCsmContainer( unsigned int hashmax) ; 
  ~MdtCsmContainer() ; 

  typedef MdtCsm::size_type size_type ; 
  typedef IdentifiableContainer<MdtCsm> MyBase; 

  size_type digit_size() const ; 

  /// Convert identifier to idhash
  unsigned int idToHash(unsigned int id) const;

  // IdentifiableContainer is still a DataObject
  // Put CLID here. 
  static const CLID& classID();
  
  /** return class ID */
  virtual const CLID& clID() const
    {
      return classID(); 
    }
  
private:

    static const MdtCsmIdHash& hashFcn();
  
};

CLASS_DEF(MdtCsmContainer,4187,0) 

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const MdtCsmContainer& rhs);


#endif     // MUONRDO_MDTCSMCONTAINER_H
