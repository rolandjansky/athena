#include "InDetPerfNtuple.h"

InDetPerfNtuple::InDetPerfNtuple(InDetPlotBase* pParent, const std::string & dirName, const std::string & treeName):
    InDetPlotBase(pParent, dirName),
    m_treeName{treeName} {
}

void InDetPerfNtuple::initializePlots(){

    ATH_MSG_DEBUG("Booking Tree");
    m_tree = BookTree(m_treeName);

    ATH_MSG_DEBUG("Attaching all "<<m_mapBranches.size()<<" branches");
    for (auto & branch : m_mapBranches){
        auto outcome = branch.second->attach(m_tree);
        switch (outcome){
            case InDetPerfNtupleBranchBase::attachmentOutcome::InvalidTree:
                ATH_MSG_ERROR("Failed to create the output tree with name "<<m_sDirectory<<"/"<<m_treeName);
                return;
                break;
            case InDetPerfNtupleBranchBase::attachmentOutcome::FailedToBranch:
                ATH_MSG_ERROR("Failed to book branch "<<branch.first<<" in tree "<<m_sDirectory<<"/"<<m_treeName);
                break;
            case InDetPerfNtupleBranchBase::attachmentOutcome::Success: 
                ATH_MSG_DEBUG("Successfully attached a branch "<<branch.first<<" in tree "<<m_sDirectory<<"/"<<m_treeName);
        }

    }
    ATH_MSG_DEBUG("Setting branches to defaults");
    ResetBranches();
    ATH_MSG_DEBUG("Done booking branches");
}

void InDetPerfNtuple::addBranch(InDetPerfNtupleBranchBase* theBranch){

    auto res = m_mapBranches.emplace(theBranch->getName(), theBranch);
    if (res.second){
        ATH_MSG_DEBUG("Registered a new branch "<<theBranch->getName());
    }
    else{
        ATH_MSG_WARNING("The branch "<<theBranch->getName()<<" already exists in tree "<<m_sDirectory<<"/"<<m_treeName<<" - not overwriting.");
    }
}

void InDetPerfNtuple::ResetBranches(){
    for (auto & b : m_mapBranches){
        b.second->setDummy();
    }
}
