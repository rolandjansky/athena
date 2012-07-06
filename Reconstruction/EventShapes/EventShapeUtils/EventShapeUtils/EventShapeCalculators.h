// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEUTILS_EVENTSHAPECALCULATORS_H
#define EVENTSHAPEUTILS_EVENTSHAPECALCULATORS_H

#include "TF1.h"

#include <vector>

namespace EventShapeCalculators
{
  class ProfileShape
  {
  public:
 
    /*! @brief Default constructor */   
    ProfileShape();
    /*! @brief Constructor with list of parameters
     *
     *  @param[in] pars @c const reference to non-modifiable list of parameters
     */
    ProfileShape(const std::vector<double>& pars);
    /*! @brief Constructor with list parameters and list of errors
     *
     *  @param[in] pars @c const reference to non-modifiable list of parameters
     *  @param[in] errs @c const reference to non-modifiable list of errors
     */
    ProfileShape(const std::vector<double>& pars,
		 const std::vector<double>& errs);
    /*! @brief Copy constructor */
    ProfileShape(const ProfileShape& shape);
    /*! @brief Base class destructor */
    virtual ~ProfileShape();

    /*! @brief Interface supporting @c ROOT
     * 
     *  For compatibility with @c ROOT . Forwards to 
     *  @c ProfileShape::evaluate(xArr[0]) after setting the function 
     *  parameters to the values provided in @c pArr . Useful for fitting
     *  within the @c ROOT environment.
     *
     *  @param[in] xArr pointer to array of variables
     *  @param[in] pArr pointer to array of parameters
     */
    double operator()(double* xArr,double* pArr);

    /*! @brief Access to function values
     *
     *  @return Function value evaluated at @c x
     * 
     *  @param[in] x value at which function is evaluated 
     */
    double evaluate(double x) const;
    /*! @brief Access to function values
     *
     *  Forwards to @c ProfileShape::evaluate(x)
     *  
     *  @return Function value evaluated at  @x 
     *
     *  @param[in] x value at which function is evaluated 
     */
    double operator()(double x) const { return this->evaluate(x); } 

    /*! @brief Returns number of function parameters */
    size_t np();

    /*! @brief Sets parameter value and error in function 
     *
     *  @param[in] index index of parameter in function
     *  @param[in] value parameter value
     *  @param[in] error parameter error
     */
    bool   setParameter(size_t index,double value,double error=0.);

    /*! @brief Retrieves value of parameter of function
     *
     *  @return Value of parameter at index @c idx
     *
     *  @param[in] idx index of parameter 
     */
    double parameter(size_t idx) const;
    /*! @brief Retrieves error associated with parameter of function
     *
     *  @return Error of parameter at index @c idx
     *
     *  @param[in] idx index of parameter
     */
    double error(size_t idx)     const;
    /*! @brief Set values and errors of specific parameter: plateau height
     *
     *  Sets the (relative) transverse energy density in plateau region. 
     *  Typical a reference value like 1.
     *
     *  @param[in] value value to be set
     *  @param[in] error (optional) associated error 
     */
    bool setPlateau(double value,double error=0.);
    /*! @brief Set values and errors of specific parameter: plateau width
     *
     *  Sets the size of the plateau region @f$ \eta_{\mathrm{plateau}} @f$. 
     *  The plateau is given by a symmetric pseudorapidity range 
     *  @f$ \left|\eta\right| < \eta_{\mathrm{plateau}} @f$ 
     *
     *  @param[in] value value to be set
     *  @param[in] error (optional) associated error 
     */
    bool setRange(double value,double error=0.);
    /*! @brief Set values and errors of specific parameter: Gaussian slope 
     *
     *  Sets the slope of the Gaussian shoulders left and right of the 
     *  plateau region. The slope is measured by @f$ \sigma_{\eta} @f$.
     *
     *  @param[in] value value to be set
     *  @param[in] error (optional) associated error 
     */
    bool setShape(double value,double error=0.);
    /*! @brief Set values and errors of specific parameter: forward baseline
     *
     *  Sets the density baseline in the forward direction 
     *  @f$ \rho_{\mathrm{base}} @f$.
     *
     *  @param[in] value value to be set
     *  @param[in] error (optional) associated error 
     */
    bool setBase(double value,double error=0.);

    /*! @brief Retrieve specific parameter value: plateau height */
    double plateau() const;
    /*! @brief Retrieve specific parameter value: symmetric plateau range */
    double range()   const;
    /*! @brief Retrieve specific parameter value: shape of Gaussian shoulder */
    double shape()   const;
    /*! @brief Retrieve specific parameter value: forward baseline */
    double base()    const;

    /*! @brief Retrieve specific parameter error: plateau height */
    double plateauError() const;
    /*! @brief Retrieve specific parameter error: symmetric plateau range */
    double rangeError()   const;
    /*! @brief Retrieve specific parameter error: shape of Gaussian shoulder */
    double shapeError()   const;
    /*! @brief Retrieve specific parameter error: forward baseline  */
    double baseError()    const;

  private:

    /*! @brief Container for parameter values list */
    std::vector<double> m_parameters;
    /*! @brief Container for parameter errors list */
    std::vector<double> m_errors;

    /*! @brief Index of plateau height parameter in list */
    static size_t m_plateauIdx;
    /*! @brief Index of plateau range parameter in list */
    static size_t m_rangeIdx;
    /*! @brief Index of Gaussian shoulder parameter in list */
    static size_t m_shapeIdx;
    /*! @brief Index of forward baseline in list */
    static size_t m_baseIdx;
  };
  /*! @class ProfileShape
   *
   *  @brief Eta profile shape descriptor and calculator. 
   *
   *  This class implements the actual profile calculation. The functional 
   *  form best fitting the 2011 minimum bias data is
   *
   *  @f[
   *   \rho(\eta) = 
   *    \left\{\begin{array}{ll} 
   *     \rho_{\mathrm{event}} & 
   *     \left|\eta\right| < \eta_{\mathrm{plateau}} \\
   *     (\rho_{\mathrm{event}}-\rho_{\mathrm{base}}) 
   *                      \exp((\eta-\mathrm{sgn}(\eta)
   *                      \eta_{\mathrm{plateau}})^{2}/
   *                      \sigma_{\eta}^{2}) + \rho_{\mathrm{base}} &
   *                      \left|\eta\right| > \eta_{\mathrm{plateau}}
   *    \end{array}
   *   \right.
   *  @f]
   *
   */


  class ProfileFunctions
  {
  public:

    /*! @brief Default constructor */
    ProfileFunctions();
    /*! @brief Constructor with a given @f$ \eta @f$ range */
    ProfileFunctions(double etaMin,double etaMax);
    /*! @brief Constructor with @c ProfileShape */
    ProfileFunctions(const ProfileShape* shape);
    /*! @brief Copy constructor */
    ProfileFunctions(const ProfileFunctions& profile);

    /*! @brief Base class destructor */
    virtual ~ProfileFunctions();

    /*! @brief Event transverse energy calculator
     *
     *  Implements the density calculation depending parameters.
     *
     *  @return Event transverse energy density evaluated under certain 
     *          pileup conditions and at a given direction. The returned value
     *          can be relative (@c rhoRef @c = @c 1 ) or absolute 
     *          (@c rhoRef @c = @f$ \rho_{\mathrm{event}} @f$ ). 
     * 
     *  @param[in] npv number of reconstructed primary vertices
     *  @param[in] mu  average number of interactions 
     *  @param[in] eta direction at which density is evaluated
     *  @param[in] rhoRef (optional) reference value for central density
     *                    @f$ \rho_{\mathrm{event}} @f$. Defaults to 1.
     *  @param[in] window (optional) window size for profile calculation,
     *                    defaults to 1.6.
     *               
     */
    double rho(double npv,double mu,double eta,
	       double rhoRef=1.,double window=1.6);
    /*! @brief Event transverse energy calculator
     * 
     *  @return Event transverse energy density evaluated under certain
     *          previously set  
     *          pileup conditions and at a given direction. The returned value
     *          can be relative (@c rhoRef @c = @c 1 ) or absolute 
     *          (@c rhoRef @c = @f$ \rho_{\mathrm{event}} @f$ ). 
     *   
     */
    double rho(double eta,double rhoRef);

    /*! @brief Retrieve @c ROOT function of profile
     * 
     *  @return Pointer to @c ROOT function object of type @c TF1 . The profile
     *          shape depends on pileup conditions and the window size
     *          used in the calculation of the profile.
     * 
     *  @param[in] npv number of reconstructed primary vertices
     *  @param[in] mu  average number of interactions 
     *  @param[in] window (optional) window size for profile calculation,
     *                    defaults to 1.6.
     *  @param[in] name (optional) name of function object, default is
     *                  @c SlidingWindow .
     *
     *  @note The @c TF1 function object pointed to is not owned by the
     *        @c ProfileFunctions instance. The client is responsible 
     *        to delete the object whenever needed.  
     */
    TF1* profile(double npv,double mu,double window=1.6,
		 const std::string& name="SlidingWindow");
    
    /*! @brief Rerieve pointer to @c ProfileShape used
     * 
     *  @return @c const pointer to non-modifiable @c PofileShape used in the 
     *          instance of @c ProfileFunctions .
     * 
     *  @param[in] npv number of reconstructed primary vertices
     *  @param[in] mu  average number of interactions 
     *  @param[in] window (optional) window size for profile calculation,
     *                    defaults to 1.6.
     */
    const ProfileShape* shape(double npv,double mu,double window=1.6); 

  private:
  
    /*! @brief @c ProfileShape object used to evaluate density */
    mutable ProfileShape* f_shape;

    /*! @brief Helper function (2nd order polynominal)*/
    double f_poly2(double p0,double p1,double x);
    /*! @brief Helper function (3rd order polynominal)*/
    double f_poly3(double p0,double p1,double p2,double x);

    ///////////////////////////////////////
    // Range only depends on window size //
    ///////////////////////////////////////

    /*! @defgroup range_p Function parameters for range
     * 
     *  @brief Parametrization of range @f$ \eta_{\mathrm{plateau}} @f$ 
     * 
     *  The range @f$ \eta_{\mathrm{plateau}} @f$ is only a function
     *  of the window size @f$ w @f$ used to calculate the profile:
     *
     *  @f[
     *   \eta_{\mathrm{plateau}}(w) = p_{0,\mathrm{plateau}} 
     *                              + p_{1,\mathrm{plateau}} \cdot w 
     *                              + p_{2,\mathrm{plateau}} \cdot w^{2}
     *  @f]  
     * 
     *  @{
     */
    static double p_range_window_p0; 
    /*!< @brief Parameter @f$ p_{0,\mathrm{plateau}} @f$ */
    static double p_range_window_p1; 
    /*!< @brief Parameter @f$ p_{1,\mathrm{plateau}} @f$ */
    static double p_range_window_p2; 
    /*!< @brief Parameter @f$ p_{2,\mathrm{plateau}} @f$ */
    /*! @}*/

    ///////////////////////////////////////
    // Shape only depends on window size //
    ///////////////////////////////////////

    /*! @defgroup shape_p Function parameters for shape
     * 
     *  @brief Parametrization of shape @f$ \sigma_{\eta} @f$
     * 
     *  The shape @f$ \sigma_{\eta} @f$ is only a function
     *  of the window size @f$ w @f$ used to calculate the profile:
     *
     *  @f[
     *   \sigma_{\eta}(w) = p_{0,\mathrm{shape}} 
     *                              + p_{1,\mathrm{shape}} \cdot w 
     *                              + p_{2,\mathrm{shape}} \cdot w^{2}
     *  @f]  
     * 
     *  @{
     */
    static double p_shape_window_p0;
    /*!< @brief Parameter @f$ p_{0,\mathrm{shape}} @f$ */
    static double p_shape_window_p1;
    /*!< @brief Parameter @f$ p_{1,\mathrm{shape}} @f$ */
    static double p_shape_window_p2;
    /*!< @brief Parameter @f$ p_{2,\mathrm{shape}} @f$ */
    /*! @}*/

    ////////////////////////////
    // Baseline depends on mu //
    ////////////////////////////

    /*! @defgroup base_p Function parameters for forward baseline
     * 
     *  @brief Parametrization of forward baseline @f$ \rho_{\mathrm{base}} @f$
     * 
     *  The shape @f$ \rho_{\mathrm{base}} @f$ is a function
     *  of the average number of interactions @f$ \mu @f$ and the number
     *  of reconstructed vertices @f$ N_{\mathrm{PV}} @f$:
     *
     *  @f[
     *   \rho_{\mathrm{base}}(N_{\mathrm{PV}},\mu) = 
     *    \underbrace{( p_{0,\mathrm{base}} 
     *                + p_{1,\mathrm{base}} \cdot \mu )}_{\mathrm{offset}}
     *                                             + 
     *    \underbrace{( q_{0,\mathrm{base}}
     *                + q_{1,\mathrm{base}} \cdot \mu )}_{\mathrm{slope}}
     *                 \cdot N_{\mathrm{PV}} 
     *  @f]  
     * 
     *  @{
     */
    static double p_baseline_mu_offset_p0;
    /*!< @brief Parameter @f$ p_{0,\mathrm{base}} @f$ */
    static double p_baseline_mu_offset_p1;
    /*!< @brief Parameter @f$ p_{1,\mathrm{base}} @f$ */
    static double p_baseline_mu_slope_p0;
    /*!< @brief Parameter @f$ q_{0,\mathrm{base}} @f$ */
    static double p_baseline_mu_slope_p1;
    /*!< @brief Parameter @f$ q_{1,\mathrm{base}} @f$ */
    /*! @}*/

    double m_window; /*!< @brief Actual window size */
    double m_npv;    /*!< @brief Actual number of reconstructed vertices */
    double m_mu;     /*!< @brief Actual average number of collisions */
    double m_etaMin; /*!< @brief Actual @f$ \eta_{\mathrm{min}} @f$ */
    double m_etaMax; /*!< @brief Actual @f$ \eta_{\mathrm{max}} @f$ */
    
  };
  /*! @class ProfileFunctions
   * 
   *  @brief Implements functions describing the @f$ \eta @f$ density profile.
   */
}

inline double 
EventShapeCalculators::ProfileFunctions::f_poly2(double p0,double p1,double x)
{ return p0+p1*x; }

inline double 
EventShapeCalculators::ProfileFunctions::f_poly3(double p0,double p1,double p2,
						 double x)
{ return this->f_poly2(p0,p1,x)+p2*x*x; }

/*! @namespace EventShapeCalculators
 *
 * @brief Collection of function objects useful for shape calculations.
 * 
 */

#endif
