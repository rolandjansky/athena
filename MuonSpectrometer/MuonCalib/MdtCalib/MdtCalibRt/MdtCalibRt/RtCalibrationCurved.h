/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_RtCalibrationCurvedH
#define MuonCalib_RtCalibrationCurvedH

/// \class RtCalibrationCurved
///
/// This class performs the autocalibration of MDT chambers from the residuals
/// of curved (parabolic) segments. The method used is a generalization of the
/// analytic autocalibration developed by Deile, Hessey, and Staude (see
/// ATL-MUON-2004-021).
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 10.05.2008

#include <vector>
#include <string>
#include <list>
#include <memory>

// CLHEP //
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

// ROOT //
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// MuonCalib //
#include "MdtCalibInterfaces/IMdtCalibration.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

class IMdtCalibrationOutput;
class IRtRelation;
class RtRelationLookUp;
class RtCalibrationOutput;
class CurvedPatRec;
class MuonCalibSegment;
class BaseFunction;
class Legendre_polynomial;
class CurvedLine;
class MultilayerRtDifference;

class RtCalibrationCurved : public IMdtCalibration {

public:
// Constructors //
    RtCalibrationCurved(std::string name);
    ///< Default constructor: r-t accuracy is set to 0.5 mm.
    ///< The r-t accuracy is used internally to distinguish between good
    ///< and bad segments.
    ///< By default Legendre polynomials are used to parametrize the
    ///< r-t correction.
    ///< The order of the r-t correction polynomial is set to 15.
    ///< Segments are not restricted to single multilayers. The full matrix
    ///< relating the errors in r(t) to the residuals is used.
    ///< No parabolic extrapolations are used.
	///< By default no smoothing is applied after convergence.

    RtCalibrationCurved(std::string name,
                        const double & rt_accuracy,
                        const unsigned int & func_type,
                        const unsigned int & ord,
                        const bool & fix_min,
                        const bool & fix_max,
                        const int & max_it,
                        bool do_parabolic_extrapolation=false,
                        bool do_smoothing=false,
			bool do_multilayer_rt_scale=false);
    ///< Constructor.
    ///< \param rt_accuracy r-t accuracy in mm. The r-t accuracy is used
    ///<        internally to distinguish between good and bad segments.
    ///< \param func_type  Type of function to be used for the r-t correction;
    ///<        = 1: Legendre polynomial (default),
    ///<        = 2: Chebyshev polynomial,
    ///<        = 3: polygon equidistant in r.
    ///< \param ord Order of the r-t correction function (15 is default).
    ///< \param fix_min =true: r(t0) is fixed in the autocalibration procedure
    ///<        (this is default).
    ///< \param fix_max =true: r(tmax) is fixed in the autocalibration procedure
    ///<        (false is default).
    ///< \param max_it Maximum number of iterations (20 by default).
    ///< \param do_parabolic_extrapolation Use parabolic extrapolations for
    ///<        small and larged drift radii.
	///< \param do_smoothing Smoothing of the r-t relations after convergence.

// Destructor //
    ~RtCalibrationCurved(void);
    ///< Destructor.

// Methods //
// get-methods //
    double reliability(void) const;
                                    ///< get the reliability of the final r-t
                                    ///< relationship:
                                    ///< 0: no convergence yet
                                    ///< 1: convergence, r(t) is reliable
                                    ///< 2: convergence, r(t) is unreliable
    double estimatedRtAccuracy(void) const;
                                    ///< get the estimated r-t quality (CLHEP::mm),
                                    ///< the accuracy of the input r-t is
                                    ///< computed at the end of the
                                    ///< iteration; in order to get the
                                    ///< accuracy of the final r-t, the
                                    ///< algorithm has to be rerun with the
                                    ///< final r-t as an input
    int numberOfSegments(void) const;
                                    ///< get the number of segments which
                                    ///< were passed to the algorithm
    int numberOfSegmentsUsed(void) const;
                                    ///< get the number of segments which
                                    ///< are used in the autocalibration
    int iteration(void) const;
                                    ///< get the number of the current
                                    ///< iteration
	bool smoothing(void) const;
									///< returns true, if the r-t relationship
									///< will be smoothened using the
									///< conventional autocalibration after
									///< convergence;
									///< returns false otherwise

// set-method //
    void setEstimateRtAccuracy(const double & acc);
                                    ///< set the estimated r-t accuracy =acc
    void fullMatrix(const bool & yes_or_no);
                                    ///< yes_or_no=true: the full matrix
                                    ///< relating the errors in the r-t
                                    ///< relationship to the residuals
                                    ///< is used
                                    ///< yes_or_no=false: unit matrix is
                                    ///< used (algorithm is equivalent to
                                    ///< to the conventional/classical
                                    ///< method
    void switch_on_control_histograms(const std::string & file_name);
                                    ///< this methods requests control
                                    ///< histograms from the algorithms;
                                    ///< the algorithm will write them to
                                    ///< ROOT file called "file_name"
    void switch_off_control_histograms(void);
                                    ///< the algorithm does not produce
                                    ///< controll histograms (this is the
                                    ///< default)
    void forceMonotony(void);
                                    ///< force r(t) to be monotonically
                                    ///< increasing (this is default)
    void doNotForceMonotony(void);
                                    ///< do not force r(t) to be
                                    ///< monotonically increasing
	void doParabolicExtrapolation(void);
									///< requires that parabolic extrapolation
                                    ///< will be used for small and large radii
	void noParabolicExtrapolation(void);
									///< no parabolic extrapolation is done
	void doSmoothing(void);
									///< requires that the r-t relationship
									///< will be smoothened using the
									///< conventional autocalibration after
									///< convergence
	void noSmoothing(void);
									///< do not smoothen the r-t relationship
									///< after convergence

// methods required by the base class "IMdtCalibration" //
    const IMdtCalibrationOutput * analyseSegments(
                                const std::vector<MuonCalibSegment*> & seg);
                                    ///< perform the full autocalibration
                                    ///< including iterations
                                    ///< (required since
                                    ///< MdtCalibInterfaces-00-01-06)
    bool handleSegment(MuonCalibSegment & seg);
                                    ///< analyse the segment "seg"
                                    ///< (this method was required before
                                    ///< MdtCalibInterfaces-00-01-06)
    void setInput(const IMdtCalibrationOutput * rt_input);
                                    ///< set the r-t relationship,
                                    ///< the internal autocalibration
                                    ///< objects are reset
    bool analyse(const std::vector<MuonCalibSegment*> & seg);
                                    ///< perform the autocalibration with
                                    ///< the segments acquired so far
    bool converged(void) const;
                                    ///< returns true, if the
                                    ///< autocalibration has converged
    const IMdtCalibrationOutput * getResults(void) const;
                                    ///< returns the final r-t relationship

private:
// options //
    bool m_control_histograms; // = true, if control histograms should be
                               //         produces
    bool m_fix_min; // = true: fix r(t_min)
    bool m_fix_max; // = true: fix r(t_max)
    int m_max_it; // maximum number of iterations
    bool m_force_monotony; // = true if r(t) is forced to monotonically
                           //   increasing, false otherwise
    bool m_do_multilayer_rt_scale; // determine multilayer rt scaling

// bookkeeping //
    int m_nb_segments; // number of segments passed to the algorithm
    int m_nb_segments_used; // number of segments used by the algorithm
    int m_iteration; // current iteration
    bool m_multilayer[2]; // m_multilayer[k] = true, if there was a segment
                          //                   extending to multilayer k+1

// r-t quality //
    int m_status; // m_status: 0: no covergence yet,
                  //           1: convergence, r-t is reliable,
                  //           2: convergence, r-t is unreliable
    double m_rt_accuracy; // r-t accuracy (CLHEP::mm) of the input r-t
    double m_rt_accuracy_previous; // r-t accuracy of the previous iteration
                                   // (used in the convergence criterion)
    double m_chi2_previous; 
                      // average chi^2 per degrees of freedom from the
                      // previous iteration (set to a large initial value
                      // to force at least two iterations);
                      // if an iteration gives a larger average than the
                      // pervious iteration, the algorithm has converged
    double m_chi2; // average chi^2 per degrees of freedom,
                   // if an iteration gives a larger average than the
                   // pervious iteration, the algorithm has converged

// r-t relationship //
    const IRtRelation * m_rt; // pointer to the input r-t relationship
    double m_t_length; // size of the drift time interval
    double m_t_mean; // mean value of the drift time interval

// r-t output //
    IRtRelation * m_rt_new; // r-t as determined by the autocalibration
    RtCalibrationOutput * m_output; // class holding the results of the
                                    // autocalibration
    std::unique_ptr<MultilayerRtDifference> m_multilayer_rt_difference;
// curved-segment fitting //
    double m_r_max; // maximum value for accepted drift radii
    CurvedPatRec *m_tracker; // curved segment finder (used for track fitting)
  // The following three objects are needed for autocalibration formulae.
  
  
  
    CLHEP::HepSymMatrix m_M_track; // segment parameters = m_M_track^-1*m_b_track
    CLHEP::HepSymMatrix m_M_track_inverse; // inverse of m_M_track

// autocalibration objects //
	bool m_do_parabolic_extrapolation; // = true: parabolic extrapolation is
                                       //         done for small and large radii
                                       // = false: no parabolic extrapolation is
                                       //          done
	bool m_do_smoothing; // = true: the r-t relationship is smoothened after
						 //         convergence, no smoothing is done
						 //         otherwise
    unsigned int m_order; // order of the polynomial describing the
                          // correction to the r-t relationship
    std::vector<CLHEP::HepVector> m_U; // vector of base correction function values
    std::vector<CLHEP::HepVector> m_U_weighted; // vector of base correction function
                                         // values weighted by the inverse
                                         // standard deviation of the radius
                                         // measurements
    CLHEP::HepSymMatrix m_A; // coefficient matrix of the final autocalibration
                      // equation
    CLHEP::HepVector m_alpha; // vector of fit parameters, i.e. the coefficients
                       // of the correction polynomial
    CLHEP::HepVector m_b; // m_A*m_alpha = m_b (final autocalibration equation)

// correction functions //
    BaseFunction *m_base_function; // pointer to the base function u
    Legendre_polynomial *m_Legendre; // pointer to the Legendre polynomial
                                     // describing the curved line

// control histograms //
    std::unique_ptr<TFile> m_tfile; // ROOT file
    std::unique_ptr<TH1F> m_cut_evolution; // cut evolution histogram
    std::unique_ptr<TH1F> m_nb_segment_hits; // number of hits on the segments
    std::unique_ptr<TH1F> m_pull_initial; // initial pull distribution
    std::unique_ptr<TH1F> m_pull_final; // final pull distribution after convergence
    std::unique_ptr<TH2F> m_residuals_initial; // initial residual distribution
    std::unique_ptr<TH2F> m_residuals_initial_all; // initial residual distribution before convergence
    std::unique_ptr<TH2F> m_residuals_final; // final residual distribution after convergence
    std::unique_ptr<TH2F> m_driftTime_initial; // final residual distribution after convergence
    std::unique_ptr<TH2F> m_driftTime_final; // final residual distribution after convergence
    std::unique_ptr<TH2F> m_adc_vs_residual_final; // final residual distribution after convergence

// private methods //
    void init(const double & rt_accuracy, const unsigned int & func_type,
                const unsigned int & ord,
                const bool & fix_min, const bool & fix_max,
                const int & max_it,
                bool do_parabolic_extrapolation,
		        bool do_smoothing,
			bool do_multilayer_rt_scale);
                            // initialization method:
                            // rt_accuracy = estimated r-t accuracy,
                            // func_type: type of function to be used for
                            //            the r-t correction;
                            //     = 1: Legendre polynomial,
                            //     = 2: Chebyshev polynomial,
                            //     = 3: polygon
                            // ord = "order" ot the r-t correction function
                            // split = true forces the algorithm to restrict
                            // segments to multilayers;
                            // fix_min, fix_max=true: fix r(t_min), r(t_max)
                            // max_it: maximum number of iterations
                            // do_parabolic_extrapolation: do or do not use
                            //    parabolic extrapolations for small and large
                            //    radii;
							// do_smoothing: smoothen the r-t relationship
							//    after convergence if do_smoothing = true
     double t_from_r(const double & r);
                            // get t(r) for the input r-t relationship,
                            // the method is auxiliary and not optimized;
                            // it will disappear when the t(r) will be
                            // available in the MuonCalib framework
    void display_segment(MuonCalibSegment * segment,
                        std::ofstream & outfile,
						const CurvedLine * curved_segment);
                            // write out a simple PAW macro displaying the
                            // segment; if the pointer "curved_segment" equals
                            // 0, a straight line as stored in segment is drawn;
                            // the curved line is used otherwise
    RtRelationLookUp * performParabolicExtrapolation(const bool & min,
                            const bool & max,
                            const IRtRelation & in_rt);
                            // use parabolic extrapolations on the given r-t
                            // relationship in_rt;
                            // min: if true, use parabolic extrapolation towards
                            //      r=0;
                            // max: if true, use parabolic extrapolation towards
                            //      r=r_max;
};

}

#endif
