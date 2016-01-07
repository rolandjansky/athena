/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ETAETCONDITION_H
#define TRIGJETHYPO_ETAETCONDITION_H

/********************************************************************
 *
 * NAME:     TrigHLTJetHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "xAODJet/Jet.h"
#include <string>
#include "TrigJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class EtaEtCondition: public ICondition{
 public:
  EtaEtCondition(double etaMin, double etaMax, double threshold);
  ~EtaEtCondition() override {}

  bool isSatisfied(const std::vector<const xAOD::IParticle*>&) const override;

  bool isSatisfied(const xAOD::IParticle*) const override;

  double orderingParameter() const noexcept override;

  std::string toString() const noexcept override;

 private:
  double m_etaMin;
  double m_etaMax;
  double m_threshold;
};

#endif
