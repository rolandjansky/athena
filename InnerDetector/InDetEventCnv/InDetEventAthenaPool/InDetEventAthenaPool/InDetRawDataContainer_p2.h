/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATACONTAINER_P2_H
#define INDETRAWDATACONTAINER_P2_H

/*

Persistent represenation of an IndetRawDataContainer.
This is used for all the RDO conainers with base RDO objects.
The converter will take care of creating TRT/SCT/Pixel
RDO containers out of it. 

The assumption is that objects contained in a container
are all of the same type, eg containers are hompgeneous.
This persistent container should only be used for objects
that contain InDetRawData_p2. 

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "InDetEventAthenaPool/InDetRawData_p2.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"


class InDetRawDataContainer_p2   
{
 public:
/// Default constructor
  InDetRawDataContainer_p2 ();
  friend class TRT_LoLumRawDataContainerCnv_p2;
  friend class SCT1_RawDataContainerCnv_p2;
  friend class Pixel1RawDataContainerCnv_p2;
 private:
  std::vector<InDetRawDataCollection_p1>  m_collections;
  std::vector<InDetRawData_p2>            m_rawdata;
};


// inlines

inline
InDetRawDataContainer_p2::InDetRawDataContainer_p2 () {}

#endif
