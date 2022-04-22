/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MUONTESTERBRANCH_H
#define MUONTESTER_MUONTESTERBRANCH_H

#include <MuonTesterTree/IMuonTesterBranch.h>
#include <TTree.h>

/// First stage of the IMuonTesterBranch implementation
///  --> saves the name of the MuonTree as member and
///      provides a generic method to add the data variable
///      to the TTree in the initialization stage

class MuonTesterTree;
class MuonTesterBranch : virtual public IMuonTesterBranch {
public:
    MuonTesterBranch(MuonTesterTree& tree, const std::string& br_name);
    /// Standard constructor taking the TTree object and the name of the branch to be added to the TTree
    MuonTesterBranch(TTree* tree, const std::string& br_name);

    MuonTesterBranch(const MuonTesterBranch&) = delete;
    void operator=(const MuonTesterBranch&) = delete;
    virtual ~MuonTesterBranch();

    std::string name() const override;
    /// Returns whether the object has been initialized or not
    /// The outcome of the variable is bound to a successful call
    /// of the addToTree method
    bool initialized() const;
    /// Returns the underlying TTree object
    TTree* tree() const;

    static std::string eraseWhiteSpaces(const std::string& In);

protected:
    template <class T> bool addToTree(T& variable);

    MuonTesterTree& parent();

private:
    TTree* m_tree{nullptr};
    MuonTesterTree* m_parent{nullptr};
    std::string m_name{};
    bool m_init{false};
};
#include <MuonTesterTree/MuonTesterBranch.icc>
#endif
