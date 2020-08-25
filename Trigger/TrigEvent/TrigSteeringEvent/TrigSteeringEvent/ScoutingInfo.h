/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @class  : ScoutingInfo
 *
 *
 * @author Noemi Calace <noemi.calace@cern.ch>  - CERN
 *
 **********************************************************************************/

#ifndef TRIGSTEERINGEVENT_SCOUTINGINFO_H
#define TRIGSTEERINGEVENT_SCOUTINGINFO_H
#include <vector>
#include <string>
#include <set>
#include "GaudiKernel/ClassID.h"

class ScoutingInfo {
public:
  ScoutingInfo();
  ~ScoutingInfo();

  /** @brief add a new element to m_clidName
   *
   **/
  void add(CLID clid, const std::string& name);

  /** @brief add a new element to m_clidName
   *
   **/
  void add(const std::pair<CLID, std::string>& clid_Name);

  /** @brief add elements in the vector passed into m_clidName
   *
   **/
  void add(const std::vector< std::pair<CLID, std::string> >& clid_Name);

  /** @brief add elements in the set passed into m_clidName
   *
   **/
  void add(const std::set< std::pair<CLID, std::string> >& clid_Name);

  /** @brief gets back all elements in  m_clidName
   * (fills up clid_Name vector given by reference)
   * @warning note that vector is not cleared
   **/
  void get(std::set< std::pair< CLID, std::string> >& clid_Name) const;

  /** @brief gets back all <CLID, CollectionName> added
   *
   **/
  const std::set< std::pair< CLID, std::string> >& get() const;

  /** @brief gets back all CLIDs in  m_clidName
   * (fills up clid vector given by reference)
   * @warning note that vector is not cleared
   **/
  void getclid(std::vector<CLID>& clid) const;
  
  /** @brief gets back all CollectionNames in  m_clidName
   * (fills up name vector given by reference)
   * @warning note that vector is not cleared
   **/
  void getName(std::vector<std::string>& name) const;
  
  /** @brief clears the vector of <CLID, CollectionName>
   *
   **/
  void clear();

  /** @brief merges two ScoutingInfo objects
   * I.e. it the subject of the method invocation will have a vector of pairs of CLID and CollectionName which is union of the two
   **/
  void merge(const ScoutingInfo& sI);


private:
  std::set< std::pair<CLID, std::string> > m_clidName; // Vector of pairs of CLID and CollectionName
  
};

#endif
