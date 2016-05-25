/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATACONTAINER_P1_H
#define INDETRAWDATACONTAINER_P1_H

/*

Persistent represenation of an IndetRawDataContainer.
This is used for all the RDO conainers with base RDO objects.
The converter will take care of creating TRT/SCT/Pixel
RDO containers out of it. 

The assumption is that objects contained in a container
are all of the same type, eg containers are hompgeneous.
This persistent container should only be used for objects
that contain InDetRawData_p1. 

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"


class InDetRawDataContainer_p1   
{
 public:
/// Default constructor
  InDetRawDataContainer_p1 ();
  friend class TRT_LoLumRawDataContainerCnv_p1;
  friend class SCT1_RawDataContainerCnv_p1;
  friend class Pixel1RawDataContainerCnv_p1;
 private:
  std::vector<InDetRawDataCollection_p1>  m_collections;
  std::vector<InDetRawData_p1>            m_rawdata;
};


// inlines

inline
InDetRawDataContainer_p1::InDetRawDataContainer_p1 () {}

#endif
