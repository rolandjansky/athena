/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MATRIXBRANCH_H
#define MUONTESTER_MATRIXBRANCH_H
#include <MuonTesterTree/VectorBranch.h>
/// Helper class to store branches of type std::vector<std::vector<T>>
class MuonTesterTree;
template <class T> class MatrixBranch : public IMuonTesterBranch {
public:
    /// Standard constructor
    MatrixBranch(TTree* tree, const std::string& name);
    MatrixBranch(MuonTesterTree& tree, const std::string& name);

    /// Constructor with default value extending the array
    MatrixBranch(TTree* tree, const std::string& b_name, const T& def);
    MatrixBranch(MuonTesterTree& tree, const std::string& b_name, const T& def);

    MatrixBranch(const MatrixBranch&) = delete;
    void operator=(const MatrixBranch&) = delete;
    virtual ~MatrixBranch() = default;

    bool fill(const EventContext& ctx) override final;
    bool init() override final;
    std::string name() const override final;

    std::vector<DataDependency> data_dependencies() override final;

    TTree* tree() const;
    bool initialized() const;

    /// Returns the i-th element of the outer vector
    inline std::vector<T>& get(size_t i);
    inline std::vector<T>& operator[](size_t i);

    /// Returns the j-th element of the i-th inner vector
    inline T& get(size_t i, size_t j);

    /// Push back the element into the i-th inner vector
    inline void push_back(size_t i, const T& value);

    /// Size of the outer vector
    inline size_t nrows() const;
    /// Size of the i-th inner vector
    inline size_t ncols(size_t row) const;

    /// Get the default value to fill the empty fields in the vector
    inline const T& getDefault() const;
    void setDefault(const T& def);

private:
    VectorBranch<std::vector<T>> m_Vec;
    T m_default;
};
#include <MuonTesterTree/MatrixBranch.icc>
#endif
