/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATA_INDETTIMECOLLECTION_H
#define INDETRAWDATA_INDETTIMECOLLECTION_H

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"


typedef DataVector< std::pair<uint32_t,  unsigned int > > InDetTimeCollection;
CLASS_DEF(InDetTimeCollection ,1103971028,1 )


#endif
