/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOHELPERVISITOR_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOHELPERVISITOR_H
#include <string>

class TrigJetHypoToolHelperMT;
class AndHelperTool;
class OrHelperTool;
class NotHelperTool;
class CombinationsHelperTool;

class ITrigJetHypoHelperVisitor{
    
public:
  virtual void visit(const TrigJetHypoToolHelperMT*, const std::string&) = 0;
  virtual void visit(const AndHelperTool*, const std::string&) = 0;
  virtual void visit(const OrHelperTool*, const std::string&) = 0;
  virtual void visit(const NotHelperTool*, const std::string&) = 0;
  virtual void visit(const CombinationsHelperTool*, const std::string&) = 0;
};
#endif
