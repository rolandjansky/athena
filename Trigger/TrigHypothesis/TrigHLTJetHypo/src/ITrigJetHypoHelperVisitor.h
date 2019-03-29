/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOHELPERVISITOR_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOHELPERVISITOR_H

class TrigJetHypoToolHelperMT;
class AndHelperTool;
class OrHelperTool;
class NotHelperTool;
class CombinationsHelperTool;

class ITrigJetHypoHelperVisitor{
    
public:
  virtual void visit(TrigJetHypoToolHelperMT*) = 0;
  virtual void visit(AndHelperTool*) = 0;
  virtual void visit(OrHelperTool*) = 0;
  virtual void visit(NotHelperTool*) = 0;
  virtual void visit(CombinationsHelperTool*) = 0;
};
#endif
