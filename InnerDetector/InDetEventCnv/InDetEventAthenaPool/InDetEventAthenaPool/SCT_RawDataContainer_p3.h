/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATACONTAINER_P3_H
#define SCT_RAWDATACONTAINER_P3_H

/**

* Persistent represenation of an SCT_RawDataContainer.
* New version to deal with SCT_RawData_p3
* N. Barlow Apr 2008.

*/

#include <vector>
#include <string>
#include "InDetEventAthenaPool/SCT3_RawData_p3.h"
#include "InDetEventAthenaPool/InDetRawData_p2.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"


class SCT_RawDataContainer_p3   
{
 public:
/// Default constructor
  SCT_RawDataContainer_p3 ();
  friend class SCT_RawDataContainerCnv_p3;
 private:
  std::vector<InDetRawDataCollection_p1>  m_collections;
  std::vector<InDetRawData_p2>            m_rawdata;
  std::vector<SCT3_RawData_p3>            m_sct3data;
  /** because ROOT isn't good at storing vectors of vectors,
   *  store the individual strip errors in the persistent container
   *  rather than in the persistent SCT3_RawData_p3 itself..
   */
  std::vector<int>                        m_allErrorsInContainer;
  std::vector<int>                        m_numErrorsInRDO;
};


/** inlines */

inline
SCT_RawDataContainer_p3::SCT_RawDataContainer_p3 () {}

#endif
