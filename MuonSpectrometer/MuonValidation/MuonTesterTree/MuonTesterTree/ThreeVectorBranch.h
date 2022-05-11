/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MUONTHREEVECTORBRANCH_H
#define MUONTESTER_MUONTHREEVECTORBRANCH_H

#include <GeoPrimitives/GeoPrimitivesHelpers.h>
#ifndef HEPMC3
#include <HepMC/SimpleVector.h>
#endif
#include <MuonTesterTree/MuonTesterTree.h>
#include <MuonTesterTree/VectorBranch.h>

/// Helper class to dump spatial vectors in their x,y,z representation
/// to the n-tuple
class ThreeVectorBranch : public MuonTesterBranch {
public:
    ThreeVectorBranch(MuonTesterTree& tree, const std::string& vec_name);

    /// interface using the Amg::Vector3D
    void push_back(const Amg::Vector3D& vec);
    void operator+=(const Amg::Vector3D& vec);
    void set(const Amg::Vector3D& vec, size_t pos);

    void push_back(const TVector3& vec);
    void operator+=(const TVector3& vec);
    void set(const TVector3& vec, size_t pos);
#ifndef HEPMC3
#ifndef XAOD_ANALYSIS
    void push_back(const HepMC::ThreeVector& vec);
    void operator+=(const HepMC::ThreeVector& vec);
    void set(const HepMC::ThreeVector& vec, size_t pos);
#endif
#endif
    void push_back(const float x, const float y, const float z);
    void set(const float x, const float y, const float z, size_t pos);

    size_t size() const;

    bool fill(const EventContext&) override final;
    bool init() override final;

private:
    VectorBranch<float>& m_x{parent().newVector<float>(name() + "X")};
    VectorBranch<float>& m_y{parent().newVector<float>(name() + "Y")};
    VectorBranch<float>& m_z{parent().newVector<float>(name() + "Z")};
};
#endif
