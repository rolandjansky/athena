#ifndef INDET__PERF__NTUPLE__H
#define INDET__PERF__NTUPLE__H

#include "InDetPerfNtupleBranch.h" 
#include "GaudiKernel/ITHistSvc.h"
#include "InDetPlotBase.h"

/// This class is a base class for the actual ntuples used when 
/// writing IDPVM ntuples. 

class InDetPerfNtuple : public InDetPlotBase {
public:
    // t: TTree to write to 
    InDetPerfNtuple(InDetPlotBase* pParent, const std::string & dirName, const std::string & treeName);

    // attaches an NtupleBranch to this object.
    // No smart pointer used since these objects are members
    // in the scope of derived classes and not should not be memory-managed as pointers
    void addBranch(InDetPerfNtupleBranchBase* theBranch); 

    // Fill an entry into the tree. Following this, will reset all branches. 
    void Fill(){ m_tree->Fill(); ResetBranches(); }

    // get the underlying ROOT TTree object 
    // tree is returned by 
    TTree* getTree() const { return m_tree; }

    // we use this to book the tree and attach branches to it
     virtual void initializePlots(); 

protected:
    // reset all branches to their defaults. 
    void ResetBranches();

private: 
    // data members
    std::string m_treeName;
    std::map<std::string, InDetPerfNtupleBranchBase*> m_mapBranches;
    TTree* m_tree{nullptr};
};

#endif // INDET__PERF__NTUPLE__H