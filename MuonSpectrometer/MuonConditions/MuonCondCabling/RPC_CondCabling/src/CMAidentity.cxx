/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CMAidentity.h"

#include <iomanip>
#include <string>

const char CMAidentity::CoverageTAG[3][5] = {{"even"}, {"odd"}, {""}};

bool CMAidentity::coverage(const std::string& side, CMAcoverage& cov) {
    for (int i = EvenSectors; i <= AllSectors; ++i) {
        std::string tag(CoverageTAG[i]);
        if (side == tag) {
            cov = (CMAcoverage)i;
            return true;
        }
    }
    return false;
}

const std::string CMAidentity::name(const ViewType view, const CMAcoverage coverage) {
    std::string cover(CMAidentity::covtag(coverage));
    std::string side = view == ViewType::Eta ? "Eta" : (view == ViewType::Phi ? "Phi" : "NoView");
    std::string v_name = cover + side + "CMA";
    return v_name;
}
CMAidentity::CMAidentity(defineParams pars) : m_params{pars} {}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, int number, int eta_index, int phi_index, int PAD_index, int Ixx_index) {
    m_params.view = view;
    m_params.coverage = side;
    m_params.seqNumber = number;
    m_params.etaIndex = eta_index;
    m_params.phiIndex = phi_index;
    m_params.padIndex = PAD_index;
    m_params.IxxIndex = Ixx_index;
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, int number) {
    m_params.view = view;
    m_params.coverage = side;
    m_params.seqNumber = number;
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, Offline_indexes& indexes) {
    m_params.view = view;
    m_params.coverage = side;
    m_params.etaIndex = indexes.first;
    m_params.phiIndex = indexes.second;
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, int PAD, int Ixx) {
    m_params.view = view;
    m_params.coverage = side;
    m_params.padIndex = PAD;
    m_params.IxxIndex = Ixx;
}

bool CMAidentity::operator==(const CMAidentity& id) const {
    if (m_params.view == id.type() && m_params.coverage == id.coverage()) {
        if (number() >= 0)
            if (number() == id.number()) return true;

        if (m_params.etaIndex >= 0)
            if (m_params.etaIndex == id.eta_index() && m_params.phiIndex == id.phi_index()) return true;

        if (m_params.padIndex >= 0)
            if (m_params.padIndex == id.PAD_index() && m_params.IxxIndex == id.Ixx_index()) return true;
    }
    return false;
}

bool CMAidentity::operator!=(const CMAidentity& id) const { return !(*this == id); }

bool CMAidentity::operator<(const CMAidentity& id) const {
    if (*this == id) return false;
    if (m_params.view < id.type()) return true;
    if (m_params.coverage < id.coverage()) return true;
    int id1 = m_params.etaIndex * 10 + m_params.phiIndex;
    int id2 = id.eta_index() * 10 + id.phi_index();
    if (id1 < id2) return true;
    return false;
}

CMAidentity& CMAidentity::operator+=(const CMAidentity& id) {
    if (*this != id) return *this;
    if (m_params.seqNumber == -1) m_params.seqNumber = id.number();
    if (m_params.etaIndex == -1) m_params.etaIndex = id.eta_index();
    if (m_params.phiIndex == -1) m_params.phiIndex = id.phi_index();
    if (m_params.padIndex == -1) m_params.padIndex = id.PAD_index();
    if (m_params.IxxIndex == -1) m_params.IxxIndex = id.Ixx_index();
    return *this;
}

void CMAidentity::inversion() { m_params.IxxIndex = (m_params.IxxIndex == 0) ? 1 : 0; }

std::ostream& operator<<(std::ostream& stream, const CMAidentity& id) {
    char exttag[5] = {' ', ' ', ' ', ' ', '\0'};
    const char* tag = CMAidentity::covtag(id.m_params.coverage);
    for (int i = 0; i < 5; ++i) {
        if (tag[i] == '\0') break;
        exttag[i] = tag[i];
    }

    std::string covtag(exttag);

    std::string type = (id.type() == Phi) ? " phi cma " + covtag : " eta cma " + covtag;
    if (covtag != "") type = type + " ";
    stream << type << std::setw(2) << id.number() << ": Eta ";
    stream << std::setw(2) << id.eta_index();
    stream << "  Phi " << std::setw(2) << id.phi_index();
    stream << "  PAD " << std::setw(2) << id.PAD_index();
    stream << "  Ixx " << std::setw(2) << id.Ixx_index();
    return stream;
}
ViewType CMAidentity::type() const { return m_params.view; }
CMAcoverage CMAidentity::coverage() const { return m_params.coverage; }
int CMAidentity::number() const { return m_params.seqNumber; }
int CMAidentity::eta_index() const { return m_params.etaIndex; }
int CMAidentity::phi_index() const { return m_params.phiIndex; }
int CMAidentity::PAD_index() const { return m_params.padIndex; }
int CMAidentity::Ixx_index() const { return m_params.IxxIndex; }
