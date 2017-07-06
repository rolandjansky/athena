/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPJETSUBSTRUCTURE_H
#define TOPJETSUBSTRUCTURE_H

#include "xAODJet/JetContainer.h"

namespace top {

/**
 * @brief JetCorrection-derived class that applies a jet substructure action on the object
 */

class TopJetSubstructure {
public:
    TopJetSubstructure();
    ~TopJetSubstructure();

    virtual void correctJet(xAOD::Jet& /*jet*/) = 0;

    ///Useful messages on the screen.
    void print(std::ostream&) const;

private:
};

}

#endif

