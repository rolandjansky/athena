/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <MuonTesterTree/MuonTesterTree.h>

namespace {
    // Erase all objects from the branch vector
    void Remove(std::vector<IMuonTesterBranch*>& vec, std::function<bool(const IMuonTesterBranch*)> remove_func) {
        std::vector<IMuonTesterBranch*>::iterator itr = std::find_if(vec.begin(), vec.end(), remove_func);
        while (itr != vec.end()) {
            vec.erase(itr);
            itr = std::find_if(vec.begin(), vec.end(), remove_func);
        }
    }

}  // namespace
std::string MuonTesterTree::name() const { return m_tree->GetName(); }
TTree* MuonTesterTree::tree() const { return m_tree.get(); }
TTree* MuonTesterTree::operator->() const { return m_tree.get(); }
bool MuonTesterTree::registerBranch(std::shared_ptr<IMuonTesterBranch> branch) {
    MsgStream log(Athena::getMessageSvc(), "MuonTesterTree");
    if (!branch) {
        log << MSG::ERROR << name() << "Nullptr given" << endmsg;
        return false;
    }
    std::vector<std::shared_ptr<IMuonTesterBranch>>::const_iterator itr = std::find_if(
        m_branches.begin(), m_branches.end(),
        [&branch](const std::shared_ptr<IMuonTesterBranch>& known) { return known == branch || known->name() == branch->name(); });
    if (itr != m_branches.end()) {
        if (typeid((*itr).get()) != typeid(branch.get())) {
            log << MSG::ERROR << "Different branches have been added here under " << branch->name() << endmsg;
            return false;
        }
        return true;
    } else if (m_filled) {
        log << MSG::ERROR << name() << "is already initialized." << endmsg;
        return false;
    }
    m_branches.push_back(branch);
    return true;
}
bool MuonTesterTree::addBranch(std::shared_ptr<IMuonTesterBranch> branch) { return registerBranch(branch) && addBranch(branch.get()); }
bool MuonTesterTree::addBranch(IMuonTesterBranch& branch) { return addBranch(&branch); }
bool MuonTesterTree::addBranch(IMuonTesterBranch* branch) {
    MsgStream log(Athena::getMessageSvc(), "MuonTesterTree");
    if (is_active(branch))
        return true;
    else if (m_filled) {
        log << MSG::ERROR << name() << " is already initialized. Cannot add " << branch->name() << endmsg;
        return false;
    } else if (!branch) {
        log << MSG::ERROR << "Nullptr given" << endmsg;
        return false;
    } else {
        m_branches_to_init.push_back(branch);
        return true;
    }
    return false;
}

void MuonTesterTree::removeBranch(IMuonTesterBranch* branch) {
    Remove(m_branches_to_init, [branch](const IMuonTesterBranch* br) { return br == branch; });
}
void MuonTesterTree::removeBranch(IMuonTesterBranch& branch) { removeBranch(&branch); }

bool MuonTesterTree::initialized() const { return m_init; }
bool MuonTesterTree::fill(const EventContext& ctx) {
    MsgStream log(Athena::getMessageSvc(), "MuonTesterTree");
    
    if (!initialized()) {
        log << MSG::ERROR << "The tree " << name() << " has not been initialized yet" << endmsg;
        return false;
    }
    if (!m_excludedBranches.empty()) m_excludedBranches.clear();
    /// These branches are actually initialized
    for (auto& branch : m_branches_to_init) {
        log << MSG::DEBUG<<"Try to fill "<<branch->name()<<endmsg;
        if (!branch->fill(ctx)) {
            log << MSG::ERROR << "fill()  --- Failed to fill branch " << branch->name() << " in tree " << name() << endmsg;
            return false;
        }
    }
    m_tree->Fill();
    m_filled = true;
    return true;
}
StatusCode MuonTesterTree::init(ServiceHandle<ITHistSvc> hist_svc) {
    MsgStream log(Athena::getMessageSvc(), "MuonTesterTree");
    if (file_stream().empty()) {
        log << MSG::ERROR << "The file stream of " << name() << " has not been set yet" << endmsg;
        return StatusCode::FAILURE;
    }
    if (!initialized()) {
        /// push back the ds id index
        std::string full_path = Form("/%s/%s%s%s", file_stream().c_str(), path().c_str(), path().empty() ? "" : "/", name().c_str());
        if (!hist_svc->regTree(full_path, tree()).isSuccess()) { return StatusCode::FAILURE; }
        m_directory = m_tree->GetDirectory();
        if (!m_directory) {
            log << MSG::ERROR << "Where is my directory to write later?" << endmsg;
            return StatusCode::FAILURE;
        }
        m_hist_svc = hist_svc;
        m_init = true;
    }

    Remove(m_branches_to_init, [this](const IMuonTesterBranch* br) {
        return !br || std::find(m_excludedBranches.begin(), m_excludedBranches.end(), br->name()) != m_excludedBranches.end();
    });
    /// Sort by alphabet
    std::sort(m_branches_to_init.begin(), m_branches_to_init.end(),
              [](IMuonTesterBranch* a, IMuonTesterBranch* b) { return a->name() < b->name(); });
    for (auto& branch : m_branches_to_init) {
        if (!branch->init()) {
            log << MSG::ERROR << "Failed to initialize branch " << branch->name() << endmsg;
            return StatusCode::FAILURE;
        }
        log << MSG::DEBUG << " Branch " << branch->name() << " has been initialized" << endmsg;
        std::vector<DataDependency> data_dep = branch->data_dependencies();
        for (const DataDependency& data : data_dep) {
            m_dependencies.emplace_back(data);
        }

    }
    /// Kick everything that is not owned by the class itself
    Remove(m_branches_to_init, [this](const IMuonTesterBranch* br) {
        return std::find_if(m_branches.begin(), m_branches.end(),
                            [br](const std::shared_ptr<IMuonTesterBranch>& owned) { return owned.get() == br; }) == m_branches.end();
    });
    log << MSG::INFO << "Successfully initialized " << name() << "." << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode MuonTesterTree::write() {
    if (!initialized() || !m_tree) { return StatusCode::SUCCESS; }
    if (!m_hist_svc->deReg(tree()).isSuccess()) {
        MsgStream log(Athena::getMessageSvc(), "MuonTesterTree");
        log << MSG::ERROR<< " write() --- Failed to put the tree out of the HistService"<<endmsg;
        return StatusCode::FAILURE;
    }
    m_directory->WriteObject(tree(), tree()->GetName(), "overwrite");
    m_tree.reset();
    m_branches_to_init.clear();
    m_branches.clear();
    return StatusCode::SUCCESS;
}
void MuonTesterTree::disableBranch(const std::string& b_name) {
    if (!m_filled) m_excludedBranches.insert(b_name);
}
void MuonTesterTree::disableBranch(const std::vector<std::string>& br_names) {
    if (!m_filled) m_excludedBranches.insert(br_names.begin(), br_names.end());
}
std::string MuonTesterTree::file_stream() const { return m_stream; }
bool MuonTesterTree::is_active(const IMuonTesterBranch* branch) const {
    if (!branch) {        
        MsgStream log(Athena::getMessageSvc(), "MuonTesterTree");
        log << MSG::ERROR<<"Nullptr was given."<<endmsg;        
        return false;
    }
    if (std::find_if(m_branches_to_init.begin(), m_branches_to_init.end(), [&branch](const IMuonTesterBranch* known) {
            return known == branch || known->name() == branch->name();
        }) != m_branches_to_init.end())
        return true;
    return false;
}
void MuonTesterTree::set_path(const std::string& new_path) { m_path = new_path;}
std::string MuonTesterTree::path() const { return m_path; }
void MuonTesterTree::set_evt_info(const xAOD::EventInfo* ev_info) { m_ev_info = ev_info; }
const xAOD::EventInfo* MuonTesterTree::evt_info() const { return m_ev_info; }
void MuonTesterTree::set_mc_evt_info(const xAOD::TruthEvent* ev_info) { m_mc_ev_info = ev_info; }
const xAOD::TruthEvent* MuonTesterTree::mc_evt_info() const { return m_mc_ev_info; }
void MuonTesterTree::set_primary_vtx(const xAOD::Vertex* vtx) { m_vtx = vtx; }
const xAOD::Vertex* MuonTesterTree::primary_vtx() const { return m_vtx; }
MuonTesterTree::MuonTesterTree(const std::string& tree_name, const std::string& stream) :
    m_tree{std::make_unique<TTree>(tree_name.c_str(), "MuonTesterTree")}, m_stream(stream) {}
