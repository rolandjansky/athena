/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_RtCalibrationIntegrationH
#define MuonCalib_RtCalibrationIntegrationH

#include <memory>
#include <string>
#include <vector>

#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibInterfaces/IMdtCalibration.h"
#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MdtCalibInterfaces/IMdtSegmentFitter.h"
#include "MdtCalibRt/RtCalibrationOutput.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

namespace MuonCalib {

    //::::::::::::::::::::::::::::::::::::
    //:: CLASS RtCalibrationIntegration ::
    //::::::::::::::::::::::::::::::::::::

    /// \class RtCalibrationIntegration
    ///
    /// This class allows the user to obtain an r-t relationship from the drift
    /// time spectrum in a given calibration region. The user can ask the class
    /// to restrict the r-t determination on hit on the segments or to use close
    /// hits too. The algorithms performs a t0 and tmax fit in order to get t(r=0)
    /// and t(r=rmax) right.

    class RtCalibrationIntegration : public IMdtCalibration {
    public:
        // Constructors //
        RtCalibrationIntegration(const std::string& name) : IMdtCalibration(name) { init(false, 14.6, 13.0, 14.0, false); }
        ///< Default constructor. Only hits on the segment are used by the
        ///< algorithm by default. The maximum drift radius is set to  14.6 mm.

        RtCalibrationIntegration(const std::string& name, bool close_hits, double r_max, double lower_extrapolation_radius,
                                 double upper_extrapolation_radius, bool add_tmax_difference) :
            IMdtCalibration(name) {
            init(close_hits, r_max, lower_extrapolation_radius, upper_extrapolation_radius, add_tmax_difference);
        }
        ///< Constructor. If close_hits is set to true, also close hits are
        ///< used. The maximum drift radius is set to r_max [mm].

        // Methods //
        // get-methods //
        unsigned int number_of_hits_used(void) const;
        ///< get the number of hits used in the
        ///< r-t determination

        // methods required by the base class "IMdtCalibration" //
        MdtCalibOutputPtr analyseSegments(const MuonSegVec& seg) override;
        ///< determine r(t)
        bool handleSegment(MuonCalibSegment& seg);
        ///< analyse the segment "seg"
        void setInput(const IMdtCalibrationOutput* rt_input) override;
        ///< the method is empty as no initial
        ///< r-t relationship is required by
        ///< the algorithm
        bool analyse(void);
        ///< perform the integration method
        bool converged(void) const;
        ///< returns true, if the integration
        ///< method has been performed
        MdtCalibOutputPtr getResults() const override;
        ///< returns the final r-t relationship

    private:
        // options //
        bool m_close_hits;                    // = true, if close hits should be used,
                                              // = false, if only hits on segments should be used
        double m_lower_extrapolation_radius;  ///< sets the lower radius to perform the
        double m_upper_extrapolation_radius;  ///< parabolic extrapolation. the parabolic fit will be
                                              ///< performet between the lower extrapolation radius
                                              ///< and the upper extrapolation value and be extrapolated
                                              ///< till r_max
        bool m_add_tmax_difference;
        // drift-time spectrum //

        std::vector<std::pair<double, bool> > m_t_drift;  // measured drift times
                                                          // r(t) //
        std::shared_ptr<IRtRelation> m_rt;                // pointer to the final r-t relationship
        unsigned int m_nb_hits_used;                      // number of hits used in the algorithm
        unsigned int m_nb_segments_used;                  // number of segments used
        double m_r_max;                                   // maximum drift radius
        std::shared_ptr<RtCalibrationOutput> m_output;    // class holding the results of the
                                                          // autocalibration

        // private methods //
        void init(bool close_hits, double r_max, double lower_extrapolation_radius, double higher_extrapolation_radius,
                  bool add_tmax_difference);
        // initialize the class, close hits are used,
        // if close_hits is set to true; the maximum
        // drift radius is set to r_max
    };

}  // namespace MuonCalib

#endif
