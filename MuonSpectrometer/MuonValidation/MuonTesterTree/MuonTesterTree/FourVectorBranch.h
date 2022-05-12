/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MUONFOURVECTORBRANCH_H
#define MUONTESTER_MUONFOURVECTORBRANCH_H
#include <AtlasHepMC/SimpleVector.h>
#include <MuonTesterTree/ThreeVectorBranch.h>
#include <TLorentzVector.h>

/// Helper class to store the four-momentum in a TTree using
/// the Pt, Eta, Phi, E representation
class PtEtaPhiEBranch : public MuonTesterBranch {
public:
    PtEtaPhiEBranch(MuonTesterTree& tree, const std::string& vec_name);

    void push_back(const TLorentzVector& vec);
    void operator+=(const TLorentzVector& vec);
    void set(const TLorentzVector& vec, size_t pos);

    void push_back(const HepMC::FourVector& vec);
    void operator+=(const HepMC::FourVector& vec);
    void set(const HepMC::FourVector& vec, size_t pos);

    void push_back(const float pt, const float eta, const float phi, const float e);
    void set(const float pt, const float eta, const float phi, const float e, size_t pos);

    size_t size() const;

    bool fill(const EventContext&) override final;
    bool init() override final;

private:
    VectorBranch<float>& m_pt{parent().newVector<float>(name() + "Pt")};
    VectorBranch<float>& m_eta{parent().newVector<float>(name() + "Eta")};
    VectorBranch<float>& m_phi{parent().newVector<float>(name() + "Phi")};
    VectorBranch<float>& m_e{parent().newVector<float>(name() + "E")};
};

/// Helper class to store the four-momentum in a TTree using
/// the X,Y,Z,T or Px,Py,Pz,E representation
class CartesFourVecBranch : public MuonTesterBranch {
public:
    enum class Type { Mom, Pos };
    CartesFourVecBranch(MuonTesterTree& tree, const std::string& vec_name, Type t);

    void push_back(const TLorentzVector& vec);
    void operator+=(const TLorentzVector& vec);
    void set(const TLorentzVector& vec, size_t pos);

    void push_back(const HepMC::FourVector& vec);
    void operator+=(const HepMC::FourVector& vec);
    void set(const HepMC::FourVector& vec, size_t pos);

    void push_back(const float x, const float y, const float z, const float t);
    void set(const float x, const float y, const float z, const float t, size_t pos);

    size_t size() const;

    bool fill(const EventContext&) override final;
    bool init() override final;

private:
    bool m_mom{false};
    VectorBranch<float>& m_x{parent().newVector<float>(name() + (m_mom ? "Px" : "X"))};
    VectorBranch<float>& m_y{parent().newVector<float>(name() + (m_mom ? "Py" : "Y"))};
    VectorBranch<float>& m_z{parent().newVector<float>(name() + (m_mom ? "Pz" : "Z"))};
    VectorBranch<float>& m_t{parent().newVector<float>(name() + (m_mom ? "E" : "T"))};
};
#endif
