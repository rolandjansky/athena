/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MUONVECTORBRANCH_H
#define MUONTESTER_MUONVECTORBRANCH_H

#include <MuonTesterTree/MuonTesterBranch.h>

// Implementation to store vector like variables

class MuonTesterTree;
template <class T> class VectorBranch : public MuonTesterBranch, virtual public IMuonTesterBranch {
public:
    /// Standard constructor
    VectorBranch(TTree* tree, const std::string& name);
    VectorBranch(MuonTesterTree& tree, const std::string& name);

    /// Constructor with assigned default value
    VectorBranch(TTree* tree, const std::string& name, const T& def);
    VectorBranch(MuonTesterTree& tree, const std::string& name, const T& def);

    virtual ~VectorBranch() = default;

    /// Clears vector in cases that it has not been updated in this event
    /// Retursn falls if the vector has not been initialized yet
    bool fill(const EventContext& ctx) override;
    /// Initialized the Branch
    bool init() override;

    /// Returns the number of actual saved elements
    inline size_t size() const;

    /// Adds a new element at the end of the vector
    inline void push_back(const T& value);
    inline void operator+=(const T& value);

    /// Accesses the idx-th element.
    /// If the index is larger than the size of the vector
    /// it is then enlarged with the default_value
    inline T& operator[](size_t idx);
    inline T& get(size_t idx);

    inline const T& getDefault() const;
    virtual void setDefault(const T& def);

    inline bool isUpdated() const;

private:
    std::vector<T> m_variable{};
    T m_default{};
    bool m_updated{false};
};

template <> bool& VectorBranch<bool>::get(size_t idx);
#include <MuonTesterTree/VectorBranch.icc>
#endif
