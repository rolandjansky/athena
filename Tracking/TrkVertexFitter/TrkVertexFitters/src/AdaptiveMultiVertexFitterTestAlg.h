// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitters/AdaptiveMultiVertexFitterTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Algorithm for testing AdaptiveMultiVertexFitter.
 */


#ifndef TRKVERTEXFITTERS_ADAPTIVEMULTIVERTEXFITTERTESTALG_H
#define TRKVERTEXFITTERS_ADAPTIVEMULTIVERTEXFITTERTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
#include "GaudiKernel/ToolHandle.h"



namespace Trk {


class AdaptiveMultiVertexFitterTestAlg
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


  ToolHandle<Trk::AdaptiveMultiVertexFitter> m_fitter
  { this, "Tool", "Trk::AdaptiveMultiVertexFitter", "Tool to test." };
};


} // namespace Trk


#endif // not TRKVERTEXFITTERS_ADAPTIVEMULTIVERTEXFITTERTESTALG_H
