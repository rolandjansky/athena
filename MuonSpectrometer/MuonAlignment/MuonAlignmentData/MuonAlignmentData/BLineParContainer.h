/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_BLINEPARCONTAINER_H
#define MUONALIGNMENTDATA_BLINEPARCONTAINER_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "MuonAlignmentData/BLinePar.h"

class BLineParContainer : public DataVector<BLinePar>
{
 public:
  BLineParContainer();
  ~BLineParContainer();

};

inline BLineParContainer::BLineParContainer():
  DataVector<BLinePar>()
{ } 

inline BLineParContainer::~BLineParContainer()
{ } 

CLASS_DEF( BLineParContainer , 1124991367 , 1 )

#endif  // MUONALIGNMENTDATA_BLINEPARCONTAINER_H
