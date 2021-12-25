/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibRt/RtCalibrationIntegration.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibRt/RtParabolicExtrapolation.h"
#include "MdtCalibT0/T0MTHistos.h"
#include "TF1.h"
#include "TList.h"

using namespace MuonCalib;

inline Double_t slope_function_C(Double_t *x, Double_t *par) {
    Double_t &t(x[0]);
    Double_t &a(par[0]), &b(par[1]), &t_0(par[2]), &back(par[3]);
    return back + std::exp(a + b * (t - t_0));
}

inline void update_parameter_on_mttmax(TH1 *h, TF1 *f, const float &b, const float &T, const T0MTSettingsTMax &tmax_settings) {
    Double_t rmin, rmax;
    f->GetRange(rmin, rmax);
    std::unique_ptr<TF1> slope_function =
        std::make_unique<TF1>("slope_function", slope_function_C, rmin, rmin + tmax_settings.WidthAB(), 4);
    slope_function->SetParameter(0, f->GetParameter(T0MTHistos::TMAX_PAR_NR_A));
    slope_function->FixParameter(1, b);
    slope_function->FixParameter(2, f->GetParameter(T0MTHistos::TMAX_PAR_NR_T0));
    slope_function->FixParameter(3, f->GetParameter(T0MTHistos::TMAX_PAR_NR_BACK));
    h->Fit("slope_function", "R+", "");
    f->FixParameter(T0MTHistos::TMAX_PAR_NR_A, slope_function->GetParameter(0));
    f->FixParameter(T0MTHistos::TMAX_PAR_NR_B, b);
    f->FixParameter(T0MTHistos::TMAX_PAR_NR_T, T);
}
void RtCalibrationIntegration::init(bool close_hits, double r_max, double lower_extrapolation_radius, double upper_extrapolation_radius,
                                    bool add_tmax_difference) {
    m_close_hits = close_hits;
    m_rt = nullptr;
    m_r_max = r_max;
    m_lower_extrapolation_radius = lower_extrapolation_radius;
    m_upper_extrapolation_radius = upper_extrapolation_radius;
    m_output = nullptr;
    m_nb_hits_used = 0;
    m_nb_segments_used = 0;
    m_add_tmax_difference = add_tmax_difference;
    return;
}
unsigned int RtCalibrationIntegration::number_of_hits_used() const { return m_nb_hits_used; }

RtCalibrationIntegration::MdtCalibOutputPtr RtCalibrationIntegration::analyseSegments(const MuonSegVec &seg) {
    for (unsigned int k = 0; k < seg.size(); k++) { handleSegment(*seg[k]); }
    m_nb_segments_used = seg.size();
    analyse();

    return getResults();
}
bool RtCalibrationIntegration::handleSegment(MuonCalibSegment &seg) {
    ///////////////////////////////////////////////////////
    // LOOP OVER THE HITS OF THE SEGMENTS AND STORE THEM //
    ///////////////////////////////////////////////////////

    // start with hits on the segment //
    for (unsigned int k = 0; k < seg.mdtHitsOnTrack(); k++) {
        if (seg.mdtHOT()[k]->driftTime() < -8e8) continue;
        m_t_drift.push_back(std::pair<double, bool>(seg.mdtHOT()[k]->driftTime(), seg.mdtHOT()[k]->identify().mdtMultilayer() == 2));
    }

    // continue with close hits if requested //
    if (m_close_hits == true) {
        for (unsigned int k = 0; k < seg.mdtCloseHits(); k++) {
            m_t_drift.push_back(std::pair<double, bool>(seg.mdtHOT()[k]->driftTime(), seg.mdtHOT()[k]->identify().mdtMultilayer() == 2));
        }
    }

    return true;
}

void RtCalibrationIntegration::setInput(const IMdtCalibrationOutput * /*rt_input*/) { return; }
bool RtCalibrationIntegration::analyse() {
    ///////////////
    // VARIABLES //
    ///////////////

    T0MTSettings t0_setting;  // settings of the MT t0 fitter
    t0_setting.AddFitfun() = true;
    //  t0_setting.DrawDebugGraphs()=true;
    t0_setting.T0Settings()->ScrambleThreshold() = 2;
    t0_setting.T0Settings()->SlicingThreshold() = 3;
    t0_setting.TMaxSettings()->DistAB() += 50;
    T0MTHistos drift_time_spec;  // drift time spectrum used in the t0 and
    // the tmax fit
    std::array<T0MTHistos, 2> drift_time_spec_ml;

    double t0, tmax;  // t0 and tmax
    int k_min(-1);    //, k_max(-1); // first and last drift-time entry to be
    // used in the integration procedure
    unsigned int nb_bins(100);                    // number of integration bins
    double bin_content;                           // number of entries per bin
    std::vector<SamplePoint> point(nb_bins + 1);  // (t, r) points
    double radius(0.0);                           // r(t)
    double scf = 0.;                              // scale factor (r_max/number of used hits)
    RtFromPoints rt_from_points;                  // r-t converter

    ///////////////////////////////////////////////////////////////////////////
    // STOP HERE, IF THERE ARE NOT ENOUGH ENTRIES IN THE DRIFT-TIME SPECTRUM //
    ///////////////////////////////////////////////////////////////////////////

    if (m_t_drift.size() < 2000) {
        MsgStream log(Athena::getMessageSvc(), "RtCalibrationIntegration");
        log << MSG::WARNING << "analyse() - Less than 2000 drift-time entries! No r-t relationship will be determined!" << endmsg;
        return false;
    }

    ////////////////////////
    // INTEGRATION METHOD //
    ////////////////////////

    // sort the hits in ascending order of their drift times //
    sort(m_t_drift.begin(), m_t_drift.end());

    // perform a t0 and tmax fit //
    // fill the histogram with the drift-time spectrum //
    int n_bins = static_cast<int>(32 * (200.0 + m_t_drift[m_t_drift.size() - 1].first - m_t_drift[0].first) / 25.0);
    float min_t = m_t_drift[0].first - 100.0;
    float max_t = m_t_drift[m_t_drift.size() - 1].first + 100.0;
    std::unique_ptr<TH1F> tspec = std::make_unique<TH1F>("tspec", "DRIFT-TIME SPECTRUM", n_bins, min_t, max_t);
    std::array<std::unique_ptr<TH1F>, 2> tspec_ml;
    tspec_ml[0] = std::make_unique<TH1F>("tspec_ml0", "DRIFT-TIME SPECTRUM ML 0", n_bins, min_t, max_t);
    tspec_ml[1] = std::make_unique<TH1F>("tspec_ml1", "DRIFT-TIME SPECTRUM ML 1", n_bins, min_t, max_t);

    for (unsigned int k = 0; k < m_t_drift.size(); k++) {
        tspec->Fill(m_t_drift[k].first, 1.0);
        tspec_ml[static_cast<unsigned int>(m_t_drift[k].second)]->Fill(m_t_drift[k].first, 1.0);
    }
    drift_time_spec.SetTSpec(1, tspec.get(), &t0_setting, false);
    drift_time_spec_ml[0].SetTSpec(2, tspec_ml[0].get(), &t0_setting, false);
    drift_time_spec_ml[1].SetTSpec(3, tspec_ml[1].get(), &t0_setting, false);

    // t0 and tmax fits //

    if (!drift_time_spec.FitT0() || !drift_time_spec.T0Ok()) {
        MsgStream log(Athena::getMessageSvc(), "RtCalibrationIntegration");
        log << MSG::WARNING << "analyse() - t0 fit not successful, no r-t relationship will be calculated!" << endmsg;
        return false;
    }
    t0 = drift_time_spec.GetT0Function()->GetParameter(T0MTHistos::T0_PAR_NR_T0);

    if (!drift_time_spec.FitTmax() || !drift_time_spec.TmaxOk()) {
        MsgStream log(Athena::getMessageSvc(), "RtCalibrationIntegration");
        log << MSG::WARNING << "analyse() - tmax fit not successful, no r-t relationship will be calculated!" << endmsg;
        return false;
    }
    tmax = drift_time_spec.GetTMaxFunction()->GetParameter(T0MTHistos::TMAX_PAR_NR_TMAX) +
           2.0 * drift_time_spec.GetTMaxFunction()->GetParameter(T0MTHistos::TMAX_PAR_NR_T);

    // determine (t,r) points by integration //
    m_nb_hits_used = 0;
    for (unsigned int k = 0; k < m_t_drift.size(); k++) {
        if (m_t_drift[k].first >= t0 && m_t_drift[k].first <= tmax) { m_nb_hits_used++; }
        if (k_min < 0 && m_t_drift[k].first >= t0) { k_min = k; }
    }
    // k_max = k_min+m_nb_hits_used-1;

    bin_content = static_cast<double>(m_nb_hits_used) / static_cast<double>(nb_bins);

    if (m_nb_hits_used > 0) { scf = m_r_max / static_cast<double>(m_nb_hits_used); }

    point[0].set_x1(t0);
    point[0].set_x2(0.0);
    point[0].set_error(0.1);  // give a higher weight to the end point in

    // the final fit
    for (unsigned int k = 1; k < nb_bins; k++) {
        radius = radius + scf * bin_content;
        point[k].set_x1(m_t_drift[k_min + static_cast<int>(bin_content) * (k)].first);
        point[k].set_x2(radius);
        point[k].set_error(1.0);
    }

    point[nb_bins].set_x1(tmax);
    point[nb_bins].set_x2(m_r_max);
    point[nb_bins].set_error(1.);

    // get the r-t relationship //
    m_rt = std::make_shared<RtChebyshev>(rt_from_points.getRtChebyshev(point, 15));

    ///////////////////////////////////////////////////
    // PARABOLIC EXTRAPOLATION FOR LARGE DRIFT RADII //
    ///////////////////////////////////////////////////
    std::vector<SamplePoint> add_fit_point;  // additional r-t points for r(t_max)
    add_fit_point.push_back(SamplePoint(tmax, m_r_max, 1.0));
    RtParabolicExtrapolation rt_extrapolated;
    RtRelationLookUp tmp_rt(rt_extrapolated.getRtWithParabolicExtrapolation(*m_rt, m_lower_extrapolation_radius,
                                                                            m_upper_extrapolation_radius, m_r_max, add_fit_point));
    std::shared_ptr<IRtRelation> rt_new = std::make_shared<RtRelationLookUp>(tmp_rt);

    //////////////////////////////////////////////////
    // Get length difference between multilayers    //
    //////////////////////////////////////////////////

    if (tspec_ml[0]->GetEntries() >= 10000 && tspec_ml[0]->GetEntries() > 10000) {
        bool fit_ok(true);
        std::array<float, 2> b{}, tmax{}, T{};
        for (unsigned int i = 0; i < 2; i++) {
            if (!drift_time_spec_ml[i].FitT0()) {
                fit_ok = false;
                break;
            }
            if (!drift_time_spec_ml[i].FitTmax()) {
                fit_ok = false;
                break;
            }
            b[i] = drift_time_spec_ml[i].GetTMaxFunction()->GetParameter(T0MTHistos::TMAX_PAR_NR_B);
            tmax[i] = drift_time_spec_ml[i].GetTMaxFunction()->GetParameter(T0MTHistos::TMAX_PAR_NR_TMAX) -
                      drift_time_spec_ml[i].GetT0Function()->GetParameter(T0MTHistos::T0_PAR_NR_T0);
            T[i] = drift_time_spec_ml[i].GetTMaxFunction()->GetParameter(T0MTHistos::TMAX_PAR_NR_T);
        }
        if (fit_ok) {
            int refit = static_cast<int>((b[1] + 1.33717e-03) > (b[0] + 1.33717e-03));
            int norefit = static_cast<bool>(refit) ? 0 : 1;
            TF1 *fixfun = drift_time_spec_ml[refit].GetTMaxFunctionNC();
            fixfun->FixParameter(T0MTHistos::TMAX_PAR_NR_B, b[norefit]);
            update_parameter_on_mttmax(drift_time_spec_ml[refit].GetTSpec(), fixfun, b[norefit], T[norefit], *t0_setting.TMaxSettings());
            TList *l = drift_time_spec_ml[refit].GetTSpec()->GetListOfFunctions();
            l->Remove(l->FindObject("mt_tmax_fermi"));
            fit_ok = drift_time_spec_ml[refit].FitTmax();
            tmax[refit] = drift_time_spec_ml[refit].GetTMaxFunction()->GetParameter(T0MTHistos::TMAX_PAR_NR_TMAX) -
                          drift_time_spec_ml[refit].GetT0Function()->GetParameter(T0MTHistos::T0_PAR_NR_T0);
        }

        if (fit_ok && m_add_tmax_difference) rt_new->SetTmaxDiff(tmax[0] - tmax[1]);
    }

    //////////////////////////////////////////////////
    // STORE THE RESULTS IN THE RtCalibrationOutput //
    //////////////////////////////////////////////////

    m_output = std::make_unique<RtCalibrationOutput>(
        rt_new, std::make_shared<RtFullInfo>("RtCalibrationIntegration", 1, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));

    return true;
}

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD converged ::
//::::::::::::::::::::::

bool RtCalibrationIntegration::converged() const { return (m_output != nullptr); }

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD getResults ::
//:::::::::::::::::::::::
RtCalibrationIntegration::MdtCalibOutputPtr RtCalibrationIntegration::getResults() const { return m_output; }
