/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_CSCLINEPARCONTAINER_H
#define MUONALIGNMENTDATA_CSCLINEPARCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"

class CscInternalAlignmentParContainer : public DataVector<CscInternalAlignmentPar> {
public:
    CscInternalAlignmentParContainer();
    ~CscInternalAlignmentParContainer() = default;
};

inline CscInternalAlignmentParContainer::CscInternalAlignmentParContainer() : DataVector<CscInternalAlignmentPar>() {}

CLASS_DEF(CscInternalAlignmentParContainer, 1324994858, 1)

#endif  // MUONALIGNMENTDATA_CSCLINEPARCONTAINER_H
