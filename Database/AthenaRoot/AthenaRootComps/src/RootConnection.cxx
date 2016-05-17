///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootConnection.cxx
// Implementation file for class RootConnection
// Author: Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// AthenaRootComps includes
#include "RootConnection.h"

// fwk includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "DataModelRoot/RootType.h"

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

namespace {
  char typecode_from_typeid(const std::type_info& ti) {
    if        (ti == typeid(char*))     {
      return 'C';
    } else if (ti == typeid(Char_t))    {
      return 'B';
    } else if (ti == typeid(UChar_t))   {
      return 'b';
    } else if (ti == typeid(Short_t))   {
      return 'S';
    } else if (ti == typeid(UShort_t))  {
      return 's';
    } else if (ti == typeid(Int_t))     {
      return 'I';
    } else if (ti == typeid(UInt_t))    {
      return 'i';
    } else if (ti == typeid(Float_t))   {
      return 'F';
    } else if (ti == typeid(Double_t))  {
      return 'D';
    } else if (ti == typeid(Long64_t))  {
      return 'L';
    } else if (ti == typeid(ULong64_t)) {
      return 'l';
    } else if (ti == typeid(Bool_t))    {
      return 'O';
    } else {
      return '\0';
    }
  }
} //> anon-namespace

namespace Athena {

RootConnection::RootConnection(const IInterface* /*own*/, const std::string& pfn) :
	m_fid(),
	m_pfn(pfn),
	//m_age(0),
	//m_owner(own),
	m_file(0),
	m_tree(0),
	m_branch(0),
	m_branchTypeCode('\0') {
}

RootConnection::~RootConnection() {
}

StatusCode RootConnection::connectRead() {
  if (m_file == 0) {
    m_file = TFile::Open(m_pfn.c_str());
  }
  if (m_file == 0 || m_file->IsZombie())   {
    delete m_file; m_file = 0;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode RootConnection::connectWrite(const std::string& mode) {
  if (m_file == 0) {
    m_file = TFile::Open(m_pfn.c_str(), mode.c_str(), "AthenaRoot event data file");
  }
  if (m_file == 0 || m_file->IsZombie()) {
    delete m_file; m_file = 0;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode RootConnection::commit() {
  if (m_file == 0) {
    REPORT_MESSAGE (MSG::ERROR) << "commit: No file connected!";
    return StatusCode::FAILURE;
  }
  if (m_tree !=0 && m_branch != 0) {
    m_tree->SetEntries(m_branch->GetEntries());
  }
  //FIXME: call OptimizeBaskets()
  //m_tree->AutoSave();
  //m_tree->FlushBaskets();
  return StatusCode::SUCCESS;
}

StatusCode RootConnection::disconnect() {
  if (!this->commit().isSuccess()) {
    return StatusCode::FAILURE;
  }
  if (m_tree != 0 && m_file->IsWritable()) {
    m_file->cd("/");
    m_tree->Write(); m_tree = 0;
  }
  m_file->Close(); m_file = 0;
  return StatusCode::SUCCESS;
}

bool RootConnection::isConnected() const {
   return false;
}

StatusCode RootConnection::read(void* const /*data*/, size_t /*len*/) {
   return StatusCode::FAILURE;
}

StatusCode RootConnection::write(const void* data, unsigned long& len) {
  void* address = (void*)data;
  if (m_file == 0 || m_tree == 0 || m_branch == 0) {
    return StatusCode::FAILURE;
  }
  if (m_branchTypeCode == '\0') {
    m_branch->SetAddress(&address);
  } else {
    m_branch->SetAddress(address);
  }
  int32_t nbytes = m_branch->Fill();
  if (nbytes < 0) {
    return StatusCode::FAILURE;
  }
  len = m_branch->GetEntryNumber();
  return StatusCode::SUCCESS;
}

StatusCode RootConnection::setContainer(const std::string& container, const std::string& type) {
  if (m_file == 0) {
    return StatusCode::FAILURE;
  }
  std::string treeName(container), branchName;
  for (std::string::iterator itr = treeName.begin(), iend = treeName.end(); itr != iend; ++itr)    {
    if (*itr == '/') *itr = '_';
  }
  std::string::size_type inx1 = container.find('(');
  if (inx1 != std::string::npos) {
    std::string::size_type inx2 = container.find(')');
    if (inx2 == std::string::npos || inx2 != container.size() - 1) {
      return StatusCode::FAILURE;
    }
    branchName = treeName.substr(inx1 + 1, inx2 - inx1 - 1);
    treeName = treeName.substr(0, inx1);
  }
  if (m_tree == 0) {
    m_tree = (TTree*)m_file->Get(treeName.c_str());
  }
  if (m_tree == 0 && m_file->IsWritable()) {
    int splitlevel = 0; //FIXME: Make configurable
    m_file->cd("/");
    m_tree = new TTree(treeName.c_str(), "Main event data tree", splitlevel);
    m_tree->SetDirectory(m_file);
    m_tree->Reset();
  }
  if (!m_tree)
    return StatusCode::FAILURE;
  m_branch = m_tree->GetBranch(branchName.c_str());
  RootType root_type = RootType::ByName(type);
  m_branchTypeCode = ::typecode_from_typeid(root_type.TypeInfo());
  if (m_branch == 0 && m_file->IsWritable()) {
    int bufsize = 32000; //FIXME: Make configurable
    int splitlevel = 0; //FIXME: Make configurable
    if (m_branchTypeCode == '\0') {
      m_branch = m_tree->Bronch(branchName.c_str(),
				type.c_str(),
				0,
				bufsize,
				splitlevel);
    } else {
      m_branch = m_tree->Branch(branchName.c_str(),
				0,
				(branchName + "/" + m_branchTypeCode).c_str(),
				bufsize);
    }
    if (m_branch != 0) {
      // let the Athena framework deal with deletion
      m_branch->SetAutoDelete(kFALSE);
      int level = 1; //FIXME: Make configurable
      m_branch->SetCompressionLevel(level);
    }
  }
  if (m_branch == 0) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

}//> namespace Athena
