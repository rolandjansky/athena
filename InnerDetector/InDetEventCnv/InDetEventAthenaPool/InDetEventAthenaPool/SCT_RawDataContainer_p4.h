/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATACONTAINER_P4_H
#define SCT_RAWDATACONTAINER_P4_H

/**

* Persistent represenation of an SCT_RawDataContainer.
* New version to deal with SCT_RawData_p4
* Susumu Oda Oct 2018.

*/

#include <vector>
#include <string>
#include "InDetEventAthenaPool/SCT3_RawData_p4.h"
#include "InDetEventAthenaPool/InDetRawData_p2.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"


class SCT_RawDataContainer_p4
{
 public:
/// Default constructor
  SCT_RawDataContainer_p4();
  friend class SCT_RawDataContainerCnv_p4;
 private:
  std::vector<InDetRawDataCollection_p1>  m_collections;
  std::vector<InDetRawData_p2>            m_rawdata;
  std::vector<SCT3_RawData_p4>            m_sct3data;
  /** because ROOT isn't good at storing vectors of vectors,
   *  store the individual strip errors in the persistent container
   *  rather than in the persistent SCT3_RawData_p4 itself..
   */
};


/** inlines */

inline
SCT_RawDataContainer_p4::SCT_RawDataContainer_p4() {}

#endif // SCT_RAWDATACONTAINER_P4_H
