/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_MDTASBUILTPARCONTAINER_H
#define MUONALIGNMENTDATA_MDTASBUILTPARCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonAlignmentData/MdtAsBuiltPar.h"

class MdtAsBuiltParContainer : public DataVector<MdtAsBuiltPar> {
public:
    MdtAsBuiltParContainer();
    ~MdtAsBuiltParContainer();
};

inline MdtAsBuiltParContainer::MdtAsBuiltParContainer() : DataVector<MdtAsBuiltPar>() {}

inline MdtAsBuiltParContainer::~MdtAsBuiltParContainer() {}

CLASS_DEF(MdtAsBuiltParContainer, 1205288943, 1)

#endif  // MUONALIGNMENTDATA_MDTASBUILTPARPARCONTAINER_H
