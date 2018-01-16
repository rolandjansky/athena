/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTATHENAPOOLDICT_H
#define INDETEVENTATHENAPOOLDICT_H

#include "InDetEventAthenaPool/InDetSimData_p1.h"
#include "InDetEventAthenaPool/InDetSimData_p2.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p1.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p2.h"
#include "InDetEventAthenaPool/InDetSimDataCollection_p3.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "InDetEventAthenaPool/SCT3_RawData_p1.h"
#include "InDetEventAthenaPool/SCT3_RawData_p2.h"
#include "InDetEventAthenaPool/SCT3_RawData_p3.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"
#include "InDetEventAthenaPool/InDetRawDataContainer_p1.h"
#include "InDetEventAthenaPool/InDetRawDataContainer_p2.h"
#include "InDetEventAthenaPool/SCT_RawDataContainer_p1.h"
#include "InDetEventAthenaPool/SCT_RawDataContainer_p2.h"
#include "InDetEventAthenaPool/SCT_RawDataContainer_p3.h"

namespace InDetEventAthenaPoolCnvDict
{
    std::pair<unsigned int, InDetSimData_p1> t1;
    std::pair<unsigned long long, InDetSimData_p1> t2;
    std::vector<std::pair<unsigned int, InDetSimData_p1> > t3;
    std::vector<std::pair<unsigned long long, InDetSimData_p1> > t4;
}

#endif
