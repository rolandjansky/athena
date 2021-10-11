/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TLACONDITION_H
#define TRIGHLTJETHYPO_TLACONDITION_H

/********************************************************************
 *
 * NAME:     TLACondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ICondition.h"

class ITrigJetHypoInfoCollector;


class TLACondition: public ICondition{
 public:
  TLACondition(const std::vector<double>& etaMins,
		 const std::vector<double>& etaMaxs,
		 const std::vector<double>& ystarMins,
		 const std::vector<double>& ystarMaxs,
		 const std::vector<double>& massMins,
		 const std::vector<double>& massMaxs);

  TLACondition(double etaMin,
               double etaMax,
               double ystarMin,
               double ystarMax,
               double massMin,
               double massMax);
  
  ~TLACondition() override {}

  bool isSatisfied(const HypoJetVector&,
                   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  std::string toString() const override;

  virtual unsigned int capacity() const override {return s_capacity;}

 private:
  double m_etaMin;
  double m_etaMax;
  double m_ystarMin;
  double m_ystarMax;
  double m_massMin;
  double m_massMax;

  const static unsigned int s_capacity{1};

};

#endif
