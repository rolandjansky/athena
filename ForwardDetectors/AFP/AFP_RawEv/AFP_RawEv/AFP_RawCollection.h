/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWCOLLECTION_H
#define AFP_RAWCOLLECTION_H

#include "AFP_RawEv/AFP_RawCollectionHead.h"
#include <list>

/// Class representing collection of silicon detector data
template <typename RAWDATA_T>
class AFP_RawCollection : public AFP_RawCollectionHead
{
public:
  /// @copydoc AFP_RawCollection::m_dataRecords;
  const std::list<RAWDATA_T>& dataRecords() const {return m_dataRecords;}
  
  /// Returns iterators to the beginning and end of the list
  typename std::list<RAWDATA_T>::iterator begin() {return m_dataRecords.begin();}
  typename std::list<RAWDATA_T>::iterator end() {return m_dataRecords.end();}
  
  /// Creates a new empty data record in the collection and returns reference to it.
  RAWDATA_T& newDataRecord () {m_dataRecords.emplace_back(); return m_dataRecords.back();}
  
private:
  /// List of silicon detector data records
  std::list<RAWDATA_T> m_dataRecords;
};

#endif
