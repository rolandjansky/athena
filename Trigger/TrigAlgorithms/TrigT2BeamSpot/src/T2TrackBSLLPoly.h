/*
Copyright (C) 2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT2BEAMSPOT_T2TRACKBSLLPOLY_H
#define TRIGT2BEAMSPOT_T2TRACKBSLLPOLY_H

#include <vector>

namespace PESA {

/**
 * Class that knows details of LogLikelihood approximation with
 * a polynomial.
 *
 * Few particular things that this class handles:
 * - calculation of the index of monomial (polynomial term) in the monitored
 *   array and the resulting histogram bin, the index is determined by the
 *   combination of powers of the variables.
 * - calculation of the coefficients of the polynomial from track parameters.
 *
 * The approximation of the LL function looks like (factorized):
 *
 *  LL = - (d_0 + B_x*sin(phi) - B_y*cos(phi) + t_x*z_0*sin(phi)
 *          - t_y*z_0*cos(phi))**2/(2*Sigma) * (1-S_prime/Sigma + (S_prime/Sigma)**2)
 *       - (ln(Sigma) + S_prime/Sigma - (S_prime/Sigma)**2/2)/2
 *  var_b = beam_size**2
 *  Sigma = var_d0 + var_b
 *  S_prime = omega_x*sin(phi)**2 + omega_y*cos(phi)**2
 *
 * The variables here are (B_x, B_y, t_x, t_y, omega_x, omega_y), the rest
 * are either track parameters or constants. `beam_size` is a constant which
 * should be close to actual transverse beam size (typically 0.01 mm).
 * In the expanded representation the polynomial terms have these possible
 * combinations of powers of omega:
 *
 *    omega_x**2, omega_y**2, omega_x*omega_y, omega_x, omega_y, 1
 *
 * with total of 6 variants and they don't depend on other variables.
 * Four other variables can appear in the polynomial terms in these
 * combinations:
 *
 *    - squared, by itself, without any of other three variables (4)
 *    - product with one other variable (4*3/2)
 *    - by itself (4)
 *    - zero power of any variable (1)
 *
 * Together with 6 omega combinations this gives (4+4*3/2+4+1)*6 = 90
 * monomials.
 *
 * In addition to the 90 coeeficients we also store other information in the
 * monitored variables:
 *
 *    - number of tracks
 *    - number of tracks multiplied by `beam_size`
 */
class T2TrackBSLLPoly {
public:

    explicit T2TrackBSLLPoly(double beam_size) : m_beam_size(beam_size) {}

    /**
     * Return number of bins in the histogram for all plynomial coefficients
     * (and few other numbers).
     */
    static unsigned nbins();

    /**
     * Return bin number (0-based) for a monomial coefficient given the powers
     * of the variables in a monomial. Returns negative number for unexpected
     * input.
     */
    static int idx(unsigned power_Bx, unsigned power_By,
                   unsigned power_tx, unsigned power_ty,
                   unsigned power_omegax, unsigned power_omegay);

    /**
     * Update polynomial coefficients with track data.
     *
     * If vector is empty it is resized to have `nbins()` size, otherwise it
     * has to be at least `nbins()` elements in size.
     */
    void update(double z0, double d0, double phi0, double d0_var, std::vector<double>& coeff);

private:

    double m_beam_size;
};

} // end namespace

#endif // TRIGT2BEAMSPOT_T2TRACKBSLLPOLY_H
