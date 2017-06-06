/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEJETTRIMMER_H
#define LARGEJETTRIMMER_H

#include "xAODJet/JetContainer.h"
#include "TopJetSubstructure/TopJetSubstructure.h"

namespace top {

/**
 * @brief JetCorrection-derived class that trims large-R jets while derivations are not ready.
 */

class LargeJetTrimmer : public TopJetSubstructure {
public:
    LargeJetTrimmer();
    ~LargeJetTrimmer();

    void correctJet(xAOD::Jet& /*jet*/);

    ///Useful messages on the screen.
    void print(std::ostream&) const;

private:
};

}

#endif

