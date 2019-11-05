/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SOURCE_HH_TAURECTOOLS
#define SOURCE_HH_TAURECTOOLS

#include <Eigen/Dense>

#include <vector>

namespace lwtDev {
  using Eigen::VectorXd;
  using Eigen::MatrixXd;

  // this is called by input nodes to get the inputs
  class ISource
  {
  public:
    virtual ~ISource() = default;
    virtual VectorXd at(size_t index) const = 0;
    virtual MatrixXd matrix_at(size_t index) const = 0;
  };

  class VectorSource: public ISource
  {
  public:
    VectorSource(std::vector<VectorXd>&&, std::vector<MatrixXd>&& = {});
    virtual VectorXd at(size_t index) const override;
    virtual MatrixXd matrix_at(size_t index) const override;
  private:
    std::vector<VectorXd> m_inputs;
    std::vector<MatrixXd> m_matrix_inputs;
  };

  class DummySource: public ISource
  {
  public:
    DummySource(const std::vector<size_t>& input_sizes,
                const std::vector<std::pair<size_t, size_t> >& = {});
    virtual VectorXd at(size_t index) const override;
    virtual MatrixXd matrix_at(size_t index) const override;
  private:
    std::vector<size_t> m_sizes;
    std::vector<std::pair<size_t, size_t> > m_matrix_sizes;
  };
}

#endif //SOURCE_HH_TAURECTOOLS
