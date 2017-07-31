/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalibEvent/CscCalibDataContainer.h"
#include "EventContainers/SelectAllObject.h"
#include <map>
#include <cassert>
#include <iostream>

using std::ostream;

/********************************************
 Implementation of CscCalibDataContainer
 Author: Ketevi A. Assamagan
*********************************************/

CscCalibDataContainer::CscCalibDataContainer(int maxhash) 
	: IdentifiableContainer<CscCalibDataCollection>(maxhash) {
  //m_hashFunc.setHelper(helper);
  //DataLinkVector<CscCalibDataCollection>::init(m_hashFunc.max(), offset);
}

/** Destructor */
CscCalibDataContainer::~CscCalibDataContainer() {
}

/** for the default constructor without parameters */
// void CscCalibDataContainer::setHashFunc (const CscIdHelper* helper, int offset) {
//   m_hashFunc.setHelper(helper);
//   DataLinkVector<CscCalibDataCollection>::init(m_hashFunc.max(), offset);
// }

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
//  const CscIdHelper * cscHelper = m_hashFunc.cscIdHelper();
//  IdContext context = cscHelper->channel_context();
//  Identifier channelId;
//  if (!cscHelper->get_id(channelHash,channelId,&context)) {
//     Identifier elemId = cscHelper->elementID(channelHash);
     MyBase::const_iterator it = MyBase::indexFind(channelHash);
     if(it!=MyBase::end()) {
       const CscCalibDataCollection* const_coll = (*it)  ;
       CscCalibDataCollection * coll = const_cast<CscCalibDataCollection*>(const_coll);
       coll->push_back(calibData); 
     } else 
       std::cout <<"CscCalibDataContainer::ERROR : Collection does not exist "<<std::endl;
//  } else std::cout << "CscCalibDataContainer::ERROR : could find Idenitfier for this element " 
//                   << channelHash << std::endl;
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
ostream& operator<<(ostream& lhs, const CscCalibDataContainer& rhs) {
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


