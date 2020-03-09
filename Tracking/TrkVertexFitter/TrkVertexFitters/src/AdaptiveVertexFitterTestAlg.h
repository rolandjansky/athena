// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitters/src/AdaptiveVertexFitterTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Algorithm for testing AdaptiveVertexFitter.
 */


#ifndef TRKVERTEXFITTERS_ADAPTIVEVERTEXFITTERTESTALG_H
#define TRKVERTEXFITTERS_ADAPTIVEVERTEXFITTERTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "GaudiKernel/ToolHandle.h"



namespace Trk {


class AdaptiveVertexFitterTestAlg
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


  ToolHandle<Trk::IVertexFitter> m_fitter
  { this, "Tool", "Trk::AdaptiveVertexFitter", "Tool to test." };
};


} // namespace Trk


#endif // not TRKVERTEXFITTERS_ADAPTIVEVERTEXFITTERTESTALG_H
