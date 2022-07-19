/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_ARRAYBRANCH_H
#define MUONTESTER_ARRAYBRANCH_H

#include <MuonTesterTree/IMuonTesterBranch.h>
#include <TTree.h>
/// Class to store array like branches into the n-tuples
class MuonTesterTree;
template <class T> class ArrayBranch : public IMuonTesterBranch {
public:
    /// Constructor
    ArrayBranch(TTree* tree, const std::string& name, size_t size);
    ArrayBranch(MuonTesterTree& tree, const std::string& name, size_t size);

    /// Constructor with default value to be set in case an element is not updated
    ArrayBranch(TTree* tree, const std::string& name, size_t size, const T& def_val);
    ArrayBranch(MuonTesterTree& tree, const std::string& name, size_t size, const T& def_val);

    ArrayBranch(const ArrayBranch&) = delete;
    void operator=(const ArrayBranch&) = delete;
    virtual ~ArrayBranch();

    /// Returns false if one of the array values is not updated
    bool fill(const EventContext&) override final;
    /// Connects the branch with the tree
    bool init() override final;
    /// Returns the name of the branch
    std::string name() const override final;
    /// Returns the data dependencies of the branch (empty)
    std::vector<DataDependency> data_dependencies() override final;

    /// Underlying tree object
    TTree* tree() const;

    /// Is the branch initialized
    bool initialized() const;

    /// How large is the array
    size_t size() const;

    /// Set's the i-th entry of the array to val
    void set(size_t s, const T& val);
    /// Returns the i-th value
    const T& get(size_t s) const;

    /// Operator implementation of get
    const T& operator[](size_t s) const;
    /// Operator implementation of set()
    T& operator[](size_t s);

    /// default value in cases the branch is not updated
    const T& getDefault() const;

    virtual void setDefault(const T& val);

private:
    /// Data type of the branch in the tree
    std::string tree_data_type() const;
    /// Resets the check mask
    void reset();

    TTree* m_tree{nullptr};
    std::string m_name{};

    size_t m_size{0};
    std::unique_ptr<T[]> m_data{nullptr};
    std::unique_ptr<bool[]> m_updated{nullptr};

    bool m_init{false};

    /// Default value in cases where the part are not updated
    T m_default{};
    bool m_failIfNotUpdated{true};

    MuonTesterTree* m_parent{nullptr};
};
/// Specification of the branch data_type for the TTree initialize routine
template <> std::string ArrayBranch<char*>::tree_data_type() const;
template <> std::string ArrayBranch<std::string>::tree_data_type() const;
template <> std::string ArrayBranch<Char_t>::tree_data_type() const;
template <> std::string ArrayBranch<UChar_t>::tree_data_type() const;
template <> std::string ArrayBranch<Short_t>::tree_data_type() const;
template <> std::string ArrayBranch<Int_t>::tree_data_type() const;
template <> std::string ArrayBranch<Long64_t>::tree_data_type() const;
template <> std::string ArrayBranch<UShort_t>::tree_data_type() const;
template <> std::string ArrayBranch<UInt_t>::tree_data_type() const;
template <> std::string ArrayBranch<ULong64_t>::tree_data_type() const;
template <> std::string ArrayBranch<Float_t>::tree_data_type() const;
template <> std::string ArrayBranch<Double_t>::tree_data_type() const;
template <> std::string ArrayBranch<Bool_t>::tree_data_type() const;

#include <MuonTesterTree/ArrayBranch.icc>
#endif
