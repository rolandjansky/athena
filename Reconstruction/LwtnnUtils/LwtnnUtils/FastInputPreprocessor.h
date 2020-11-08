#ifndef FAST_INPUT_PREPROCESSOR_HH
#define FAST_INPUT_PREPROCESSOR_HH

#include "lwtnn/lightweight_network_config.hh"
#include "lwtnn/Exceptions.hh"

#include <Eigen/Dense>
#include <map>
#include <vector>

namespace lwt::atlas {

  using Eigen::VectorXd;
  using Eigen::MatrixXd;

  // use a normal map externally, since these are more common in user
  // code.  TODO: is it worth changing to unordered_map?
  typedef std::map<std::string, double> ValueMap;
  typedef std::vector<std::pair<std::string, double> > ValueVector;
  typedef std::map<std::string, std::vector<double> > VectorMap;

  // ______________________________________________________________________
  // input preprocessor (handles normalization and packing into Eigen)

  class FastInputPreprocessor
  {
  public:
    FastInputPreprocessor(const std::vector<Input>& inputs);
    VectorXd operator()(const ValueMap&) const;
  private:
    // input transformations
    VectorXd m_offsets;
    VectorXd m_scales;
    std::vector<std::string> m_names;
  };

  class FastInputVectorPreprocessor
  {
  public:
    FastInputVectorPreprocessor(const std::vector<Input>& inputs);
    MatrixXd operator()(const VectorMap&) const;
  private:
    // input transformations
    VectorXd m_offsets;
    VectorXd m_scales;
    std::vector<std::string> m_names;
  };
}


#endif
