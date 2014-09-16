/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_ALINEPARCONTAINER_H
#define MUONALIGNMENTDATA_ALINEPARCONTAINER_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "MuonAlignmentData/ALinePar.h"

class ALineParContainer : public DataVector<ALinePar>
{
 public:
  ALineParContainer();
  ~ALineParContainer();

};

inline ALineParContainer::ALineParContainer():
  DataVector<ALinePar>()
{ } 

inline ALineParContainer::~ALineParContainer()
{ } 

CLASS_DEF(ALineParContainer, 1324994856, 1)

#endif  // MUONALIGNMENTDATA_ALINEPARCONTAINER_H
