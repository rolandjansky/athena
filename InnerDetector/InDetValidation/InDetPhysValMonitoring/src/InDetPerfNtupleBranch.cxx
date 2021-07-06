#include "InDetPerfNtupleBranch.h" 
#include "InDetPerfNtuple.h" 

InDetPerfNtupleBranchBase::InDetPerfNtupleBranchBase(const std::string & branchName, InDetPerfNtuple & mgr):
    m_branchName(branchName) {
    mgr.addBranch(this);
}

InDetPerfNtupleBranchBase::attachmentOutcome InDetPerfNtupleBranchBase::attach(TTree* targetTree){
    if (targetTree) {
        m_branch = this->addPayloadAsBranch(targetTree);
        return (m_branch ? attachmentOutcome::Success : attachmentOutcome::FailedToBranch); 
    }
    else {
        return attachmentOutcome::InvalidTree;
    }
}