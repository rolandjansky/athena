/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ========================================================================================
 * Drift Time Correction Function for Wire Sag Effects
 * ---------------------------------------------------
 * ========================================================================================
 */

#ifndef MDTCALIBDATA_WIRESAGCORFUNC_H
#define MDTCALIBDATA_WIRESAGCORFUNC_H

/// Includes

#include "MdtCalibData/IMdtWireSagCorFunc.h"

/**
 * ========================================================================================
 *
 *  class WireSagCorFunc
 * 
 *  Description
 *  -----------
 *  This class calculates the correction to drift time due to the asymmetry of the
 *  electric field caused by wire sag.  It takes in as parameters the effective radius
 *  and wire sag and returns the corresponding change in the drift time do to this sag.
 *
 *  Notes:
 *    1) The coefficients of this correction function were obtained from the
 *       garfield-simulations calibration procedure WireSagDTR.C.
 *    2) The function is a quintic polynomial.
 *    3) The function was normalized to an effective wire sag of 100um, but because
 *       of the linearity in dT with wire sag, we can simply scale it by a factor of
 *       (Sag/100um).
 * 
 *  The Calibration Procedure WireSagDTR.C creates the correction function by the
 *  following procedure:
 *
 *    1) Compute RT functions from garfield simulations for wires sags of 30-500um.
 *    1) Fit the RT functions with Chebyshev polynomial algorithm.
 *    2) Compute all possible dT(R) functions and normalize them to 100um.
 *    3) Average dT(R, 100um), apply 2*sigma cut, reaverage...
 *    4) Fit dT(R,100um) with a quintic polynomial.
 * 
 *  Inheritance
 *  -----------
 *  IMdtWireSagCorFunc
 * 
 *  Authors
 *  -------
 *  Jason Hindes, Dan Levin
 *
 *  Coder
 *  ------
 *  Steven Goldfarb <Steven.Goldfarb@cern.ch>
 * 
 * ========================================================================================
 */

namespace MuonCalib {

class WireSagCorFunc : public IMdtWireSagCorFunc {
  public:

    explicit WireSagCorFunc(const CalibFunc::ParVec & params)
      : IMdtWireSagCorFunc(params) {
      init(params);
    }

    /// class name
    std::string name() const;

    /// correction function
    double correction(double signedDriftRadius, double effectiveSag) const;

  private:
    /// data members
    std::vector<double> m_params; // could eventually be used to pass coefficients

    /// initialization method
    void init(const CalibFunc::ParVec & params);
  };

}  //namespace MuonCalib

#endif // MDTCALIBDATA_WIRESAGCORFUNC_H
