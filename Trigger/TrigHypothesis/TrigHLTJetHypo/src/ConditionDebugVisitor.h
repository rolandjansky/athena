/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CONDITIONDEBUGVISITOR_H
#define TRIGHLTJETHYPO_CONDITIONDEBUGVISITOR_H

#include <map>
#include <string>
#include "./IConditionVisitor.h"

class EtaEtConditionMT;
class EtaEtAsymmetricConditionMT;
class DijetConditionMT;
class HTConditionMT;
class TLAConditionMT;

class ConditionDebugVisitor: public IConditionVisitor{
  public:
  virtual void visit(const EtaEtConditionMT*,
                     const std::string&) override;
  
  virtual void visit(const EtaEtAsymmetricConditionMT*,
                     const std::string&) override;
  
  virtual void visit(const DijetConditionMT*,
                     const std::string&) override;
  
  virtual void visit(const HTConditionMT*,
                     const std::string&) override;
  
  virtual void visit(const TLAConditionMT*,
                     const std::string&) override;

  virtual std::string toString() const override;

 private:
  std::map<std::string, std::string> m_debugStrings;
};

#endif
