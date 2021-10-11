/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/CurvedLine.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/LegendrePolynomial.h"

using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::::::::::
//:: DEFAULT CONSTRUCTOR ::
//:::::::::::::::::::::::::

CurvedLine::CurvedLine() {
    ///////////////
    // VARIABLES //
    ///////////////

    std::vector<Amg::Vector3D> points(3);
    std::vector<Amg::Vector3D> errors(3);

    ////////////////////
    // FILL VARIABLES //
    ////////////////////

    points[0] = Amg::Vector3D(0.0, 0.0, 0.0);
    errors[0] = Amg::Vector3D(1.0, 1.0, 0.0);
    points[1] = Amg::Vector3D(0.0, 0.0, 1.0);
    errors[1] = Amg::Vector3D(1.0, 1.0, 0.0);
    points[2] = Amg::Vector3D(0.0, 0.0, 2.0);
    errors[2] = Amg::Vector3D(1.0, 1.0, 0.0);

    ////////////////////
    // INITIALIZATION //
    ////////////////////

    init(points, errors);
}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

CurvedLine::CurvedLine(std::vector<Amg::Vector3D>& points) {
    ///////////////
    // VARIABLES //
    ///////////////

    std::vector<Amg::Vector3D> errors(points.size());

    ////////////////////
    // FILL VARIABLES //
    ////////////////////

    for (unsigned int k = 0; k < errors.size(); k++) { errors[k] = Amg::Vector3D(1.0, 1.0, 0.0); }

    ////////////////////
    // INITIALIZATION //
    ////////////////////

    init(points, errors);
}

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

CurvedLine::CurvedLine(std::vector<Amg::Vector3D>& points, std::vector<Amg::Vector3D>& x_and_y_errors) { init(points, x_and_y_errors); }

//*****************************************************************************

//::::::::::::::::::::::::::::::
//:: METHOD getPointOnLine ::
//::::::::::::::::::::::::::::::

Amg::Vector3D CurvedLine::getPointOnLine(const double& loc_z) const {
    ///////////////
    // VARIABLES //
    ///////////////

    double loc_x(0.0), loc_y(0.0);

    ///////////////////////////////
    // CALCULATE THE COORDINATES //
    ///////////////////////////////

    for (int k = 0; k < m_coeff_xz.rows(); k++) { loc_x = loc_x + m_coeff_xz[k] * m_Legendre->value(k, loc_z); }

    for (int k = 0; k < m_coeff_yz.rows(); k++) { loc_y = loc_y + m_coeff_yz[k] * m_Legendre->value(k, loc_z); }

    ////////////////////////////////
    // RETURN THE REQUESTED POINT //
    ////////////////////////////////

    return Amg::Vector3D(loc_x, loc_y, loc_z);
}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD getTangent ::
//::::::::::::::::::::::::

MTStraightLine CurvedLine::getTangent(const double& loc_z) const {
    ///////////////
    // VARIABLES //
    ///////////////

    Amg::Vector3D null_vec(0.0, 0.0, 0.0);               // auxiliary 0 vector
    Amg::Vector3D point_1(getPointOnLine(loc_z));        // first point on the curved
                                                         // line
    Amg::Vector3D point_2(getPointOnLine(loc_z + 1.0));  // second point on the
                                                         // curved line

    ////////////////////////
    // RETURN THE TANGENT //
    ////////////////////////

    return MTStraightLine(point_1, point_2 - point_1, null_vec, null_vec);
}

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::

void CurvedLine::init(std::vector<Amg::Vector3D>& points, std::vector<Amg::Vector3D>& x_and_y_errors) {
    ////////////////////////////////
    // CHECK THE NUMBER OF POINTS //
    ////////////////////////////////

    if (points.size() < 3) {
        MsgStream log(Athena::getMessageSvc(), "CurvedLine");
        log << MSG::ERROR << "Class CurvedLine, method init: Not enough points given, must be at least 3 points!" << endmsg;
    }

    ///////////////
    // VARIABLES //
    ///////////////

    BaseFunctionFitter fitter;
    LegendrePolynomial legendre;                            // Legendre polynomial needed by the base
                                                            // function fitter
    std::vector<SamplePoint> sample_points(points.size());  // sample points needed
                                                            // by the base function
                                                            // fitter
                                                            /////////////////////////////////////////////
                                                            // FILL THE VARIABLES AND PERFORM THE FITS //
                                                            /////////////////////////////////////////////

    // xz plane //
    for (unsigned int k = 0; k < points.size(); k++) {
        sample_points[k].set_x1(points[k].z());
        sample_points[k].set_x2(points[k].x());
        sample_points[k].set_error(x_and_y_errors[k].x());
    }
    fitter.set_number_of_coefficients(2);
    fitter.fit_parameters(sample_points, 1, sample_points.size(), &legendre);
    m_coeff_xz = fitter.coefficients();

    // yz plane //
    for (unsigned int k = 0; k < points.size(); k++) {
        sample_points[k].set_x1(points[k].z());
        sample_points[k].set_x2(points[k].y());
        sample_points[k].set_error(x_and_y_errors[k].y());
    }
    fitter.set_number_of_coefficients(3);
    fitter.fit_parameters(sample_points, 1, sample_points.size(), &legendre);
    m_coeff_yz = fitter.coefficients();

    //////////////////////////////////////////////
    // GET A POINTER TO THE LEGENDRE POLYNOMIAL //
    //////////////////////////////////////////////

    m_Legendre = Legendre_polynomial::get_Legendre_polynomial();

    return;
}
void CurvedLine::setChi2(double chi2) { m_chi2 = std::isnan(chi2) ? -1 : chi2; }
double CurvedLine::chi2() const { return m_chi2; }
void CurvedLine::setNumberOfTrackHits(unsigned int n_hits) { m_numTrkHits = n_hits; }
unsigned int CurvedLine::numberOfTrackHits() const { return m_numTrkHits; }

double CurvedLine::chi2PerDegreesOfFreedom() const { return m_chi2 / (m_numTrkHits > 2 ? m_numTrkHits - 3 : 0.01); }

void CurvedLine::setUsedHits(const MdtHitVec& hits) { m_used_hits = hits; }
const CurvedLine::MdtHitVec& CurvedLine::trackHits() const { return m_used_hits; }