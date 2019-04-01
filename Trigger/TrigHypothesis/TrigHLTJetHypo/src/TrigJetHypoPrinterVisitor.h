/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETHYPOPRINTERVISITOR_H
#define TRIGHLTJETHYPO_JETHYPOPRINTERVISITOR_H

#include "./ITrigJetHypoHelperVisitor.h"

#include "./TrigJetHypoToolHelperMT.h"
#include "./AndHelperTool.h"
#include "./OrHelperTool.h"
#include "./NotHelperTool.h"
#include "./CombinationsHelperTool.h"

class TrigJetHypoPrinterVisitor: public ITrigJetHypoHelperVisitor {
    
public:
  virtual void visit(TrigJetHypoToolHelperMT*) override;
  virtual void visit(AndHelperTool*) override;
  virtual void visit(OrHelperTool*) override;
  virtual void visit(NotHelperTool*) override;
  virtual void visit(CombinationsHelperTool*) override;

  std::string toString() const noexcept;
  void write(std::string, uint32_t run, uint32_t event);
  void write(std::string, std::size_t);

 private:
  std::vector<std::pair<std::string, std::string>> m_nodeStrings;
  std::size_t m_eventSN{0};
};
#endif
