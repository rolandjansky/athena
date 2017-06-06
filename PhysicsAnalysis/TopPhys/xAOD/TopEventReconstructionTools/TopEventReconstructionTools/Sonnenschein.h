/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SONNENSCHEIN_H_
#define SONNENSCHEIN_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventReconstructionTools/SonnenscheinEngine.h"

#include "TLorentzVector.h"

namespace top {
class Event;

/**
 * @brief An AnalysisTop friendly wrapper for the Sonnenschein reconstruction
 * that is implemented in SonnenscheinEngine.
 */
class Sonnenschein : public EventSelectorBase {
public:
    Sonnenschein();
    virtual ~Sonnenschein();

    virtual bool apply(const top::Event&) const override;

    std::string name() const { return "RECO:SONNENSCHEIN"; }

private:
    bool tWb(const top::Event& event, int start, TLorentzVector& t, TLorentzVector& W, TLorentzVector& b, TLorentzVector& l, TLorentzVector& nu) const;

    top::SonnenscheinEngine m_sonn;
};

}

#endif
