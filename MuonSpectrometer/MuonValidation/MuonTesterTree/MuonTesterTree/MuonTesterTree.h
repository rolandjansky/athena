/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTERTREE_MUONTESTERTREE_H
#define MUONTESTERTREE_MUONTESTERTREE_H

#include <GaudiKernel/ITHistSvc.h>
#include <GaudiKernel/ServiceHandle.h>
#include <MuonTesterTree/IMuonTesterBranch.h>
#include <MuonTesterTree/MatrixBranch.h>
#include <MuonTesterTree/ScalarBranch.h>
#include <MuonTesterTree/VectorBranch.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODTruth/TruthEvent.h>

#include <set>

class MuonTesterTree {
public:
    MuonTesterTree(const std::string& tree_name, const std::string& stream);
    ~MuonTesterTree() = default;
    /// TTree object
    TTree* tree() const;
    /// Operator to the TTree object
    TTree* operator->() const;

    /// Has the init method been called and the tree is connected with the output file
    bool initialized() const;

    /// Fills the tree per call
    bool fill(const EventContext& ctx);

    /// Name of the tree
    std::string name() const;
    /// file_stream of the analysis to which the tree belongs
    std::string file_stream() const;
    /// sub directory in the TFile
    std::string path() const;
    
    /// Save the TTree in a subfolder of the TFile
    void set_path(const std::string& new_path);

    /// Initialize method
   template <class OWNER,
            typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
   StatusCode init(OWNER* instance);

    /// Finally write the TTree objects
    StatusCode write();
    /// This method adds the branch to the tree and hands over the ownership
    /// to the MuonAnalysisTree instance
    /// IF the second argument is set to false
    /// the branch is not added to the active list of branches i.e.
    /// no fill and initialize function is called on it
    bool registerBranch(std::shared_ptr<IMuonTesterBranch> branch);

    /// Branch is added to the tree without transferring the ownership
    bool addBranch(std::shared_ptr<IMuonTesterBranch> branch);
    bool addBranch(IMuonTesterBranch& branch);
    bool addBranch(IMuonTesterBranch* branch);
    /// In case instances of a certain branch type are destroyed before hand
    void removeBranch(IMuonTesterBranch* branch);
    void removeBranch(IMuonTesterBranch& branch);

    /// Skips the branch from being added to the tree
    void disableBranch(const std::string& br_name);
    void disableBranch(const std::vector<std::string>& br_names);

    /// Retrieves the branches owned by the muon_tree
    template <class T> std::shared_ptr<T> get_branch(const std::string& str) const;

    /// Creates new branches and returns their reference
    template <typename T> VectorBranch<T>& newVector(const std::string& name);
    template <typename T> ScalarBranch<T>& newScalar(const std::string& name);
    template <typename T> MatrixBranch<T>& newMatrix(const std::string& name);
    /// returns the reference of the branch
    template <typename T> T& newBranch(std::shared_ptr<T> br);

    /// Returns a boolean whether the branch is already part of the tree or one of the deligated friends
    bool is_active(const IMuonTesterBranch* branch) const;

    ///
    ///	Event objects needed for the calculation
    /// of few variables
    void set_evt_info(const xAOD::EventInfo* ev_info);
    void set_mc_evt_info(const xAOD::TruthEvent* ev_info);
    void set_primary_vtx(const xAOD::Vertex* vtx);

    const xAOD::EventInfo* evt_info() const;
    const xAOD::TruthEvent* mc_evt_info() const;
    const xAOD::Vertex* primary_vtx() const;

private:
     /// Initialze the tree with the output file. The stream corresponds to the stream
    /// of the file e.g MDTTester HighEtaTester
    StatusCode init(ServiceHandle<ITHistSvc> hist_svc);
    
    using DataDependency = IMuonTesterBranch::DataDependency;
    std::vector<DataDependency> m_dependencies{};

    std::unique_ptr<TTree> m_tree{nullptr};
    std::string m_stream{};
    std::string m_path{};
    bool m_init{false};
    // List of branches which are owned by the tree. They will be deleted if the object is destructed
    std::vector<std::shared_ptr<IMuonTesterBranch>> m_branches{};
    // List of branches which shall be initialized. The list will be sorted before initialization and then cleared
    // None of the branches is explicitly owned by the object
    std::vector<IMuonTesterBranch*> m_branches_to_init{};
    std::set<std::string> m_excludedBranches{};

    TDirectory* m_directory{nullptr};
    bool m_filled{false};
    ServiceHandle<ITHistSvc> m_hist_svc{"", ""};

    const xAOD::EventInfo* m_ev_info{nullptr};
    const xAOD::TruthEvent* m_mc_ev_info{nullptr};
    const xAOD::Vertex* m_vtx{nullptr};
};

#include <MuonTesterTree/MuonTesterTree.icc>
#endif
