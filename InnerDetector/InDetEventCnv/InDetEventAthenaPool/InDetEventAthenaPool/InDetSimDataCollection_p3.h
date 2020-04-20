/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTION_P3_H
#define INDETSIMDATACOLLECTION_P3_H

#include <vector>
#include <utility>
#include "InDetEventAthenaPool/InDetSimData_p2.h"
#include "Identifier/Identifier.h"

class InDetSimDataCollection_p3
{


public:

  InDetSimDataCollection_p3()
  { } ;

  // container cnv does conversion
  friend class InDetSimDataCollectionCnv_p3;

private:
  std::vector<std::pair<Identifier::value_type, InDetSimData_p2> > m_simdata;
};

#endif
