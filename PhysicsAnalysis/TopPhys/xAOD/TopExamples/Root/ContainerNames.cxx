/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/ContainerNames.h"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const top::ContainerNames& names) {
    os << "EventInfo           " << names.eventInfoName << "\n";
    os << "ElectronCollection: " << names.electronCollectionName << "\n";
    os << "MuonCollection:     " << names.muonCollectionName << "\n";
    os << "JetCollection:      " << names.jetCollectionName << "\n";
    os << "LargeJetCollection: " << names.largeJetCollectionName << "\n";
    os << "MET:                " << names.metName << "\n";
    os << "TauCollection:      " << names.tauCollectionName << "\n";
    os << "TruthCollection:    " << names.truthCollectionName << "\n";

    return os;
}
