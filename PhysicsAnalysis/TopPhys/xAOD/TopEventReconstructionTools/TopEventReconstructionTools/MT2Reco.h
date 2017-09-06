/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MT2RECO_H_
#define MT2RECO_H_

#ifdef ROOTCORE_PACKAGE_Oxbridgekinetics

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {
class Event;

/**
 * @brief Not complete. Work in progress
 */
class MT2Reco : public EventSelectorBase {
public:
    MT2Reco();
    virtual ~MT2Reco();

    virtual bool apply(const top::Event&) const override;

    std::string name() const { return "RECO:MT2"; }
};

}

#endif //ROOTCORE_PACKAGE_Oxbridgekinetics

#endif
