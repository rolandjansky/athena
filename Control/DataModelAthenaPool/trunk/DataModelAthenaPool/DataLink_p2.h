/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_DATALINK_p2_H
#define DATAMODELATHENAPOOL_DATALINK_p2_H

/** @file DataLink_p2.h
 *  @brief This file contains the class definition for DataLink_p2 and  DataLinkVector_p2
 *  @author Marcin.Nowak@cern.ch
 **/

#include <vector>
#include <stdint.h>

/** @class DataLink_p2
 *  @brief Persistent representation of DataLink
    contains the StoreGate key of the container object to which the link is pointing 
 **/
 
class DataLink_p2 {
public:
  DataLink_p2() : m_SGKeyHash(0) {}

  uint32_t     m_SGKeyHash;     // hash for StoreGate container name
};



/** @class DataLinkVector_p2
 *  @brief Persistent representation of DataLinkVector
 **/
typedef std::vector<DataLink_p2>	DataLinkVector_p2;	


#endif



