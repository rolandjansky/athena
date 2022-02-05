/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RtCalibrationAnalytic__H
#define RtCalibrationAnalytic__H

//:::::::::::::::::::::::::::::::::
//:: CLASS RtCalibrationAnalytic ::
//:::::::::::::::::::::::::::::::::

namespace MuonCalib {
    /// \class RtCalibrationAnalytic
    /// This class performs the analytic autocalibration whose basic ideas were
    /// developed by Mario Deile (see ATL-MUON-2004-021).
    ///
    /// \author Oliver.Kortner@CERN.CH
    ///
    /// \date 06.04.2006
    /**
    @class RtCalibrationAnalytic
    This class performs the analytic autocalibration whose basic ideas were
    developed by Mario Deile (see ATL-MUON-2004-021).

    @author Oliver.Kortner@CERN.CH

    */

}  // namespace MuonCalib

// STL //
#include <memory>
#include <string>
#include <vector>
// CLHEP //
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

// ROOT //
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// MuonCalib //
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MdtCalibRt/MeanRMS.h"

namespace MuonCalib {

    class IRtRelation;
    class RtRelationLookUp;
    class RtCalibrationOutput;
    class IMdtCalibrationOutput;
    class MuonCalibSegment;
    class BaseFunction;

    class RtCalibrationAnalytic : public IMdtCalibration {
    public:
        // Constructors //
        RtCalibrationAnalytic(const std::string &name);
        ///< Default constructor: r-t accuracy is set to 0.5 mm.
        ///< The r-t accuracy is used internally to distinguish between good
        ///< and bad segments.
        ///< By default Legendre polynomials are used to parametrize the
        ///< r-t correction.
        ///< The order of the r-t correction polynomial is set to 5.
        ///< Segments are restricted to single multilayers. The full matrix
        ///< relating the errors in r(t) to the residuals is used.
        ///< By default no smoothing is applied after convergence.

        RtCalibrationAnalytic(const std::string &name, const double &rt_accuracy, const unsigned int &func_type, const unsigned int &ord,
                              const bool &split, const bool &full_matrix, const bool &fix_min, const bool &fix_max, const int &max_it,
                              bool do_smoothing = false, bool do_parabolic_extrapolation = false);
        ///< Constructor: r-t accuracy is set to rt_accuracy (unit: CLHEP::mm).
        ///< The r-t accuracy is used internally to distinguish between good
        ///< and bad segments.
        ///< \param func_type: type of function to be used for the r-t correction;
        ///<    = 1: Legendre polynomial,
        ///<    = 2: Chebyshev polynomial,
        ///<    = 3: polygon equidistant in r.
        ///< \param ord The order of the r-t correction polynomial is set to ord.
        ///< \param split = true forces the algorithm to restrict segments to
        ///< multilayers. Otherwise, segments may contain hits from different
        ///< multilayers.
        ///< \param full_matrix = true lets the algorithm use the full matrix
        ///< relating the errors in r(t) to the residuals is used. Otherwise
        ///< the unit matrix is used, making the code equivalent to the
        ///< conventional/classical method.
        ///< \param fix_min,fix_max =true fix r(t_min), r(t_max) (this is default).
        ///< \param max_it maximum number of iterations.
        ///< \param do_smoothing Smoothen the r-t relations after convergence.
        ///> \param do_parabolic_extrapolation Use parabolic extrapolations for
        ///< small and larged drift radii.

        // Desctructor //
        ~RtCalibrationAnalytic();

        // Methods //
        // get-methods //
        double reliability() const;
        ///< get the reliability of the r-t:
        ///< 0: no convergence yet
        ///< 1: convergence, r-t is reliable
        ///< 2: convergence, r-t is unreliable
        double estimatedRtAccuracy() const;
        ///< get the estimated r-t quality (CLHEP::mm),
        ///< the accuracy of the input r-t is
        ///< computed at the end of the
        ///< iteration; in order to get the
        ///< accuracy of the final r-t, the
        ///< algorithm has to be rerun with the
        ///< final r-t as an input
        int numberOfSegments() const;
        ///< get the number of segments which
        ///< were passed to the algorithm
        int numberOfSegmentsUsed() const;
        ///< get the number of segments which
        ///< are used in the autocalibration
        int iteration() const;
        ///< get the number of the current
        ///< iteration
        bool splitIntoMultilayers() const;
        ///< returns true, if segments are
        ///< internally restricted to single
        ///< multilayers;
        ///< returns false, if segments may
        ///< contain hits from both multilayers
        bool fullMatrix() const;
        ///< returns true, if the full matrix
        ///< relating the errors in the r-t
        ///< relationship to the residuals
        ///< should be used;
        ///< returns false, if the unit matrix
        ///< is used
        bool smoothing() const;
        ///< returns true, if the r-t relationship
        ///< will be smoothened using the
        ///< conventional autocalibration after
        ///< convergence;
        ///< returns false otherwise

        // set-method //
        void setEstimateRtAccuracy(const double &acc);
        ///< set the estimated r-t accuracy =acc
        void splitIntoMultilayers(const bool &yes_or_no);
        ///< yes_or_no=true: segments are
        ///< internally restriced to single
        ///< multilayers;
        ///< yes_or_no=false: segments over
        ///< two multilayers of a chamber are
        ///< allowed
        void fullMatrix(const bool &yes_or_no);
        ///< yes_or_no=true: the full matrix
        ///< relating the errors in the r-t
        ///< relationship to the residuals
        ///< is used
        ///< yes_or_no=false: unit matrix is
        ///< used (algorithm is equivalent to
        ///< to the conventional/classical
        ///< method
        void switch_on_control_histograms(const std::string &file_name);
        ///< this methods requests control
        ///< histograms from the algorithms;
        ///< the algorithm will write them to
        ///< ROOT file called "file_name"
        void switch_off_control_histograms();
        ///< the algorithm does not produce
        ///< controll histograms (this is the
        ///< default)
        void forceMonotony();
        ///< force r(t) to be monotonically
        ///< increasing
        void doNotForceMonotony();
        ///< do not force r(t) to be
        ///< monotonically increasing
        void doSmoothing();
        ///< requires that the r-t relationship
        ///< will be smoothened using the
        ///< conventional autocalibration after
        ///< convergence
        void noSmoothing();
        ///< do not smoothen the r-t relationship
        ///< after convergence
        void doParabolicExtrapolation();
        ///< requires that parabolic extrapolation
        ///< will be used for small and large radii
        void noParabolicExtrapolation();
        ///< no parabolic extrapolation is done

        // methods required by the base class "IMdtCalibration" //
        MdtCalibOutputPtr analyseSegments(const MuonSegVec &seg);
        ///< perform the full autocalibration
        ///< including iterations
        ///< (required since
        ///< MdtCalibInterfaces-00-01-06)
        bool handleSegment(MuonCalibSegment &seg);
        ///< analyse the segment "seg"
        ///< (this method was required before
        ///< MdtCalibInterfaces-00-01-06)
        void setInput(const IMdtCalibrationOutput *rt_input);
        ///< set the r-t relationship,
        ///< the internal autocalibration
        ///< objects are reset
        bool analyse();
        ///< perform the autocalibration with
        ///< the segments acquired so far
        bool converged() const;
        ///< returns true, if the
        ///< autocalibration has converged
        MdtCalibOutputPtr getResults() const;
        ///< returns the final r-t relationship

    private:
        // options //
        bool m_control_histograms = false;  // = true, if control histograms should be
                                    //         produces
        bool m_split_into_ml = false;       // = true, if segments should be restricted to the
                                    //         multilayers;
                                    // = false, if segments over both multilayers are
                                    //         allowed
        bool m_full_matrix = false;         // = true, if the full matrix relating the errors in
                                    //         the r-t relationship to the residuals
                                    //         should be used;
                                    // = false, if a diagonal matrix should be used;
                                    //          in this case the algorithm is equivalent
                                    //          to conventional method
        bool m_fix_min = false;             // = true: fix r(t_min)
        bool m_fix_max = false;             // = true: fix r(t_max)
        int m_max_it = 0;               // maximum number of iterations
        bool m_force_monotony = false;      // = true if r(t) is forced to monotonically
                                    //        increasing, false otherwise

        // bookkeeping //
        int m_nb_segments = 0;                   // number of segments passed to the algorithm
        int m_nb_segments_used = 0;              // number of segments used by the algorithm
        int m_iteration = 0;                     // current iteration
        std::array<bool, 2> m_multilayer{};  // m_multilayer[k] = true, if there was a segment
                                             //                   extending to multilayer k+1

        // r-t quality //
        int m_status = 0;                   // m_status: 0: no covergence yet,
                                        //           1: convergence, r-t is reliable,
                                        //           2: convergence, r-t is unreliable
        double m_rt_accuracy = 0.0;           // r-t accuracy (CLHEP::mm) of the input r-t
        double m_rt_accuracy_previous = 0.0;  // r-t accuracy of the previous iteration
                                        // (used in the convergence criterion)
        double m_chi2_previous = 0.0;
        // average chi^2 per degrees of freedom from the
        // previous iteration (set to a large initial value
        // to force at least two iterations);
        // if an iteration gives a larger average than the
        // pervious iteration, the algorithm has converged
        double m_chi2 = 0.0;  // average chi^2 per degrees of freedom,
                        // if an iteration gives a larger average than the
                        // pervious iteration, the algorithm has converged

        // r-t relationship //
        std::shared_ptr<const IRtRelation> m_rt;        // pointer to the input r-t relationship
        double m_t_length = 0.0;                        // size of the drift time interval
        double m_t_mean = 0.0;                          // mean value of the drift time interval

        // r-t output //
        std::shared_ptr<IRtRelation> m_rt_new;          // r-t as determined by the autocalibration
        std::shared_ptr<RtCalibrationOutput> m_output;  // class holding the results of the
                                                        // autocalibration

        // straight-segment fitting //
        double m_r_max = 0.0;                       // maximum value for accepted drift radii
        QuasianalyticLineReconstruction m_tracker;  // quasianalytic
                                                    // straight-line segment
                                                    // finder, used because it
                                                    // performs a pattern
                                                    // recognition

        // autocalibration objects //
        bool m_do_smoothing = false;        // = true: the r-t relationship is smoothened after
                                            //         convergence, no smoothing is done
                                            //         otherwise
        bool m_do_parabolic_extrapolation = false;  // = true: parabolic extrapolation is
                                            //         done for small and large radii
                                            // = false: no parabolic extrapolation is
                                            //          done
        unsigned int m_order = 0U;          // order of the polynomial describing the
                                            // correction to the r-t relationship
        std::vector<CLHEP::HepVector> m_U;  // vector of base function values
        CLHEP::HepSymMatrix m_A;            // coefficient matrix of the final autocalibration
                                            // equation
        CLHEP::HepVector m_alpha;           // vector of fit parameters, i.e. the coefficients
                                            // of the correction polynomial
        CLHEP::HepVector m_b;               // m_A*m_alpha = m_b (final autocalibration equation)

        // Legendre polynomial //
        std::unique_ptr<BaseFunction> m_base_function;  // pointer to the base function u

        // control histograms //
        std::unique_ptr<TFile> m_tfile;           // ROOT file
        std::unique_ptr<TH1F> m_cut_evolution;    // cut evolution histogram
        std::unique_ptr<TH1F> m_nb_segment_hits;  // number of hits on the segments
        std::unique_ptr<TH1F> m_CL;               // confidence level distribution of the selected segments
        std::unique_ptr<TH2F> m_residuals;        // residual distribution

        // private methods //
        void init(const double &rt_accuracy, const unsigned int &func_type, const unsigned int &ord, const bool &split,
                  const bool &full_matrix, const bool &fix_min, const bool &fix_max, const int &max_it, bool do_smoothing,
                  bool do_parabolic_extrapolation);
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
        // full_matrix = true forces the algorithm ti
        // use the full matrix relating the errors in
        // in r(t) to the residuals; otherwise a unit
        // matrix is used;
        // fix_min, fix_max=true: fix r(t_min), r(t_max);
        // do_smoothing: smoothen the r-t relationship
        // after convergence if do_smoothing = true;
        // do_parabolic_extrapolation: do or not do
        // parabolic extrapolation for small and large
        // radii;
        // max_it: maximum number of iterations
        double t_from_r(const double &r);
        // get t(r) for the input r-t relationship,
        // the method is auxiliary and not optimized;
        // it will disappear when the t(r) will be
        // available in the MuonCalib framework

        void display_segment(MuonCalibSegment *segment, std::ofstream &outfile);

        std::shared_ptr<RtRelationLookUp> performParabolicExtrapolation(const bool &min, const bool &max, const IRtRelation &in_rt);
        // use parabolic extrapolations on the given r-t
        // relationship in_rt;
        // min: if true, use parabolic extrapolation towards
        //      r=0;
        // max: if true, use parabolic extrapolation towards
        //      r=r_max;

        MeanRMS m_track_slope;
        MeanRMS m_track_position;  // mean and rms of track slope and position
    };

}  // namespace MuonCalib

#endif
