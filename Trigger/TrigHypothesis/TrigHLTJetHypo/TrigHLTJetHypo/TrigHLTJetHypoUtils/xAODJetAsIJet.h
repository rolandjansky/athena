/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_XAODJETASIJET_H
#define TRIGHLTJETHYPO_XAODJETASIJET_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/JetBase.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include  "xAODJet/Jet.h"

class TLorentzVector;

namespace HypoJet{
  class xAODJetAsIJet: virtual public IJet, private JetBase{
  public:
    xAODJetAsIJet(const xAOD::Jet* jet, unsigned int position);
    ~xAODJetAsIJet();
    double pt() const override;
    double eta() const override;
    double phi() const override;
    double m() const override;
    double e() const override;
    double et() const override;
    double rapidity() const override;
    TLorentzVector p4() const override;
    unsigned int position() const override;


    virtual std::optional<const xAOD::Jet*> xAODJet() const override;
    bool getAttribute(const std::string&, float&) const override;
    bool getAttribute(const std::string&, bool&) const override;

  private:
    const xAOD::Jet* m_jet;
    unsigned int m_position;
  };
}
#endif
