/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MultilayerRtDifference.h"

// MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"

// MdtCalibFitters
#include "MdtCalibFitters/CurvedPatRec.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"

// MdtCalibData
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/RtScaleFunction.h"

// root
#include "TDirectory.h"
#include "TF1.h"
#include "TF2.h"
#include "TH2F.h"
#include "TProfile.h"

// std
#include "cmath"
#include "iostream"
#include "sstream"
#include "utility"

namespace MuonCalib {

    Double_t MultilayerRtDifference_ScaleFunction(Double_t *x, Double_t *par) { return par[0] * RtScalePolynomial(x[0]); }

    class MultilayerRtDifference_Histograms {
    public:
        inline MultilayerRtDifference_Histograms(TDirectory *control_histogram_dir);
        MultilayerRtDifference_Histograms(const MultilayerRtDifference_Histograms &MLRTDH);
        inline ~MultilayerRtDifference_Histograms();
        MultilayerRtDifference_Histograms &operator=(const MultilayerRtDifference_Histograms &MLRTDH);
        inline TH2F *GetResHist(const int &ml);
        inline TProfile *GetProfileDiff(const int &min_number_of_entries);

    private:
        std::list<TH2F *> m_residuals[2];
        TH2F *m_current_residuals[2]{};
        std::list<TProfile *> m_res_prov[2];
        std::list<TProfile *> m_res_prov_diff;
        TDirectory *m_control_histogram_dir;
    };

    inline MultilayerRtDifference_Histograms::MultilayerRtDifference_Histograms(TDirectory *control_histogram_dir) :
        m_control_histogram_dir(control_histogram_dir) {
        for (int i = 0; i < 2; i++) { m_current_residuals[i] = NULL; }
    }

    // copy constructor to keep Coverity happy
    MultilayerRtDifference_Histograms::MultilayerRtDifference_Histograms(const MultilayerRtDifference_Histograms &MLRTDH) {
        for (int i = 0; i < 2; i++) {
            for (auto res : MLRTDH.m_residuals[i]) { m_residuals[i].push_back(new TH2F(*res)); }
            m_current_residuals[i] = new TH2F(*(MLRTDH.m_current_residuals[i]));
            for (auto res : MLRTDH.m_res_prov[i]) { m_res_prov[i].push_back(new TProfile(*res)); }
        }
        for (auto res : MLRTDH.m_res_prov_diff) { m_res_prov_diff.push_back(new TProfile(*res)); }
        m_control_histogram_dir = MLRTDH.m_control_histogram_dir;  // not a deep copy, hope it is ok
    }

    MultilayerRtDifference_Histograms &MultilayerRtDifference_Histograms::operator=(const MultilayerRtDifference_Histograms &MLRTDH) {
        if (this != &MLRTDH) {
            for (int i = 0; i < 2; i++) {
                m_residuals[i] = MLRTDH.m_residuals[i];
                m_current_residuals[i] = MLRTDH.m_current_residuals[i];
                m_res_prov[i] = MLRTDH.m_res_prov[i];
            }
            m_res_prov_diff = MLRTDH.m_res_prov_diff;
            m_control_histogram_dir = MLRTDH.m_control_histogram_dir;
        }
        return *this;
    }

    template <typename T> inline void clearall(T &container) {
        typename T::iterator it = container.begin();
        for (; it != container.end(); it++) {
            delete *it;
            *it = NULL;
        }
    }

    inline MultilayerRtDifference_Histograms::~MultilayerRtDifference_Histograms() {
        for (unsigned int i = 0; i < 2; i++) {
            clearall(m_residuals[i]);
            clearall(m_res_prov[i]);
        }
        clearall(m_res_prov_diff);
    }

    inline TH2F *MultilayerRtDifference_Histograms::GetResHist(const int &ml) {
        if (!m_current_residuals[ml]) {
            for (int i = 0; i < 2; i++) {
                TDirectory *prevdir = gDirectory;
                if (m_control_histogram_dir) m_control_histogram_dir->cd();
                std::ostringstream nm;
                nm << "temporal_residual_ml" << i << "_iteration" << m_residuals[i].size();
                m_current_residuals[i] = new TH2F(nm.str().c_str(), "", 15, 0., 15., 200, -100, 100);
                if (m_control_histogram_dir)
                    prevdir->cd();
                else
                    m_current_residuals[i]->SetDirectory(0);
                m_residuals[i].push_back(m_current_residuals[i]);
            }
        }
        return m_current_residuals[ml];
    }

    inline TProfile *MultilayerRtDifference_Histograms::GetProfileDiff(const int &min_number_of_entries) {
        TDirectory *prev = gDirectory;
        if (m_control_histogram_dir) { m_control_histogram_dir->cd(); }
        bool ok(true);
        TProfile *prof[2];
        for (unsigned int i = 0; i < 2; i++) {
            if (m_current_residuals[i] && m_current_residuals[i]->GetEntries() >= min_number_of_entries) {
                prof[i] = m_current_residuals[i]->ProfileX();
                if (!m_control_histogram_dir) { prof[i]->SetDirectory(0); }
            } else {
                prof[i] = NULL;
                ok = false;
            }
            m_res_prov[i].push_back(prof[i]);
            m_current_residuals[i] = NULL;
        }
        if (!ok) {
            prev->cd();
            m_res_prov_diff.push_back(NULL);
            return NULL;
        }
        std::ostringstream nm;
        nm << "res_prov_diff_step" << m_res_prov_diff.size();
        TProfile *prov_diff = new TProfile(nm.str().c_str(), "", 15, 0., 15.);
        prov_diff->Add(prof[0], prof[1], 1., -1);
        m_res_prov_diff.push_back(prov_diff);
        if (!m_control_histogram_dir) { prov_diff->SetDirectory(0); }
        return prov_diff;
    }

    MultilayerRtDifference::MultilayerRtDifference(int min_hits, TDirectory *control_histogram_dir) :
        m_histograms(new MultilayerRtDifference_Histograms(control_histogram_dir)), m_min_number_of_hits(min_hits) {
        m_polfun = new TF1("polfun", MultilayerRtDifference_ScaleFunction, 4.0, 15.0, 1);
    }

    // Copy constructor, to keep Coverity happy
    MultilayerRtDifference::MultilayerRtDifference(const MultilayerRtDifference &MLRTD) : m_min_number_of_hits(MLRTD.m_min_number_of_hits) {
        m_polfun = new TF1(*MLRTD.m_polfun);
        m_histograms = new MultilayerRtDifference_Histograms(*MLRTD.m_histograms);
    }

    // Assignment operator, for Coverity happiness
    MultilayerRtDifference &MultilayerRtDifference::operator=(const MultilayerRtDifference &MLRTD) {
        if (this != &MLRTD) {
            m_min_number_of_hits = MLRTD.m_min_number_of_hits;
            delete m_polfun;
            m_polfun = new TF1(*MLRTD.m_polfun);
            delete m_histograms;
            m_histograms = new MultilayerRtDifference_Histograms(*MLRTD.m_histograms);
        }
        return *this;
    }

    MultilayerRtDifference::~MultilayerRtDifference() {
        delete m_histograms;
        delete m_polfun;
    }

    void MultilayerRtDifference::Fill(const MdtCalibHitBase &hit, const IRtRelation &rt_relation) {
        int ml = hit.identify().mdtMultilayer() - 1;
        double r_track = std::abs(hit.signedDistanceToTrack());
        double res = std::abs(hit.driftRadius()) - r_track;
        double v = rt_relation.driftvelocity(hit.driftTime());
        m_histograms->GetResHist(ml)->Fill(r_track, res / v);
    }

    bool MultilayerRtDifference::DoFit(IRtRelation *rt_relation, const IMdtCalibration::MuonSegVec &seg) {
        TProfile *prov_diff = m_histograms->GetProfileDiff(m_min_number_of_hits);
        if (!prov_diff) {
            MsgStream log(Athena::getMessageSvc(), "MultilayerRtDifference");
            log << MSG::WARNING << "MultilayerRtDifference::DoFit: Not enough hits!" << endmsg;
            return false;
        }
        if (prov_diff->Fit("polfun", "Q", "", 4., 15.) != 0) {
            MsgStream log(Athena::getMessageSvc(), "MultilayerRtDifference");
            log << MSG::WARNING << "MultilayerRtDifference: Fit of polinomial failed! Not updating scale!" << endmsg;
            return false;
        }
        if (std::abs(m_polfun->GetParameter(0)) < m_polfun->GetParError(0)) {
            MsgStream log(Athena::getMessageSvc(), "MultilayerRtDifference");
            log << MSG::WARNING << "MultilayerRtDifference: No Scale update needed! Scale correction: " << m_polfun->GetParameter(0)
                << " +- " << m_polfun->GetParError(0) << endmsg;
            return true;
        }
        if (!rt_relation) return true;
        float scale = m_polfun->GetParameter(0);
        if (std::abs(scale) > 2) scale *= 4;
        if (rt_relation->HasTmaxDiff()) { scale += rt_relation->GetTmaxDiff(); }
        rt_relation->SetTmaxDiff(scale);
        if (seg.empty()) return true;
        // update input segments
        for (const auto &segment : seg) {
            for (const MuonCalibSegment::MdtHitPtr &hit : segment->mdtHOT()) {
                float old_corr = hit->TemperatureTime();
                float corr = RtScaleFunction(hit->driftTime(), hit->identify().mdtMultilayer() == 2, *rt_relation);
                hit->setTemperatureTime(corr);
                hit->setDriftTime(hit->driftTime() - corr + old_corr);
                hit->setDriftRadius(rt_relation->radius(hit->driftTime()), hit->sigmaDriftRadius());
            }
        }
        return true;
    }

}  // namespace MuonCalib
