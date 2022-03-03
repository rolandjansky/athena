/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPOBJECTSELECTIONTOOLS_TRACKJET_H_
#define TOPOBJECTSELECTIONTOOLS_TRACKJET_H_

#include "TopObjectSelectionTools/JetSelectionBase.h"

namespace top {
  class TrackJet: public top::JetSelectionBase {
  public:
    TrackJet(const double ptcut, const double etamax);
    ~TrackJet();
    virtual bool passSelection(const xAOD::Jet& jet) override;
    virtual void print(std::ostream& os) const override;
  private:
    double m_ptcut;
    double m_etamax;
  };
}
#endif
