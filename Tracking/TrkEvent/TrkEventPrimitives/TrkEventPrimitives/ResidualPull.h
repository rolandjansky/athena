/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
//  Header file for class ResidualPull
///////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////////////////
// Class for residual and pull information
///////////////////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////////////////

#ifndef TRK_RESIDUALPULL_H
#define TRK_RESIDUALPULL_H

#include <vector>

namespace Trk {
/** @brief This class containes residual and pull information.

    Beside the values of the residual and pull the parameterKey belonging
    to the measurement and information about the residual type are stored
    in this class. One should distinguish (at least) four types of residuals:
    - the residual between reconstructed track state and MC truth: @f$ r_i = x_i
   - {x_{truth}}_i @f$ with the pull: @f$ p_i = \frac{x_i -
   {x_{truth}}_i}{\sigma(x_i)} @f$
    - biased residual belonging to a track state, which includes the considered
   measurement: @f$ r_i = m_i - x_i @f$ with the related pull: @f$ p_i =
   \frac{m_i - x_i}{\sqrt{\sigma(m_i)^2 - \sigma(x_i)^2}} @f$
    - alignment residuals belonging to a track state, which includes
      the considered measurement: @f$ r_i = m_i - x_i @f$
      with the related pull: @f$ p_i = \frac{m_i - x_i}{\sqrt{\sigma(m_i)^2}}
   @f$
    - unbiased residual belonging to a track state, which does not include the
   measurement: @f$ r_i = m_i - x_i^* @f$ with its pull: @f$ p_i = \frac{m_i -
   x_i^*}{\sqrt{\sigma(m_i)^2 + \sigma(x_i^*)^2}} @f$

    The ResidualType enum reflects the above distinction.

    @author Sebastian.Fleischmann@cern.ch
*/

class ResidualPull
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:
  enum ResidualType
  {
    McTruth = 0,  //!< RP between rec and true track intersection
    Biased = 1,   //!< RP with track state including the hit
    HitOnly = 2,  //!< RP with biased track state, but pull from hit only
    Unbiased = 3, //!< RP with track state that has measurement not included
    NumberOfResidualTypes = 4
  };

  /**  default constructor */
  ResidualPull() = default;
  /**  explicit constructor */
  ResidualPull(const std::vector<double>& residual,
               const std::vector<double>& pull,
               bool pullValid,
               ResidualType residualType,
               int parameterKey,
               double sineStripAngle = 0.0);

  ///////////////////////////////////////////////////////////////////
  // Main methods
  ///////////////////////////////////////////////////////////////////

  //!  return residual vector
  const std::vector<double>& residual() const;

  //!  return pull vector
  const std::vector<double>& pull() const;

  /**  @brief returns false if the pull was calculated without a
       valid covariance matrix
       for the track parameters, this means the pull is not valid! */
  bool isPullValid() const;

  /** @brief  return the parameter key of the Trk::LocalParameters
      (of the measurement) used for the calculation */
  int parameterKey() const;

  //!  return the dimension of the residual and pull
  int dimension() const;

  //!  returns residual type (truth/biased/unbiased etc)
  ResidualType residualType() const;

  /** @brief returns strip angle wrt. local frame for fan-shaped
      strip sensors, 0.0 for others */
  double sineStripAngle() const;

protected:
  ///////////////////////////////////////////////////////////////////
  // Protected Data
  ///////////////////////////////////////////////////////////////////
  std::vector<double> m_residual;
  std::vector<double> m_pull;
  bool m_pullValid = false;
  ResidualType m_residualType = Trk::ResidualPull::Biased;
  int m_parameterKey = 0;
  double m_sineStripAngle = 0.0;
};

/////////////////////////////////////////////////////////////////////////////////
// Inline methods
/////////////////////////////////////////////////////////////////////////////////

} // end of name space
#include "TrkEventPrimitives/ResidualPull.icc"
#endif // TRK_RESIDUALPULL_H
