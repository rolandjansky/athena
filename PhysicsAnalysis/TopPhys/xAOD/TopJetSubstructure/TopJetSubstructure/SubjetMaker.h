/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUBJETMAKER_H
#define SUBJETMAKER_H

#include "xAODJet/JetContainer.h"
#include "TopJetSubstructure/TopJetSubstructure.h"

namespace top {

/**
 * @brief Derived class that constructs subjets while derivations are not ready.
 */

class SubjetMaker : public TopJetSubstructure {
public:
    SubjetMaker();
    ~SubjetMaker();

    void correctJet(xAOD::Jet& /*jet*/);

    ///Useful messages on the screen.
    void print(std::ostream&) const;

private:
};

}

#endif

