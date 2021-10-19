/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/TrRelation.h"

#include <cmath>
#include <iostream>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

using namespace MuonCalib;

//*****************************************************************************

//::::::::::::::::::::::::::::::::::::::::
//:: CONSTRUCTOR (const IRtRelation & ) ::
//::::::::::::::::::::::::::::::::::::::::
TrRelation::TrRelation(const IRtRelation &input_rt) {
    ///////////////
    // VARIABLES //
    ///////////////
    unsigned int nb_points(100);  // number of r-t points used internally
    // step size in r
    double step_size(input_rt.radius(input_rt.tUpper()) / static_cast<double>(nb_points - 1));
    m_r = std::vector<double>(nb_points);
    m_t = std::vector<double>(nb_points);

    ///////////////////////////
    // FILL THE (r, t) PAIRS //
    ///////////////////////////
    for (unsigned int k = 0; k < nb_points; k++) {
        m_r[k] = k * step_size;
        m_t[k] = getTFromR(m_r[k], input_rt);
    }

}  // end TrRelation::TrRelation

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: CONSTRUCTOR (const std::vector<double> & , const std::vector<double> &) ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TrRelation::TrRelation(const std::vector<double> &r_values, const std::vector<double> &t_values) {
    m_r = r_values;
    m_t = t_values;
}

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD tFromR ::
//:::::::::::::::::::
double TrRelation::tFromR(const double &r, bool &out_of_bound_flag) const {
    //////////////////
    // CHECK BOUNDS //
    //////////////////
    if (r < m_r[0]) {
        out_of_bound_flag = true;
        return m_t[0];
    }
    if (r > m_r[m_r.size() - 1]) {
        out_of_bound_flag = true;
        return m_t[m_r.size() - 1];
    }

    ///////////////////
    // INTERPOLATION //
    ///////////////////
    for (unsigned int k = 0; k < m_r.size() - 1; k++) {
        if (r >= m_r[k] && r <= m_r[k + 1]) {
            if (m_r[k] == m_r[k + 1]) {
                out_of_bound_flag = false;
                return m_t[k];
            }
            out_of_bound_flag = false;
            return m_t[k] + (r - m_r[k]) * (m_t[k + 1] - m_t[k]) / (m_r[k + 1] - m_r[k]);
        }
    }
    MsgStream log(Athena::getMessageSvc(), "TrRelation");
    log << MSG::ERROR << "Class TrRelation, method tFromR: ERROR!" << endmsg;
    return 0.0;
}  // end TrRelation::tFromR

//*****************************************************************************

//:::::::::::::::::::
//:: METHOD rFromT ::
//:::::::::::::::::::
double TrRelation::rFromT(const double &t, bool &out_of_bound_flag) const {
    //////////////////
    // CHECK BOUNDS //
    //////////////////
    if (t < m_t[0]) {
        out_of_bound_flag = true;
        return m_r[0];
    }
    if (t > m_t[m_r.size() - 1]) {
        out_of_bound_flag = true;
        return m_r[m_r.size() - 1];
    }

    ///////////////////
    // INTERPOLATION //
    ///////////////////
    for (unsigned int k = 0; k < m_r.size() - 1; k++) {
        if (t >= m_t[k] && t <= m_t[k + 1]) {
            if (m_t[k] == m_t[k + 1]) {
                out_of_bound_flag = false;
                return m_r[k];
            }
            out_of_bound_flag = false;
            return m_r[k] + (t - m_t[k]) * (m_r[k + 1] - m_r[k]) / (m_t[k + 1] - m_t[k]);
        }
    }
    MsgStream log(Athena::getMessageSvc(), "TrRelation");
    log << MSG::ERROR << "Class TrRelation, method rFromT: ERROR!" << endmsg;
    return 0.0;
}  // end TrRelation::rFromT

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD getTFromR ::
//::::::::::::::::::::::
double TrRelation::getTFromR(const double &r, const IRtRelation &input_rt) {
    ///////////////
    // VARIABLES //
    ///////////////
    double precision(0.001);          // spatial precision of the inversion
    double t_max(input_rt.tUpper());  // upper time search limit
    double t_min(input_rt.tLower());  // lower time search limit

    /////////////////////////////////////////////
    // SEARCH FOR THE CORRESPONDING DRIFT TIME //
    /////////////////////////////////////////////
    while (t_max - t_min > 0.1 && std::abs(input_rt.radius(0.5 * (t_min + t_max)) - r) > precision) {
        if (input_rt.radius(0.5 * (t_min + t_max)) > r) {
            t_max = 0.5 * (t_min + t_max);
        } else {
            t_min = 0.5 * (t_min + t_max);
        }
    }

    return 0.5 * (t_min + t_max);
}  // end TrRelation::getTFromR
