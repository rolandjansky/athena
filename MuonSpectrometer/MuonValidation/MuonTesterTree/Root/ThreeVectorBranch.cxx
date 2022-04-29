/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <MuonTesterTree/ThreeVectorBranch.h>

bool ThreeVectorBranch::fill(const EventContext&) { return true; }
bool ThreeVectorBranch::init() { return true; }

ThreeVectorBranch::ThreeVectorBranch(MuonTesterTree& tree, const std::string& vec_name) : MuonTesterBranch(tree, vec_name) {}
size_t ThreeVectorBranch::size() const { return m_x.size(); }

void ThreeVectorBranch::push_back(const float x, const float y, const float z) {
    m_x += x;
    m_y += y;
    m_z += z;
}
void ThreeVectorBranch::set(const float x, const float y, const float z, size_t pos) {
    m_x[pos] = x;
    m_y[pos] = y;
    m_z[pos] = z;
}
void ThreeVectorBranch::push_back(const Amg::Vector3D& vec) { push_back(vec[0], vec[1], vec[2]); }
void ThreeVectorBranch::set(const Amg::Vector3D& vec, size_t pos) { set(vec[0], vec[1], vec[2], pos); }
void ThreeVectorBranch::operator+=(const Amg::Vector3D& vec) { push_back(vec); }
#ifndef HEPMC3
#ifndef XAOD_ANALYSIS
void ThreeVectorBranch::push_back(const HepMC::ThreeVector& vec) { push_back(vec.x(), vec.y(), vec.z()); }
void ThreeVectorBranch::set(const HepMC::ThreeVector& vec, size_t pos) { set(vec.x(), vec.y(), vec.z(), pos); }
void ThreeVectorBranch::operator+=(const HepMC::ThreeVector& vec) { push_back(vec); }
#endif
#endif
void ThreeVectorBranch::push_back(const TVector3& vec) { push_back(vec.X(), vec.Y(), vec.Z()); }
void ThreeVectorBranch::operator+=(const TVector3& vec) { push_back(vec); }
void ThreeVectorBranch::set(const TVector3& vec, size_t pos) { set(vec.X(), vec.Y(), vec.Z(), pos); }
