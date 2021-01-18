#ifndef INDET__PERF__NTUPLEBRANCH__H
#define INDET__PERF__NTUPLEBRANCH__H

#include "TTree.h"
#include "TBranch.h"
#include <memory>

/// This defines a helper class used for writing output ntuples in IDPVM. 

// forward declare the template used for providing the actual functionality
template <typename branchType> class InDetPerfNtupleBranch;

// and a manager class used to work with the branches 
class InDetPerfNtuple;

// base class used to store branches in a manager class
class InDetPerfNtupleBranchBase {
public: 
    InDetPerfNtupleBranchBase(const std::string & branchName, InDetPerfNtuple & mgr);
    virtual ~InDetPerfNtupleBranchBase(){};

    // fill the output branch 
    inline void fill(){ if (m_branch) m_branch->Fill(); }

    // set content to a dummy value. Used to reset this branch
    // dummy value specified in specific template instantiations
    virtual void setDummy()=0;

    // get the branch name
    inline const std::string & getName() const { return m_branchName; }

    // get the corresponding TBranch of the underlying TTree 
    inline TBranch* getBranch() const { return m_branch; }

    // reports outcome of attachment attempt 
    enum class attachmentOutcome {
        Success,
        InvalidTree,
        FailedToBranch
    };
    // attach to output tree - wrap around addPayloadAsBranch with input validation
    attachmentOutcome attach(TTree* targetTree);

protected:
    // method specific to the payload type - handles creation of the TBranch instance
    virtual TBranch* addPayloadAsBranch(TTree* targetTree)=0;

private:
    // data members

    // name of the branch
    const std::string m_branchName;
    // TBranch used to store the information 
    TBranch* m_branch{nullptr};
};

// template specialisation for arbitrary branchType 
template<typename branchType> class InDetPerfNtupleBranch : public InDetPerfNtupleBranchBase { 
public: 
    // constructor
    InDetPerfNtupleBranch(const std::string & branchName, branchType defaultValue, InDetPerfNtuple & mgr): 
        InDetPerfNtupleBranchBase(branchName, mgr),
        m_default{defaultValue} {
    }
  
    // access the current content of the branch
    inline branchType & operator()(){ return get(); }
    inline branchType & get(){ return m_content; }

    // fill the branch when writing an output tree 
    inline void set(const branchType & in){ m_content=in; }
    inline void operator=(const branchType & in){ set(in); }

    // set to a dummy value
    inline void setDummy(){ set(m_default); }

protected:
    // for reading
    virtual TBranch* addPayloadAsBranch(TTree* targetTree){ 
        return (targetTree ? targetTree->Branch(getName().c_str(), &m_content) : nullptr);
    }

private: 
    branchType m_content{m_default};
    const branchType m_default;
};

// template specialisation for vector branches 
template<typename branchType> class InDetPerfNtupleBranch<std::vector<branchType>> : public InDetPerfNtupleBranchBase { 
public: 
    // constructor, providing the branch name used and the tree to read from. 
    InDetPerfNtupleBranch(const std::string & branchName, InDetPerfNtuple & mgr): 
        InDetPerfNtupleBranchBase(branchName, mgr) {
    }

    // access the vector member for direct manipulation
    inline std::vector<branchType> & operator()(){ return get(); }
    inline const std::vector<branchType> & get(){ return *m_content; }

    // access to elements by index
    inline branchType operator()(size_t i){ return get(i); }
    inline branchType get(size_t i){ return m_content->at(i); }

    // make iterable 
    inline const typename std::vector<branchType>::const_iterator begin(){ return get().begin(); }
    inline const typename std::vector<branchType>::const_iterator end(){ return get().end(); }

    // overwrite the content 
    inline void set(const std::vector<branchType> & in){ *m_content = in; }

    // set to a dummy value - for a vector, this will trigger a reset
    inline void setDummy(){ m_content->clear(); }

protected:
    virtual TBranch* addPayloadAsBranch(TTree* targetTree){
        return (targetTree ? targetTree->Branch(getName().c_str(), m_content.get()) : nullptr);
    }

private: 
    std::shared_ptr<std::vector<branchType>> m_content;
};

#endif // INDET__PERF__NTUPLEBRANCH__H
