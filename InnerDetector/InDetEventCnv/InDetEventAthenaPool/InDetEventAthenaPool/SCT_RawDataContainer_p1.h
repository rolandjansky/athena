/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATACONTAINER_P1_H
#define SCT_RAWDATACONTAINER_P1_H

/*

Persistent represenation of an SCT_RawDataContainer.
This can contain both SCT3_RawData at M4
and SCT1_RawData

Author: Davide Costanzo - 23 Aug 2007

*/

#include <vector>
#include <string>
#include "InDetEventAthenaPool/SCT3_RawData_p1.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"


class SCT_RawDataContainer_p1   
{
 public:
/// Default constructor
  SCT_RawDataContainer_p1 ();
  friend class SCT_RawDataContainerCnv_p1;
  //private:
  std::vector<InDetRawDataCollection_p1>  m_collections;
  std::vector<InDetRawData_p1>            m_rawdata;
  std::vector<SCT3_RawData_p1>            m_sct3data;
};


// inlines

inline
SCT_RawDataContainer_p1::SCT_RawDataContainer_p1 () {}

#endif
