// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderTools/src/VertexSeedFinderTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Algorithm for doing unit tests of the seed finder tools.
 *        Basic test only, not terribly comprehensive.
 */


#ifndef TRKVERTEXSEEDFINDERTOOLS_VERTEXSEEDFINDERTESTALG_H
#define TRKVERTEXSEEDFINDERTOOLS_VERTEXSEEDFINDERTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "StoreGate/WriteHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"


namespace Trk {


class VertexSeedFinderTestAlg
  : public AthAlgorithm
{
public:
  using AthAlgorithm::AthAlgorithm;


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;

  /// Execute the algorithm.
  virtual StatusCode execute() override;


private:
  /// Make a test McEventCollection.
  StatusCode makeMcEventCollection (const EventContext& ctx) const;

  Gaudi::Property<std::vector<double> > m_priVert
  { this, "PriVert", {}, "Primary vertex 2D position" };
  Gaudi::Property<std::vector<double> > m_expected1
  { this, "Expected1", {}, "First expected result" };
  Gaudi::Property<std::vector<double> > m_expected2
  { this, "Expected2", {}, "Second expected result" };
  Gaudi::Property<std::vector<double> > m_expected3
  { this, "Expected3", {}, "Third expected result" };

  Gaudi::Property<std::vector<float> > m_expected1PhiModes
  { this, "Expected1PhiModes", {}, "" };
  Gaudi::Property<std::vector<float> > m_expected1RModes
  { this, "Expected1RModes", {}, "" };
  Gaudi::Property<std::vector<float> > m_expected1ZModes
  { this, "Expected1ZModes", {}, "" };
  Gaudi::Property<std::vector<float> > m_expected1Weights
  { this, "Expected1Weights", {}, "" };
  Gaudi::Property<std::vector<int> > m_expected1Indices
  { this, "Expected1Indices", {}, "" };
  Gaudi::Property<std::vector<double> > m_expected1CorrDist
  { this, "Expected1CorrDist", {}, "" };
  
  Gaudi::Property<unsigned int> m_npart3
  { this, "NPart3", 1000, "Number of test particles." };

  SG::WriteHandleKey<McEventCollection> m_mcEventCollectionKey
  { this, "McEventCollectionKey", "", "Test MC Collecton" };

  ToolHandle<Trk::IVertexSeedFinder> m_finder
  { this, "VertexSeedFinderTool", "", "Tool to test." };
};


} // namespace Trk


#endif // not TRKVERTEXSEEDFINDERTOOLS_VERTEXSEEDFINDERTESTALG_H
