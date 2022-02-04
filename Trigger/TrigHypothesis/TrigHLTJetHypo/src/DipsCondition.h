/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DIPSCONDITION_H
#define TRIGHLTJETHYPO_DIPSCONDITION_H

/********************************************************************
 *
 * NAME:     DipsCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   M. Valente
 *********************************************************************/

#include <string>
#include "./ICondition.h"
#include "AsgTools/AsgTool.h"
#include "xAODJet/JetContainer.h"
namespace HypoJet{
  class IJet;
}

class ITrigJetHypoInfoCollector;

class DipsCondition: public ICondition{
 public:
   DipsCondition(double workingPoint,
                 const float &cfrac = 0.018,
                 const std::string &decName_pb = "fastDips_pb",
                 const std::string &decName_pu = "fastDips_pu",
                 const std::string &decName_pc = "fastDips_pc" );

   float getDipsDecValue(const pHypoJet &ip,
                         const std::unique_ptr<ITrigJetHypoInfoCollector> &collector,
                         const std::string &decName) const;

   float evaluateDips(const float &dips_pb,
                      const float &dips_pc,
                      const float &dips_pu) const;

   bool isSatisfied(const HypoJetVector &,
                    const std::unique_ptr<ITrigJetHypoInfoCollector> &) const override;

   virtual unsigned int capacity() const override { return s_capacity; }

   std::string toString() const override;

 private:
   double      m_workingPoint;
   float       m_cfrac;
   std::string m_decName_pb;
   std::string m_decName_pu;
   std::string m_decName_pc;

   bool isSatisfied(const pHypoJet &,
                    const std::unique_ptr<ITrigJetHypoInfoCollector> &) const;

   const static unsigned int s_capacity{1};

};

#endif
