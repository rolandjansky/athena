/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_CSCLINEPARCONTAINER_H
#define MUONALIGNMENTDATA_CSCLINEPARCONTAINER_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"

class CscInternalAlignmentParContainer : public DataVector<CscInternalAlignmentPar>
{
 public:
  CscInternalAlignmentParContainer();
  ~CscInternalAlignmentParContainer();

};

inline CscInternalAlignmentParContainer::CscInternalAlignmentParContainer():
  DataVector<CscInternalAlignmentPar>()
{ } 

inline CscInternalAlignmentParContainer::~CscInternalAlignmentParContainer()
{ } 

CLASS_DEF(CscInternalAlignmentParContainer, 1324994858, 1)

#endif  // MUONALIGNMENTDATA_CSCLINEPARCONTAINER_H
