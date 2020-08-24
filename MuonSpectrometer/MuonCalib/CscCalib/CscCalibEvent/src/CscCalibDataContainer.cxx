/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalibEvent/CscCalibDataContainer.h"
#include "EventContainers/SelectAllObject.h"
#include <map>
#include <cassert>
#include <iostream>
#include <TString.h> // for Form

CscCalibDataContainer::CscCalibDataContainer(int maxhash) 
	: IdentifiableContainer<CscCalibDataCollection>(maxhash) {
}

/** return the class ID */
const CLID& CscCalibDataContainer::classID() {
  return ClassID_traits<CscCalibDataContainer>::ID();       
}

/** Insert a CscCalibData. */
void CscCalibDataContainer::push_back(CscCalibData* calibData) {

  /** find the collection where to insert this CalibData */ 
  IdentifierHash channelHash = calibData->idHash(); 
  // I don't think there's any need for this any more. EJWM
  // Not sure if the "Identifier existing" check is important though
     const CscCalibDataCollection* const_coll = MyBase::indexFindPtr(channelHash);
     if(const_coll!=nullptr) {
       CscCalibDataCollection * coll = const_cast<CscCalibDataCollection*>(const_coll);
       coll->push_back(calibData); 
     } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nCscCalibDataContainer::push_back() - ERROR: Collection does not exist", __FILE__, __LINE__));
     }
  return ; 
}

/** Return the total number of CalibDatas in the container */
CscCalibDataContainer::size_type CscCalibDataContainer::calibData_size() const {
  CscCalibDataContainer::size_type count = 0;
  CscCalibDataContainer::const_iterator it = begin();
  CscCalibDataContainer::const_iterator iend = end();
  for (; it != iend; ++it ) {
    count += (*it)->size();
  }
  return count;
}


/** Output stream - for printing */ 
std::ostream& operator<<(std::ostream& lhs, const CscCalibDataContainer& rhs) {
  lhs << "CscDataCollectionContainer has " << rhs.calibData_size() << " CalibData:" << std::endl;
  typedef SelectAllObject<CscCalibDataContainer> SELECTOR; 
  SELECTOR sel(&rhs);
  SELECTOR::const_iterator it = sel.begin(); 
  SELECTOR::const_iterator it_end = sel.end(); 
  bool first = true;
  for ( ; it!=it_end;++it) { 
    if ( first ) {
       first = false;
    } else {
      lhs << std::endl;
    }
    lhs << "  " << *it;
  }
  return lhs;
}

