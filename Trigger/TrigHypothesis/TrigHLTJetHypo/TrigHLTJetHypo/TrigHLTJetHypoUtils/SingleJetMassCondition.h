/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SINGLEJETMASSCONDITION_H
#define TRIGHLTJETHYPO_SINGLEJETMASSCONDITION_H

/********************************************************************
 *
 * NAME:     SingleJetMassCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include <string>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"


class SingleJetMassCondition: public EtaEtCondition{
 public:
  SingleJetMassCondition(double etaMin,
                         double etaMax,
                         double threshold,
                         double massMin,
                         double massMax,
                         bool debug);
  virtual ~SingleJetMassCondition() override {}

  virtual bool isSatisfied(const HypoJetVector&) const override;
  virtual bool isSatisfied(const pHypoJet&) const;

  virtual double orderingParameter() const noexcept override;

  virtual std::string toString() const noexcept override;

 private:

  double m_massMin;
  double m_massMax;
};

#endif
