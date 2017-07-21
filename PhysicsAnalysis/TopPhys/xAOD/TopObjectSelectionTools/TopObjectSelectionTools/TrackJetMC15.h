/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKJETMC15_H_
#define TRACKJETMC15_H_

#include "TopObjectSelectionTools/JetSelectionBase.h"

namespace top {
  class TrackJetMC15 : public top::JetSelectionBase {
    public:
      TrackJetMC15(const double ptcut,const double etamax);
      ~TrackJetMC15();
      virtual bool passSelection(const xAOD::Jet& jet) override;
      virtual void print(std::ostream& os) const override;
    private:
      double m_ptcut;
      double m_etamax;


  };
}
#endif
