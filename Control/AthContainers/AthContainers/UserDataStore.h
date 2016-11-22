/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef ATHCONTAINERS_USERDATASTORE_H
#define ATHCONTAINERS_USERDATASTORE_H

#ifdef XAOD_STANDALONE

/**
 *  @short Placeholder class when using AthContainers in ROOT
 *
 *         The xAOD format doesn't consider the UserDataStore at the
 *         moment, but it could change later on. For now, because the
 *         dictionary generation expects a class called UserDataStore
 *         to be available, we use a dummy class.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 764634 $
 * $Date: 2016-07-27 16:09:53 +0200 (Wed, 27 Jul 2016) $
 */
class UserDataStore {

public:
   /// Default constructor
   UserDataStore() {}

}; // class UserDataStore;

#else

#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IAthenaBarCode.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <map>

//#include <ext/hash_map>
//#include <boost/unordered_map.hpp>


#include "AthContainers/tools/UDSLabelHashTable.h"
#include "SGTools/CLASS_DEF.h"

#include "boost/any.hpp"

/** @class UserDataStoreObj
 * @brief User Data store for type T
 */

class UserDataStore {
 
  friend class UserDataStoreCnv_p1;

public:

  /// Default constructor
  UserDataStore();  

  /**
   * @brief Record method with template payload
   * @param abc A data-object implementing the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param value Decoration payload 
   */
  template<typename T>
  StatusCode record(const IAthenaBarCode& obj, const std::string& label, const T& value); 


  /**
   * @brief Retrieve method with template payload
   * @param abc A data-object implementing the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] value Decoration payload 
   */
  template<typename T>
  StatusCode retrieve(const IAthenaBarCode& obj, const std::string& label, T& value) const; 


  /** 
   * @brief Contains method for element decorations
   * @param abc A data-object implementing the IAthenaBarCode interface
   * @param label String label of the decoration
   * @return True if the decoration exists
   */ 
  bool contains(const IAthenaBarCode& obj, const std::string& label) const;


  /**
   * @brief Table-of-contents method
   * @param barcode A data-object implementing the IAthenaBarCode interface
   * @return A vector of labels existing for this object
   */
  std::vector<const std::string*> getLabels(const AthenaBarCode_t& barcode) const;


  
  /**
   * @brief Record method with boost::any as payload 
   * @param abc A data-object implementing the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param value Decoration payload 
   */
  StatusCode recordAny(const IAthenaBarCode& obj, const std::string& label, const boost::any& value);


  /**
   * @brief Retrieve method with boost::any as payload
   * @param abc A data-object implementing the IAthenaBarCode interface
   * @param label String label of the decoration
   * @param[out] value Decoration payload 
   */
  StatusCode retrieveAny(const IAthenaBarCode& obj, const std::string& label, const boost::any*& value) const;


  /**
   * @brief Method to set the AthenaBarCode of this event (called by UserDataSvc)
   * @param eventABC The AthenaBarCode
   */
  void setEventABC(const AthenaBarCode_t& eventABC);



  /**
   * @brief Method to get the AthenaBarCode of this event (called by UserDataSvc)
   * @return The AthenaBarCode
   */
  AthenaBarCode_t getEventABC() const { return m_eventABC; }


  enum NotFoundTypes {
    ALLGOOD=0,
    BARCODE,
    LABEL
  };

private: 

  // For experimental use of hash-maps instead of std::map
  // class abcHashFcn {
  //   public:
  //     size_t operator()(const AthenaBarCode_t abc) const { return ((abc & 0xFFFFFFFF)>>6); } //Extract the 26 counter-bits
  //   };
  
  typedef boost::any payload_t; 

  typedef UDSLabelHashTable::hash_t index_t;

  typedef std::map<AthenaBarCode_t,std::map<index_t, payload_t > > mapmap_t;
  //typedef __gnu_cxx::hash_map<AthenaBarCode_t,std::map<index_t, payload_t >, abcHashFcn > mapmap_t;
  //typedef boost::unordered_map<AthenaBarCode_t,std::map<index_t, payload_t >, abcHashFcn > mapmap_t;

  typedef std::map<index_t, payload_t >::iterator iterator;
  typedef std::map<index_t, payload_t >::const_iterator const_iterator;

  mapmap_t m_data;

  //find a label (const version)
  const_iterator find(const AthenaBarCode_t& barcode, const std::string& label) const;

  //Update the cached map-iterator m_lastIt;
  //void updateIt(const AthenaBarCode_t& barcode);

  const_iterator m_notFoundIt;
  const std::map<index_t, payload_t > m_defaultEntry;

  UDSLabelHashTable m_labelTable;

  ///Cache of last barcode/object (const)
  mutable  mapmap_t::const_iterator m_lastIt;

  ///Cache of last barcode/object (non-const)
  mutable  mapmap_t::iterator m_lastIt_nc;

  ///AthenaBarCode for this event
  AthenaBarCode_t m_eventABC;

  ///Cached Message Stream
  mutable MsgStream m_msg;

  mutable NotFoundTypes m_whyNotFound;

};
  
CLASS_DEF( UserDataStore , 51567825 , 1 )
  
template<typename T>
StatusCode UserDataStore::retrieve(const IAthenaBarCode& obj, const std::string& label, T& value) const {
  const boost::any* aValue=0;
  StatusCode sc=this->retrieveAny(obj,label, aValue);
  if (sc.isFailure()) return sc;
  if (typeid(T) != aValue->type()) { 
    m_msg << MSG::ERROR << "Typeid mismatch while retrieving user data with label " << label << endmsg;
    return StatusCode::FAILURE;
  }
  const T* decoPtr=boost::any_cast<T>(aValue);
  value=*decoPtr; //Deep copy! 
  return StatusCode::SUCCESS;
}


template<typename T>
StatusCode UserDataStore::record(const IAthenaBarCode& obj, const std::string& label, const T& value) {
  boost::any aValue(value);
  return this->recordAny(obj,label, aValue);
}

#endif // not XAOD_STANDALONE


#endif // not ATHCONTAINERS_USERDATASTORE_H
