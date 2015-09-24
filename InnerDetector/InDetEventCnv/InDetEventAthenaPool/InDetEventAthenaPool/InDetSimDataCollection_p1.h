/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTION_P1_H
#define INDETSIMDATACOLLECTION_P1_H
                                                                                                                                                             
#include <vector>
#include "InDetEventAthenaPool/InDetSimData_p1.h"
#include "Identifier/Identifier.h"

class InDetSimDataCollection_p1
{
                                                                                                                                                             
                                                                                                                                                             
public:
                                                                                                                                                             
    InDetSimDataCollection_p1()
        { } ;
                                                                                                                                                             
    // container cnv does conversion
    friend class InDetSimDataCollectionCnv_p1;
                                                                                                                                                             
private:
    std::vector<std::pair<Identifier32::value_type, InDetSimData_p1> > m_simdata;
};
                                                                                                                                                             
#endif
                                                                                                                                                             

