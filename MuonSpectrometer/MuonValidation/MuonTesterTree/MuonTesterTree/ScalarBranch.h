/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MUONSCALARBRANCH_H
#define MUONTESTER_MUONSCALARBRANCH_H

#include <MuonTesterTree/MuonTesterBranch.h>
/// Implementation to save simple Scalar variables to the Tree. Satisfies all properties of the
/// common interface and inherits from the MuonTesterBranch class to make use of the addToTree method
class MuonTesterTree;
template <class T> class ScalarBranch : public MuonTesterBranch, virtual public IMuonTesterBranch {
public:
    /// Default constructor passing the TTree object and
    /// the name of the branch in the tree. The value of the branch
    /// must always be set before the fill call
    ScalarBranch(TTree* tree, const std::string& name);
    ScalarBranch(MuonTesterTree& tree, const std::string& name);

    /// Constructor providing a default_value which is inserted when the information
    /// in the branch is not updated.
    ScalarBranch(TTree* tree, const std::string& name, const T& default_value);
    ScalarBranch(MuonTesterTree& tree, const std::string& name, const T& default_value);

    virtual ~ScalarBranch() = default;

    bool fill(const EventContext& ctx) override;
    bool init() override;

    ///  Setter methods. Either via assignment operator
    ///  E.g. my_branch = 1.
    ///  ot by
    void operator=(const T& value);
    void setValue(const T& value);

    bool isUpdated() const;
    void setDefault(const T& def);
    void disableFailing();

    const T& getDefault() const;
    const T& getVariable() const;
    bool mustBeUpdated() const;

private:
    T m_variable{};
    T m_default{};
    bool m_failIfNotUpdated{true};
    bool m_updated{false};
};

template <typename T> std::ostream& operator<<(std::ostream& out, const ScalarBranch<T>& B);

#include <MuonTesterTree/ScalarBranch.icc>
#endif
