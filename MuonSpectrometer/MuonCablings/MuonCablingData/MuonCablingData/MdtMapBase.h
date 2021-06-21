/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTMAPBASE_H
#define MUONMDT_CABLING_MDTMAPBASE_H

/**********************************************
 *
 * @brief Base class for the mdt mapping objects
 *
 * @author Stefano Rosati <Stefano.Rosati@roma1.infn.it>
 *
 **********************************************/

#include "GaudiKernel/MsgStream.h"


#include <stdint.h>
#include <string>
#include <map>

template<class T> class MdtMapBase 
{

 public:

  typedef std::map< uint8_t, std::unique_ptr<T>, std::less<uint8_t> > MapOfItems;


  /** constructor */
  MdtMapBase(uint8_t moduleId, const std::string itemName);

  /** destructor */
  ~MdtMapBase() = default;

  /** access function */
  uint8_t moduleId() const {return m_moduleId;}

  /** cleanup */
  void clear();

  /** get full map (used for testing) */
  const MapOfItems* getListOfElements() const {return &m_mapOfItems;}

 protected:
  
  /** Module ID */
  uint8_t m_moduleId;  
  /** Output level and message service */
  bool m_debug;

  /** set function */
  bool addItem(uint8_t itemId, T* item, MsgStream &log);

  /** get function - fast, used for online->offline conversion */
  T* getItem(uint8_t itemId) const;

 private:

  /** name of the mapped item */
  const std::string m_itemName;
  
  /** pointer to the current item */
  //T* m_currentItem;

  /** map containing the items */
  MapOfItems m_mapOfItems;

  /** private access function */
  T* findItem(uint8_t itemId) const;  

};


/** default constructor */
template<class T> MdtMapBase<T>::MdtMapBase(uint8_t moduleId, std::string itemName) :
m_moduleId(moduleId), m_itemName(std::move(itemName))//, m_currentItem(NULL)
{ 
}


template<class T> void MdtMapBase<T>::clear() {
  // deleting all the objects in the map
  m_mapOfItems.clear();
}

/** Add an item to the map */
template<class T> bool MdtMapBase<T>::addItem(uint8_t itemId, T* item, MsgStream &log) {

  bool itemAdded = false;
  
  // check if the id are matching
  if (itemId != item->moduleId() ) {
    log << MSG::ERROR << m_itemName << " Id different from the module one: " 
	   << MSG::hex << (int) item->moduleId() << MSG::dec << endmsg;
    return false;
  }
  
  // check if the item has been already added
  if (m_mapOfItems.find(itemId) != m_mapOfItems.end()) {
    log << MSG::ERROR << m_itemName << " with id " << MSG::hex << (int) itemId
	   << MSG::dec << " already added in moduleId: " << MSG::hex 
	   << (int) m_moduleId << endmsg;
  }
  else {
    if (m_debug) {
      log << MSG::VERBOSE << "Adding " << m_itemName << " with id: 0x" 
	     << MSG::hex << (int) itemId << MSG::dec << " to module 0x" 
	     << MSG::hex << (int) m_moduleId << MSG::dec << endmsg;
    }
    m_mapOfItems.insert(std::pair<uint8_t,std::unique_ptr<T>>(itemId,item));
    itemAdded=true;
  }

  return itemAdded;
}


/** return the item for a given access key (onlineId) */
template<class T> T* MdtMapBase<T>::getItem(uint8_t itemId) const{
  return findItem(itemId);
}


/** find the item in the datamember map */
template<class T> T* MdtMapBase<T>::findItem(uint8_t itemId) const{

  typename MapOfItems::const_iterator it = m_mapOfItems.find(itemId);
  if (it!=m_mapOfItems.end()) {
    return (*it).second.get();
  } else {    
    return nullptr;
  }
}


#endif    //  MUONMDT_CABLING_MDTMAPBASE
