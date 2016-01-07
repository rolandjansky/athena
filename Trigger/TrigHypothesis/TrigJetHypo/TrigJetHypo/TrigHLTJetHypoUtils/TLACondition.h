/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_TLACONDITION_H
#define TRIGJETHYPO_TLACONDITION_H
/********************************************************************
 *
 * NAME:     TLACondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "xAODJet/Jet.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ICondition.h"


class TLACondition: public ICondition{
 public:
  TLACondition(double etaMin,
	       double etaMax,
	       double ystarMin,
	       double ystarMax,
	       double massMin,
	       double massMax);

  ~TLACondition() override {}

  bool isSatisfied(const std::vector<const xAOD::IParticle*>&) const override;

  bool isSatisfied(const xAOD::IParticle*) const override;

  double orderingParameter() const noexcept override;

  std::string toString() const noexcept override;

 private:
  double m_etaMin;
  double m_etaMax;
  double m_ystarMin;
  double m_ystarMax;
  double m_massMin;
  double m_massMax;
};

#endif
