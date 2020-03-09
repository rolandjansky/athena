// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVKalVrtFitter/src/TrkVKalVrtFitterTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Algorithm for testing TrkVKalVrtFitter.
 */


#ifndef TRKVKALVRTFITTER_TRKVKALVRTFITTERTESTALG_H
#define TRKVKALVRTFITTER_TRKVKALVRTFITTERTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "GaudiKernel/ToolHandle.h"


namespace Trk {


class TrkVKalVrtFitterTestAlg
  : public AthAlgorithm
{
public:
  using AthAlgorithm::AthAlgorithm;


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;

  /// Execute the algorithm.
  virtual StatusCode execute() override;


private:
  StatusCode test1();
  StatusCode test2();
  StatusCode test3();
  StatusCode test4();
  StatusCode test5();
  StatusCode test6();
  StatusCode test7();


  ToolHandle<Trk::IVertexFitter> m_fitter
  { this, "Tool", "Trk::TrkVKalVertexFitter", "Tool to test." };
};


} // namespace Trk


#endif // not TRKVKALVRTFITTER_TRKVKALVRTFITTERTESTALG_H
