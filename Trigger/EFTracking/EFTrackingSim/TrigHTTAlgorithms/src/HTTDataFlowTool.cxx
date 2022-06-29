// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "HTTDataFlowTool.h"

#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTDataFlowInfo.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTFunctions.h"

#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "CxxUtils/bitscan.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
HTTDataFlowTool::HTTDataFlowTool(std::string const & algname, std::string const & name, IInterface const * ifc) :
    AthAlgTool(algname, name, ifc)
{
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::initialize()
{
    ATH_MSG_INFO("HTTDataFlowTool::initialize()");

    ATH_CHECK(m_tHistSvc.retrieve());
    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_evtSel.retrieve());

    m_nLayers_1st = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();

    m_dataFlowTxt.open(m_dataFlowTxtName);
    m_dataFlowTeX.open(m_dataFlowTeXName);

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::calculateDataFlow(HTTDataFlowInfo* info, HTTLogicalEventInputHeader const * header_1st,
                                              std::vector<HTTCluster> const & clusters_1st,
                                              std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack> const & tracks_1st,
                                              std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack> const & tracks_2nd)
{
    info->nMappedHits_1st_layer.resize(m_nLayers_1st);
    for (HTTHit const & hit : header_1st->towers().at(0).hits()) {
        info->nMappedHits_1st_layer[hit.getLayer()]++;
        info->nMappedHits_1st_total++;
    }

    info->nClusters_1st_layer.resize(m_nLayers_1st);
    for (HTTCluster const & cluster : clusters_1st) {
        info->nClusters_1st_layer[cluster.getClusterEquiv().getLayer()]++;
        info->nClusters_1st_total = clusters_1st.size();
    }

    info->nRoads_1st_total = roads_1st.size();
    for (HTTRoad const * r : roads_1st) {
        if (CxxUtils::count_ones(r->getHitLayers()) == m_nLayers_1st - 1) info->nRoads_1st_7hits++;
        if (CxxUtils::count_ones(r->getHitLayers()) == m_nLayers_1st)     info->nRoads_1st_8hits++;
    }

    std::unordered_map<int, size_t> pattID_nTracks_1st;
    std::unordered_map<int, size_t> pattID_nTracks_1st_passChi2;
    std::unordered_map<int, size_t> pattID_nTracks_1st_afterOR;

    std::unordered_map<int, size_t> sectID_nTracks_1st;
    std::unordered_map<int, size_t> sectID_nTracks_1st_passChi2;
    std::unordered_map<int, size_t> sectID_nTracks_1st_afterOR;

    info->nTracks_1st_total = tracks_1st.size();
    for (HTTTrack const & t : tracks_1st) {
        if (t.getChi2ndof() <= m_cut_chi2ndof) {
            info->nTracks_1st_passChi2++;
            pattID_nTracks_1st_passChi2[t.getPatternID()]++;
            sectID_nTracks_1st_passChi2[t.getFirstSectorID()]++;
        }
        if (t.passedOR() == 1) {
            info->nTracks_1st_afterOR++;
            pattID_nTracks_1st_afterOR[t.getPatternID()]++;
            sectID_nTracks_1st_afterOR[t.getFirstSectorID()]++;
        }
        pattID_nTracks_1st[t.getPatternID()]++;
        sectID_nTracks_1st[t.getFirstSectorID()]++;
    }
    for (const auto& [ID, ntrack]: pattID_nTracks_1st) {
      info->nTracks_1st_per_patt_total.push_back(ntrack);
    }
    for (const auto& [ID, ntrack]: pattID_nTracks_1st_passChi2) {
      info->nTracks_1st_per_patt_passChi2.push_back(ntrack);
    }
    for (const auto& [ID, ntrack]: pattID_nTracks_1st_afterOR) {
      info->nTracks_1st_per_patt_afterOR.push_back(ntrack);
    }

    info->nConstants_1st_fitter = sectID_nTracks_1st.size();
    info->nConstants_1st_passChi2 = sectID_nTracks_1st_passChi2.size();
    info->nConstants_1st_afterOR = sectID_nTracks_1st_afterOR.size();

    if (m_runSecondStage) {
        info->nRoads_2nd_total = roads_2nd.size();

        std::unordered_map<int, size_t> sectID_nTracks_2nd;
        std::unordered_map<int, size_t> sectID_nTracks_2nd_passChi2;
        std::unordered_map<int, size_t> sectID_nTracks_2nd_afterOR;

        info->nTracks_2nd_total = tracks_2nd.size();
        for (HTTTrack const & t : tracks_2nd) {
            if (t.getChi2ndof() <= m_cut_chi2ndof) {
                info->nTracks_2nd_passChi2++;
                sectID_nTracks_2nd_passChi2[t.getSecondSectorID()]++;
            }
            if (t.passedOR() == 1) {
                info->nTracks_2nd_afterOR++;
                sectID_nTracks_2nd_afterOR[t.getSecondSectorID()]++;
            }
            sectID_nTracks_2nd[t.getSecondSectorID()]++;
        }

        info->nConstants_2nd_fitter = sectID_nTracks_2nd.size();
        info->nConstants_2nd_passChi2 = sectID_nTracks_2nd_passChi2.size();
        info->nConstants_2nd_afterOR = sectID_nTracks_2nd_afterOR.size();
    }

    for (HTTTruthTrack const & truth_t : header_1st->optional().getTruthTracks()) {
        if (m_evtSel->passCuts(truth_t)) info->nTruthTracks++;
    }

    for (HTTOfflineTrack const & offline_t : header_1st->optional().getOfflineTracks()) {
        if (m_evtSel->passCuts(offline_t)) info->nOfflineTracks++;
    }

    if (info->nOfflineTracks > 0) {
        info->nTracks_1st_over_nOfflineTracks = (float)tracks_1st.size() / (float)info->nOfflineTracks;
        if (m_runSecondStage) info->nTracks_2nd_over_nOfflineTracks = (float)tracks_2nd.size() / (float)info->nOfflineTracks;
    }

    ATH_CHECK(getDataFlowInfo(*info));

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::getDataFlowInfo(HTTDataFlowInfo const & info)
{
    m_nEvents++;

    for (size_t i = 0; i < m_nLayers_1st; i++) {
        ATH_CHECK(addDataFlow(info.nMappedHits_1st_layer[i], "nMappedHitsL" + std::to_string(i) + "_1st"));
    }
    ATH_CHECK(addDataFlow(info.nMappedHits_1st_total, "nMappedHits_1st per layer")); // We fill the total number here. It will be divided by the number of layers later.

    for (size_t i = 0; i < m_nLayers_1st; i++) {
        ATH_CHECK(addDataFlow(info.nClusters_1st_layer[i], "nClustersL" + std::to_string(i) + "_1st"));
    }
    ATH_CHECK(addDataFlow(info.nClusters_1st_total, "nClusters_1st per layer")); // We fill the total number here. It will be divided by the number of layers later.

    ATH_CHECK(addDataFlow(info.nRoads_1st_total, "nRoads_1st(Total)"));
    ATH_CHECK(addDataFlow(info.nRoads_1st_7hits, "nRoads_1st(7/8)"));
    ATH_CHECK(addDataFlow(info.nRoads_1st_8hits, "nRoads_1st(8/8)"));

    ATH_CHECK(addDataFlow(info.nTracks_1st_total,    "nTracks_1st(Total)"));
    ATH_CHECK(addDataFlow(info.nTracks_1st_passChi2, "nTracks_1st(Chi2ndof<40)"));
    ATH_CHECK(addDataFlow(info.nTracks_1st_afterOR,  "nTracks_1st(passing OR)"));

    for (size_t n : info.nTracks_1st_per_patt_total) {
        ATH_CHECK(addDataFlow(n, "nTracks_1st per patt(Total)"));
    }
    for (size_t n : info.nTracks_1st_per_patt_passChi2) {
        ATH_CHECK(addDataFlow(n, "nTracks_1st per patt(Chi2ndof<40)"));
    }
    for (size_t n : info.nTracks_1st_per_patt_afterOR) {
        ATH_CHECK(addDataFlow(n, "nTracks_1st per patt(passing OR)"));
    }

    ATH_CHECK(addDataFlow(info.nConstants_1st_fitter,   "nConstants_1st(Fitter)"));
    ATH_CHECK(addDataFlow(info.nConstants_1st_passChi2, "nConstants_1st(Chi2ndof<40)"));
    ATH_CHECK(addDataFlow(info.nConstants_1st_afterOR,  "nConstants_1st(after OR)"));

    if (m_runSecondStage) {
        ATH_CHECK(addDataFlow(info.nRoads_2nd_total, "nRoads_2nd(Total)"));

        ATH_CHECK(addDataFlow(info.nTracks_2nd_total,    "nTracks_2nd(Total)"));
        ATH_CHECK(addDataFlow(info.nTracks_2nd_passChi2, "nTracks_2nd(Chi2ndof<40)"));
        ATH_CHECK(addDataFlow(info.nTracks_2nd_afterOR,  "nTracks_2nd(passing OR)"));

        ATH_CHECK(addDataFlow(info.nConstants_2nd_extrapolate, "nConstants_2nd(Extrapolate)"));
        ATH_CHECK(addDataFlow(info.nConstants_2nd_fitter,      "nConstants_2nd(Fitter)"));
        ATH_CHECK(addDataFlow(info.nConstants_2nd_passChi2,    "nConstants_2nd(Chi2ndof<40)"));
        ATH_CHECK(addDataFlow(info.nConstants_2nd_afterOR,      "nConstants_2nd(after OR)"));
    }

    ATH_CHECK(addDataFlow(info.nTruthTracks,   "nTruthTracks"));
    ATH_CHECK(addDataFlow(info.nOfflineTracks, "nOfflineTracks"));

    if (info.nOfflineTracks > 0) {
        ATH_CHECK(addDataFlow(info.nTracks_1st_over_nOfflineTracks, "nTracks_1st(Total)/nOfflineTracks", false));
        if (m_runSecondStage) {
            ATH_CHECK(addDataFlow(info.nTracks_2nd_over_nOfflineTracks, "nTracks_2nd(Total)/nOfflineTracks", false));
        }
    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::makeDataFlowTable()
{
    m_dataFlowTxt << std::left << std::setw(m_tableTypeWidth) << "Type"
                               << std::setw(m_tableDataWidth) << "Stage"
                               << std::setw(m_tableDataWidth) << "Mean"
                               << std::setw(m_tableDataWidth) << "RMS"
                               << std::setw(m_tableDataWidth) << "RMS95"
                               << std::setw(m_tableDataWidth) << "Min"
                               << std::setw(m_tableDataWidth) << "Max" << std::endl;

    m_dataFlowTeX << "\\documentclass[12pt]{article}" << std::endl;
    m_dataFlowTeX << "\\begin{document}" << std::endl;
    m_dataFlowTeX << "\\begin{table}" << std::endl;
    m_dataFlowTeX << "\\makebox[\\linewidth]{" << std::endl;
    m_dataFlowTeX << "\\begin{tabular}{|c|c|c|c|c|c|c|}" << std::endl;
    m_dataFlowTeX << "\\hline" << std::endl;
    m_dataFlowTeX << "Type & Stage & Mean & RMS & RMS95 & Min & Max \\\\" << std::endl;

    addTableBreak();

    for (size_t i = 0; i < m_nLayers_1st; i++) {
        ATH_CHECK(printDataFlow("nMappedHitsL" + std::to_string(i) + "_1st"));
    }
    ATH_CHECK(printDataFlow("nMappedHits_1st per layer", m_nLayers_1st));

    addTableBreak();

    for (size_t i = 0; i < m_nLayers_1st; i++) {
        ATH_CHECK(printDataFlow("nClustersL" + std::to_string(i) + "_1st"));
    }
    ATH_CHECK(printDataFlow("nClusters_1st per layer", m_nLayers_1st));

    addTableBreak();

    ATH_CHECK(printDataFlow("nRoads_1st(Total)"));
    ATH_CHECK(printDataFlow("nRoads_1st(7/8)"));
    ATH_CHECK(printDataFlow("nRoads_1st(8/8)"));

    addTableBreak();

    ATH_CHECK(printDataFlow("nTracks_1st(Total)"));
    ATH_CHECK(printDataFlow("nTracks_1st(Chi2ndof<40)"));
    ATH_CHECK(printDataFlow("nTracks_1st(passing OR)"));

    addTableBreak();

    ATH_CHECK(printDataFlow("nTracks_1st per patt(Total)"));
    ATH_CHECK(printDataFlow("nTracks_1st per patt(Chi2ndof<40)"));
    ATH_CHECK(printDataFlow("nTracks_1st per patt(passing OR)"));

    addTableBreak();

    ATH_CHECK(printDataFlow("nConstants_1st(Fitter)"));
    ATH_CHECK(printDataFlow("nConstants_1st(Chi2ndof<40)"));
    ATH_CHECK(printDataFlow("nConstants_1st(after OR)"));

    addTableBreak();

    if (m_runSecondStage) {
        ATH_CHECK(printDataFlow("nRoads_2nd(Total)"));

        addTableBreak();

        ATH_CHECK(printDataFlow("nTracks_2nd(Total)"));
        ATH_CHECK(printDataFlow("nTracks_2nd(Chi2ndof<40)"));
        ATH_CHECK(printDataFlow("nTracks_2nd(passing OR)"));

        addTableBreak();

        ATH_CHECK(printDataFlow("nConstants_2nd(Extrapolate)"));
        ATH_CHECK(printDataFlow("nConstants_2nd(Fitter)"));
        ATH_CHECK(printDataFlow("nConstants_2nd(Chi2ndof<40)"));
        ATH_CHECK(printDataFlow("nConstants_2nd(after OR)"));

        addTableBreak();
    }

    ATH_CHECK(printDataFlow("nTruthTracks"));
    ATH_CHECK(printDataFlow("nOfflineTracks"));
    ATH_CHECK(printDataFlow("nTracks_1st(Total)/nOfflineTracks"));
    if (m_runSecondStage) ATH_CHECK(printDataFlow("nTracks_2nd(Total)/nOfflineTracks"));

    std::string str_sample;
    std::string str_PU;

    switch (m_evtSel->getSampleType()) {
        case SampleType::singleElectrons : str_sample = " single electron"; break;
        case SampleType::singleMuons     : str_sample = " single muon";     break;
        case SampleType::singlePions     : str_sample = " single pion";     break;
        default                          : str_sample = "";                 break;
    }

    str_PU = m_evtSel->checkPU() ? " with pile-up" : "";

    m_dataFlowTxt << "--------------" << std::endl;
    m_dataFlowTxt << "Data flow for " << m_nEvents << str_sample << " events" << str_PU << ". "
                  << "(" << roundTo(100. * m_max_frac, m_nSigDigits) << "%" << " acceptance)" << std::endl;

    m_dataFlowTeX << "\\hline" << std::endl;
    m_dataFlowTeX << "\\end{tabular}}" << std::endl;
    m_dataFlowTeX << "\\caption{Data flow for " << m_nEvents << str_sample << " events" << str_PU << ". "
                  << "(" << roundTo(100. * m_max_frac, m_nSigDigits) << "\\%" << " acceptance)} " << std::endl;
    m_dataFlowTeX << "\\end{table}" << std::endl;
    m_dataFlowTeX << "\\end{document}" << std::endl;

    m_dataFlowTxt.close();
    m_dataFlowTeX.close();

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::finalize()
{
    ATH_MSG_INFO("HTTDataFlowTool::finalize()");
    ATH_CHECK(makeDataFlowTable());
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::addDataFlow(float const n, std::string const & key, bool const isInt)
{
    if (isInt) {
        if (m_dataFlowDataI_min.find(key) == m_dataFlowDataI_min.end()) {
            m_dataFlowDataI_min.insert({key, n});
        }
        else if (n < m_dataFlowDataI_min.find(key)->second) {
            m_dataFlowDataI_min.find(key)->second = n;
        }

        if (m_dataFlowDataI_max.find(key) == m_dataFlowDataI_max.end()) {
            m_dataFlowDataI_max.insert({key, n});
        }
        else if (n > m_dataFlowDataI_max.find(key)->second) {
            m_dataFlowDataI_max.find(key)->second = n;
        }
    }
    else {
        if (m_dataFlowDataF_min.find(key) == m_dataFlowDataF_min.end()) {
            m_dataFlowDataF_min.insert({key, n});
        }
        else if (n < m_dataFlowDataF_min.find(key)->second) {
            m_dataFlowDataF_min.find(key)->second = n;
        }

        if (m_dataFlowDataF_max.find(key) == m_dataFlowDataF_max.end()) {
            m_dataFlowDataF_max.insert({key, n});
        }
        else if (n > m_dataFlowDataF_max.find(key)->second) {
            m_dataFlowDataF_max.find(key)->second = n;
        }
    }
    
    if ((isInt && m_dataFlowHistsI.find(key) == m_dataFlowHistsI.end()) ||
        (!isInt && m_dataFlowHistsF.find(key) == m_dataFlowHistsF.end())) {
        std::string hname = "h_dataflow_" + key;
        if (!m_outputtag.value().empty()) hname += ("_" + m_outputtag.value());
        findAndReplaceAll(hname, "/", "_over_");

        setHistDir("/DataFlowHist/");
        TH1* h;
        if (isInt)
            h = new TH1I(hname.c_str(), key.c_str(), n + 1, -0.5, n + 0.5);
        else
            h = new TH1F(hname.c_str(), key.c_str(), (round(n) + 1) * 100, -0.5, round(n) + 0.5);
        ATH_CHECK(regHist(getHistDir(), h));
        clearHistDir();

        h->Fill(n);

        if (isInt)
            m_dataFlowHistsI.insert({key, dynamic_cast<TH1I*>(h)});
        else
            m_dataFlowHistsF.insert({key, dynamic_cast<TH1F*>(h)});
    }
    else {
        if (m_dataFlowDataI_max.find(key) != m_dataFlowDataI_max.end()) {
            int max = m_dataFlowDataI_max.find(key)->second;
            TH1I* h = m_dataFlowHistsI.find(key)->second;
            h->SetBins(max + 1, -0.5, max + 0.5);
            h->Fill(n);
        }
        else if (m_dataFlowDataF_max.find(key) != m_dataFlowDataF_max.end()) {
            float max = m_dataFlowDataF_max.find(key)->second;
            TH1F* h = m_dataFlowHistsF.find(key)->second;
            h->SetBins((round(max) + 1) * 100, -0.5, round(max) + 0.5);
            h->Fill(n);
        }
    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTDataFlowTool::printDataFlow(std::string const & key, int const div)
{
    bool isInt = false;
    if (m_dataFlowHistsI.find(key) != m_dataFlowHistsI.end())
        isInt = true;
    else if (m_dataFlowHistsF.find(key) == m_dataFlowHistsF.end())
        return StatusCode::SUCCESS;

    double min = isInt ? m_dataFlowDataI_min.find(key)->second : m_dataFlowDataF_min.find(key)->second;
    double max = isInt ? m_dataFlowDataI_max.find(key)->second : m_dataFlowDataF_max.find(key)->second;

    TH1* h;
    if (isInt)
        h = m_dataFlowHistsI.find(key)->second;
    else
        h = m_dataFlowHistsF.find(key)->second;

    setMaxAcceptance(h, m_max_frac, max);

    std::string str_stage = "", str_key = key;
    if (key.find("_1st") != std::string::npos) {
        str_stage = "FIRST";
        str_key.erase(key.find("_1st"), std::string("_1st").length());
    }
    else if (key.find("_2nd") != std::string::npos) {
        str_stage = "SECOND";
        str_key.erase(key.find("_2nd"), std::string("_2nd").length());
    }
    else {
      ATH_MSG_WARNING("Did not find \"_1st\" or \"_2nd\" in the key!");
      str_stage = "UNKNOWN";
    }

    double mean    = h->GetMean() / div;
    double rms     = h->GetRMS()  / div;
    double rms95_n = rms95(h)     / div;
    min = min / div;
    max = max / div;

    m_dataFlowTxt << std::left << std::setw(m_tableTypeWidth) << str_key
                               << std::setw(m_tableDataWidth) << str_stage
                               << std::setw(m_tableDataWidth) << roundTo(mean,    m_nSigDigits)
                               << std::setw(m_tableDataWidth) << roundTo(rms,     m_nSigDigits)
                               << std::setw(m_tableDataWidth) << roundTo(rms95_n, m_nSigDigits)
                               << std::setw(m_tableDataWidth) << roundTo(min,     m_nSigDigits)
                               << std::setw(m_tableDataWidth) << roundTo(max,     m_nSigDigits) << std::endl;

    // Replace all "<" by "$<$" in the key to make the type text for tex.
    // If there are other characters in the future, we can make a vector to store all of them.
    std::string key_tex = str_key;
    findAndReplaceAll(key_tex, "<", "$<$");

    m_dataFlowTeX << key_tex << " & " << str_stage << " & " << roundTo(mean,     m_nSigDigits)
                                                   << " & " << roundTo(rms,      m_nSigDigits)
                                                   << " & " << roundTo(rms95(h), m_nSigDigits)
                                                   << " & " << roundTo(min,      m_nSigDigits)
                                                   << " & " << roundTo(max,      m_nSigDigits) << " \\\\" << std::endl;

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTDataFlowTool::addTableBreak(unsigned const n)
{
    for (unsigned i = 0; i < n; i++) {
        m_dataFlowTxt << "--------------" << std::endl;
        m_dataFlowTeX << "\\hline" << std::endl;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTDataFlowTool::findAndReplaceAll(std::string & data, std::string const & toFind, std::string const & replaceStr) const
{
    size_t pos = data.find(toFind);
    while (pos != std::string::npos) {
        data.replace(pos, toFind.size(), replaceStr);
        pos = data.find(toFind, pos + replaceStr.size());
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void HTTDataFlowTool::setMaxAcceptance(TH1* h, double const max_frac, double& max_value) const
{
    double total = h->Integral();
    for (int i = 1; i <= h->GetNbinsX(); i++) {
        double partial = h->Integral(1, i);
        if (partial / total > max_frac) {
            max_value = h->GetBinCenter(i);
            h->GetXaxis()->SetRange(1, i);
            break;
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
double HTTDataFlowTool::roundTo(double const v, int const nSigDigits) const
{
    int sigDigit  = v != 0 ? trunc(log10(fabs(v))) : 0;
    if (nSigDigits >= 1) {
        double factor = pow(10., nSigDigits - sigDigit - 1);
        return round(v * factor) / factor;
    }

    return v;
}

