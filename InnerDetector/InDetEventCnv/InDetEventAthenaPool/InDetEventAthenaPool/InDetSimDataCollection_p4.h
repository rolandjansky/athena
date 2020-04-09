/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTION_P4_H
#define INDETSIMDATACOLLECTION_P4_H

#include <vector>
#include "InDetEventAthenaPool/InDetSimData_p3.h"
#include "Identifier/Identifier.h"

class InDetSimDataCollection_p4
{


public:

    InDetSimDataCollection_p4()
        { } ;

    // container cnv does conversion
    friend class InDetSimDataCollectionCnv_p4;

private:
    std::vector<std::pair<Identifier::value_type, InDetSimData_p3> > m_simdata;
};

#endif
