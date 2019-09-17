// This file's extension implies that it's C, but it's really -*- C++ -*-.
/**
 * @file DebugTutorial/src/TestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief debugging tutorial
 */


#ifndef DEBUGTUTORIAL_TESTALG_H
#define DEBUGTUTORIAL_TESTALG_H


#include "xAODJet/JetContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "TH1.h"
#include <vector>
#include <memory>


namespace Tutorial {


class TestAlg
  : public AthAlgorithm
{
public:
  /// Use base class constructor.
  using AthAlgorithm::AthAlgorithm;


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize() override final;


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize() override final;


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute() override final;


private:
  /// Handle used to access jets from the current event.
  SG::ReadHandleKey<xAOD::JetContainer> m_jetsKey
  { this, "Jets", "", "" };

  /// Adjacent entries contain the (i,j) indices of jets to combine.
  std::vector<int> m_pairtable;

  /// Number of pairs for each jet multiplicity.
  std::vector<int> m_end;

  /// Output histogram.
  std::unique_ptr<TH1> m_hist
  { new TH1F ("hist", "hist", 20, 0, 200) };
};


} // namespace Tutorial


#endif // not DEBUGTUTORIAL_TESTALG_H

