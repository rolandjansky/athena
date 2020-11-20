/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATACONTAINER_P3_H
#define INDETRAWDATACONTAINER_P3_H

/*
   For the TRT, the channel Ids are dropped from the persistent container and so
   the p2 version "std::vector<InDetRawData_p2> m_rawdata;" is replaced with
   the p3 version "std::vector<unsigned int>    m_rawdata;".
*/

#include <vector>
#include <string>
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"

class InDetRawDataContainer_p3
{
 public:
/// Default constructor
  InDetRawDataContainer_p3 ();
  friend class TRT_LoLumRawDataContainerCnv_p3;
 private:
  std::vector<InDetRawDataCollection_p1>  m_collections;
  std::vector<unsigned int>               m_rawdata;
};

// inlines
inline
InDetRawDataContainer_p3::InDetRawDataContainer_p3 () {}

#endif
