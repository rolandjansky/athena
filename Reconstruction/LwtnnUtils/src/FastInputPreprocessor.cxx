// this is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Any modifications to this file may be copied to lwtnn[1] without
// attribution.
//
// [1]: https::www.github.com/lwtnn/lwtnn

#include "LwtnnUtils/FastInputPreprocessor.h"
#include "lwtnn/Exceptions.hh"

namespace {
  // utility functions
  //
  // Build a mapping from the inputs in the saved network to the
  // inputs that the user is going to hand us.
  std::vector<size_t> get_value_indices(
    const std::vector<std::string>& order,
    const std::vector<lwt::Input>& inputs)
  {
    std::map<std::string, size_t> order_indices;
    for (size_t i = 0; i < order.size(); i++) {
      order_indices[order.at(i)] = i;
    }
    std::vector<size_t> value_indices;
    for (const lwt::Input& input: inputs) {
      if (!order_indices.count(input.name)) {
        throw lwt::NNConfigurationException("Missing input " + input.name);
      }
      value_indices.push_back(order_indices.at(input.name));
    }
    return value_indices;
  }

}

namespace lwt::atlas {
  // ______________________________________________________________________
  // FastInput preprocessors

  // simple feed-forwared version
  FastInputPreprocessor::FastInputPreprocessor(
    const std::vector<Input>& inputs,
    const std::vector<std::string>& order):
    m_offsets(inputs.size()),
    m_scales(inputs.size())
  {
    size_t in_num = 0;
    for (const auto& input: inputs) {
      m_offsets(in_num) = input.offset;
      m_scales(in_num) = input.scale;
      in_num++;
    }
    m_indices = get_value_indices(order, inputs);
  }
  VectorXd FastInputPreprocessor::operator()(const VectorXd& in) const {
    VectorXd invec(m_indices.size());
    size_t input_number = 0;
    for (size_t index: m_indices) {
      if (static_cast<int>(index) >= in.rows()) {
        throw NNEvaluationException(
          "index " + std::to_string(index) + " is out of range, scalar "
          "input only has " + std::to_string(in.rows()) + " entries");
      }
      invec(input_number) = in(index);
      input_number++;
    }
    return (invec + m_offsets).cwiseProduct(m_scales);
  }


  // Input vector preprocessor
  FastInputVectorPreprocessor::FastInputVectorPreprocessor(
    const std::vector<Input>& inputs,
    const std::vector<std::string>& order):
    m_offsets(inputs.size()),
    m_scales(inputs.size())
  {
    size_t in_num = 0;
    for (const auto& input: inputs) {
      m_offsets(in_num) = input.offset;
      m_scales(in_num) = input.scale;
      in_num++;
    }
    // require at least one input at configuration, since we require
    // at least one for evaluation
    if (in_num == 0) {
      throw NNConfigurationException("need at least one input");
    }
    m_indices = get_value_indices(order, inputs);
  }
  MatrixXd FastInputVectorPreprocessor::operator()(const MatrixXd& in) const {
    using namespace Eigen;
    size_t n_cols = in.cols();
    MatrixXd inmat(m_indices.size(), n_cols);
    size_t in_num = 0;
    for (size_t index: m_indices) {
      if (static_cast<int>(index) >= in.rows()) {
        throw NNEvaluationException(
          "index " + std::to_string(index) + " is out of range, sequence "
          "input only has " + std::to_string(in.rows()) + " entries");
      }
      inmat.row(in_num) = in.row(index);
      in_num++;
    }
    if (n_cols == 0) {
        return MatrixXd(m_indices.size(), 0);
    }
    return m_scales.asDiagonal() * (inmat.colwise() + m_offsets);
  }

}
