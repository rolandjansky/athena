/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///To include TRT_RDO_Container to SG db

#include "SGTools/BaseInfo.h"
#include "InDetRawData/TRT_RDO_Container.h"

SG_ADD_BASE( TRT_RDO_Container, InDetRawDataContainer<InDetRawDataCollection<TRT_RDORawData> > );

