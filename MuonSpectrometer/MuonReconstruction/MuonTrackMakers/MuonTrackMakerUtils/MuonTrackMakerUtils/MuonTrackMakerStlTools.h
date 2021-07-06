/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMAKERSTLTOOLS_H
#define MUONTRACKMAKERSTLTOOLS_H

namespace Muon {

    template <class T> struct MuonDeleteObject {
        void operator()(T* entry) { delete entry; }
    };

}  // namespace Muon

#endif
