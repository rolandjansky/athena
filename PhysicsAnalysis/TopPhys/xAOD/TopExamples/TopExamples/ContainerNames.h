/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONTAINERNAMES_H_
#define CONTAINERNAMES_H_

#include <string>

namespace top {

struct ContainerNames {
    std::string eventInfoName;
    std::string electronCollectionName;
    std::string muonCollectionName;
    std::string jetCollectionName;
    std::string largeJetCollectionName;
    std::string metName;
    std::string tauCollectionName;
    std::string truthCollectionName;
};

}

std::ostream& operator<<(std::ostream& os, const top::ContainerNames& names);

#endif
