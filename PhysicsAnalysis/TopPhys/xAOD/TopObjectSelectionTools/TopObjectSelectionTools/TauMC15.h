/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPOBJECTSELECTIONTOOLS_TAUMC15_H_
#define TOPOBJECTSELECTIONTOOLS_TAUMC15_H_

#include <memory>

#include "TopObjectSelectionTools/TauSelectionBase.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/Enums.h"
#include "TopEvent/EventTools.h"

namespace top {

/**
 * @brief To select taus based on BoostedDecisionTree definition.
 */
class TauMC15 : public top::TauSelectionBase {
 public:
  TauMC15();

  virtual ~TauMC15() {}
  /**
   * @brief Selection for the main analysis (i.e. tight object definitions).
   *
   * @param tauJet The tau in question
   * @return True if it passes the selection, false otherwise.
   */
  bool passSelection(const xAOD::TauJet& tau) const override;

  /**
   * @brief Selection for the "loose" object definitions (e.g. in fakes
   * estimates).
   *
   * @param tauJet The tau in question
   * @return True if it passes the selection, false otherwise.
   */
  bool passSelectionLoose(const xAOD::TauJet& tau) const override;

  /**
   * @brief Why would you not want to know what you're running?
   *
   * @param os Print details of the cuts to the output stream.
   */
  void print(std::ostream& os) const override;


 protected:
  // The selection tool to use for the tight (main) analysis.
  ToolHandle<TauAnalysisTools::TauSelectionTool> m_tauSelection;

  // The selection tool to use for the loose (e.g. fakes) object selection.
  ToolHandle<TauAnalysisTools::TauSelectionTool> m_tauSelectionLoose;
};

}  // namespace top

#endif  // TOPOBJECTSELECTIONTOOLS_TAUMC15_H_
