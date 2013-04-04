/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTION_P2_H
#define INDETSIMDATACOLLECTION_P2_H
                                                                                                                                                             
#include <vector>
#include "InDetEventAthenaPool/InDetSimData_p1.h"
#include "Identifier/Identifier.h"

class InDetSimDataCollection_p2
{
                                                                                                                                                             
                                                                                                                                                             
public:
                                                                                                                                                             
    InDetSimDataCollection_p2()
        { } ;
                                                                                                                                                             
    // container cnv does conversion
    friend class InDetSimDataCollectionCnv_p2;
                                                                                                                                                             
private:
    std::vector<std::pair<Identifier::value_type, InDetSimData_p1> > m_simdata;
};
                                                                                                                                                             
#endif
                                                                                                                                                             

