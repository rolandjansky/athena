/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PTMAXRECO_H_
#define PTMAXRECO_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

#include "TLorentzVector.h"

namespace top {
class Event;

/**
 * @brief Not complete. Work in progress
 */
class PTMaxReco : public EventSelectorBase {
public:
    PTMaxReco();
    virtual ~PTMaxReco();

    bool apply(const top::Event&) const override;

    std::string name() const { return "RECO:PTMAX"; }

private:
    TLorentzVector neutrinoCandidate(const TLorentzVector& lep, const xAOD::MissingET& met, bool dealWithNegative_nu) const;

    double m_wmass;
    double m_topmass;
};

}

#endif
