/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICONDITIONVISITOR_H
#define TRIGHLTJETHYPO_ICONDITIONVISITOR_H

#include <string>

class EtaEtConditionMT;
class EtaEtAsymmetricConditionMT;
class DijetConditionMT;
class HTConditionMT;
class TLAConditionMT;

class IConditionVisitor{
    
public:
  virtual ~IConditionVisitor() = default;
  virtual void visit(const EtaEtConditionMT*, const std::string&) = 0;
  virtual void visit(const EtaEtAsymmetricConditionMT*, const std::string&) = 0;
  virtual void visit(const DijetConditionMT*, const std::string&) = 0;
  virtual void visit(const HTConditionMT*, const std::string&) = 0;
  virtual void visit(const TLAConditionMT*, const std::string&) = 0;
  virtual std::string toString() const = 0;
};
#endif
