/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/PrintEventSelector.h"

#include "TopEvent/EventTools.h"

#include "TopCorrections/ScaleFactorRetriever.h"

namespace top {

bool PrintEventSelector::apply(const top::Event& event) const {
    //print some stuff about the event
    std::cout << event << std::endl;

    //print the SF if calculated and if MC
//     if (top::isSimulation(event)) {
//         top::ScaleFactorRetriever::print(event);
//         std::cout << std::endl;
//     }

    return true;
}

std::string PrintEventSelector::name() const {
    return "PRINT";
}

}
