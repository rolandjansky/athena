/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ICscClusterFitter.cxx
#include "CscClusterization/ICscClusterFitter.h"

typedef ICscClusterFitter::DataNames DataNames;

// const InterfaceID& ICscClusterFitter::interfaceID() {
//  return IID_ICscClusterFitter;
//}

const DataNames& ICscClusterFitter::dataNames() const {
    static const DataNames dnames{};
    return dnames;
}

