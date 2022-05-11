/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <MuonTesterTree/FourVectorBranch.h>

PtEtaPhiEBranch::PtEtaPhiEBranch(MuonTesterTree& tree, const std::string& vec_name) : MuonTesterBranch(tree, vec_name) {}
void PtEtaPhiEBranch::push_back(const float pt, const float eta, const float phi, const float e) {
    m_pt += pt;
    m_eta += eta;
    m_phi += phi;
    m_e += e;
}
void PtEtaPhiEBranch::set(const float pt, const float eta, const float phi, const float e, size_t pos) {
    m_pt[pos] = pt;
    m_eta[pos] = eta;
    m_phi[pos] = phi;
    m_e[pos] = e;
}
void PtEtaPhiEBranch::push_back(const TLorentzVector& vec) { push_back(vec.Pt(), vec.Eta(), vec.Phi(), vec.E()); }
void PtEtaPhiEBranch::operator+=(const TLorentzVector& vec) { push_back(vec); }
void PtEtaPhiEBranch::set(const TLorentzVector& vec, size_t pos) { set(vec.Pt(), vec.Eta(), vec.Phi(), vec.E(), pos); }
void PtEtaPhiEBranch::push_back(const HepMC::FourVector& vec) { push_back(vec.perp(), vec.eta(), vec.phi(), vec.e()); }
void PtEtaPhiEBranch::set(const HepMC::FourVector& vec, size_t pos) { set(vec.perp(), vec.eta(), vec.phi(), vec.e(), pos); }
void PtEtaPhiEBranch::operator+=(const HepMC::FourVector& vec) { push_back(vec); }
size_t PtEtaPhiEBranch::size() const { return m_e.size(); }
bool PtEtaPhiEBranch::fill(const EventContext&) { return true; }
bool PtEtaPhiEBranch::init() { return true; }

CartesFourVecBranch::CartesFourVecBranch(MuonTesterTree& tree, const std::string& vec_name, Type t) :
    MuonTesterBranch(tree, vec_name), m_mom{t == Type::Mom} {}
void CartesFourVecBranch::push_back(const TLorentzVector& vec) { push_back(vec.X(), vec.Y(), vec.Z(), vec.T()); }
void CartesFourVecBranch::operator+=(const TLorentzVector& vec) { push_back(vec); }
void CartesFourVecBranch::set(const TLorentzVector& vec, size_t pos) { set(vec.X(), vec.Y(), vec.Z(), vec.T(), pos); }
void CartesFourVecBranch::push_back(const HepMC::FourVector& vec) { push_back(vec.x(), vec.y(), vec.z(), vec.t()); }
void CartesFourVecBranch::operator+=(const HepMC::FourVector& vec) { push_back(vec); }
void CartesFourVecBranch::set(const HepMC::FourVector& vec, size_t pos) { set(vec.x(), vec.y(), vec.z(), vec.t(), pos); }
size_t CartesFourVecBranch::size() const { return m_x.size(); }
bool CartesFourVecBranch::fill(const EventContext&) { return true; }
bool CartesFourVecBranch::init() { return true; }
void CartesFourVecBranch::push_back(const float x, const float y, const float z, const float t) {
    m_x += x;
    m_y += y;
    m_z += z;
    m_t += t;
}
void CartesFourVecBranch::set(const float x, const float y, const float z, const float t, size_t pos) {
    m_x[pos] = x;
    m_y[pos] = y;
    m_z[pos] = z;
    m_t[pos] = t;
}
