/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_DATALINK_P1_H
#define DATAMODELATHENAPOOL_DATALINK_P1_H

/** @file DataLink_p1.h
 *  @brief This file contains the class definition for DataLink_p1 and  DataLinkVector_p1
 *  @author Marcin.Nowak@cern.ch
 **/

#include <string>
#include <vector>
#include <stdint.h>

/** @class DataLink_p1
 *  @brief Persistent representation of DataLink
    contains the StoreGate key of the container object to which the link is pointing 
 **/
 
class DataLink_p1 {
public:
  DataLink_p1() : m_SGKeyHash(0) {}

  std::string	m_link;
  uint32_t     m_SGKeyHash;     // replaces m_contName
};



/** @class DataLinkVector_p1
 *  @brief Persistent representation of DataLinkVector
 **/
typedef std::vector<DataLink_p1>	DataLinkVector_p1;	


#endif



