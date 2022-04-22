/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <AthenaKernel/getMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <MuonTesterTree/ArrayBranch.h>
#include <MuonTesterTree/MuonTesterBranch.h>
#include <MuonTesterTree/MuonTesterTree.h>
#include <MuonTesterTree/ScalarBranch.h>
#include <MuonTesterTree/VectorBranch.h>

#include <functional>

#define SET_BRANCHSTRING(theType, thestring) \
    template <> std::string ArrayBranch<theType>::tree_data_type() const { return thestring; }
SET_BRANCHSTRING(Char_t, "/B")
SET_BRANCHSTRING(std::string, "/C")
SET_BRANCHSTRING(UChar_t, "/b")
SET_BRANCHSTRING(Short_t, "/S")
SET_BRANCHSTRING(Int_t, "/I")
SET_BRANCHSTRING(Long64_t, "/L")
SET_BRANCHSTRING(long, "/I")
SET_BRANCHSTRING(unsigned long, "/i")
SET_BRANCHSTRING(UShort_t, "/s")
SET_BRANCHSTRING(UInt_t, "/i")
SET_BRANCHSTRING(ULong64_t, "/l")
SET_BRANCHSTRING(Float_t, "/F")
SET_BRANCHSTRING(Double_t, "/D")
SET_BRANCHSTRING(Bool_t, "/o")
#undef SET_BRANCHSTRING

MuonTesterBranch::MuonTesterBranch(MuonTesterTree& tree, const std::string& br_name) : MuonTesterBranch(tree.tree(), br_name) {
    m_parent = &tree;
    m_parent->addBranch(this);
}
MuonTesterTree& MuonTesterBranch::parent() {
    if (!m_parent) {
        MsgStream log(Athena::getMessageSvc(), "MuonTesterBranch()");
        log << MSG::WARNING << "The parent of  " << name() << " is null." << endmsg;
    }
    return *m_parent;
}
MuonTesterBranch::~MuonTesterBranch() {
    if (m_parent) m_parent->removeBranch(this);
}
MuonTesterBranch::MuonTesterBranch(TTree* tree, const std::string& br_name) : m_tree(tree), m_name(br_name) {}
std::string MuonTesterBranch::name() const { return m_name; }
bool MuonTesterBranch::initialized() const { return m_init; }
TTree* MuonTesterBranch::tree() const { return m_tree; }

std::string MuonTesterBranch::eraseWhiteSpaces(const std::string& In) {
    std::string out = In;
    out.erase(std::remove_if(out.begin(), out.end(), isspace), out.end());
    return out;
}

template <> bool& VectorBranch<bool>::get(size_t) {
    throw std::runtime_error("For boolean branches the get() operator is cumbersome");
    return m_default;
}
