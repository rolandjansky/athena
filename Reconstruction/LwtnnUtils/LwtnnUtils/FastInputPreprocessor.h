// this is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Any modifications to this file may be copied to lwtnn[1] without
// attribution.
//
// [1]: https::www.github.com/lwtnn/lwtnn


#ifndef FAST_INPUT_PREPROCESSOR_HH
#define FAST_INPUT_PREPROCESSOR_HH

#include "lwtnn/lightweight_network_config.hh"
#include "lwtnn/Exceptions.hh"

#include <Eigen/Dense>
#include <vector>

namespace lwt::atlas {

  using Eigen::VectorXd;
  using Eigen::MatrixXd;

  // ______________________________________________________________________
  // input preprocessor (handles normalization and packing into Eigen)

  class FastInputPreprocessor
  {
  public:
    FastInputPreprocessor(const std::vector<Input>& inputs,
                          const std::vector<std::string>& order);
    VectorXd operator()(const VectorXd&) const;
  private:
    // input transformations
    VectorXd m_offsets;
    VectorXd m_scales;
    std::vector<size_t> m_indices;
  };

  class FastInputVectorPreprocessor
  {
  public:
    FastInputVectorPreprocessor(const std::vector<Input>& inputs,
                                const std::vector<std::string>& order);
    MatrixXd operator()(const MatrixXd&) const;
  private:
    // input transformations
    VectorXd m_offsets;
    VectorXd m_scales;
    std::vector<size_t> m_indices;
  };
}


#endif
