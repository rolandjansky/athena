/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TLACONDITIONMT_H
#define TRIGHLTJETHYPO_TLACONDITIONMT_H

/********************************************************************
 *
 * NAME:     TLAConditionMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./IConditionMT.h"

class IConditionVisitor;


class TLAConditionMT: public IConditionMT{
 public:
  TLAConditionMT(const std::vector<double>& etaMins,
               const std::vector<double>& etaMaxs,
               const std::vector<double>& ystarMins,
               const std::vector<double>& ystarMaxs,
               const std::vector<double>& massMins,
               const std::vector<double>& massMaxs);

  TLAConditionMT(double etaMin,
               double etaMax,
               double ystarMin,
               double ystarMax,
               double massMin,
               double massMax);
  
  ~TLAConditionMT() override {}

  bool isSatisfied(const HypoJetVector&,
                   std::unique_ptr<IConditionVisitor>&) const override;
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
