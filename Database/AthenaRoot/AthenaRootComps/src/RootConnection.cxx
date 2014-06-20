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
#include "RootGlobalsRestore.h"

// fwk includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "DataModelRoot/RootType.h"

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TBranch.h"
#include "TBranchElement.h"
#include "TLeaf.h"

namespace {

  char typecode_from_typeid(const std::type_info &ti)
  {
    if        (ti == typeid(char*))     { return 'C';
    } else if (ti == typeid(Char_t))    { return 'B';
    } else if (ti == typeid(UChar_t))   { return 'b';
    } else if (ti == typeid(Short_t))   { return 'S';
    } else if (ti == typeid(UShort_t))  { return 's';
    } else if (ti == typeid(Int_t))     { return 'I';
    } else if (ti == typeid(UInt_t))    { return 'i';
    } else if (ti == typeid(Float_t))   { return 'F';
    } else if (ti == typeid(Double_t))  { return 'D';
    } else if (ti == typeid(Long64_t))  { return 'L';
    } else if (ti == typeid(ULong64_t)) { return 'l';
    } else if (ti == typeid(Bool_t))    { return 'O';
    } else {
      return '\0';
    }
  }
} //> anon-namespace

namespace Athena {

RootConnection::RootConnection(const IInterface* own,
                               IIoSvc::Fd fd) :
  m_fd(fd),
  m_fid(),
  m_pfn(),
  m_age(0),
  m_owner(own),
  m_file(NULL),
  m_tree(NULL),
  m_branch(NULL),
  m_treeName("CollectionTree"),
  m_descr()
{
  m_pfn = ServiceHandle<IIoSvc>("AthIoSvc", "RootConnection")->fname(fd);
}

RootConnection::~RootConnection()
{
  for (BranchDescriptors_t::iterator
	 itr = m_descr.begin(),
	 iend= m_descr.end();
       itr != iend;
       ++itr) {
    delete itr->second; itr->second = 0;
  }
}

StatusCode
RootConnection::connectRead()
{
  m_file = TFile::Open(m_pfn.c_str());
  TDirectory::TContext dir_ctx(m_file);

  if ( m_file && !m_file->IsZombie() )   {
    return StatusCode::SUCCESS;
  }
  else if ( m_file ) {
    delete m_file; m_file = NULL;
  }
  return StatusCode::FAILURE;
}

StatusCode
RootConnection::connectWrite(IIoSvc::IoType mode)
{
  // std::cerr << "::RootConnection::connectWrite(" << m_pfn
  //           << "|" << m_treeName
  //           << ")...\n";
  if (m_file == NULL) {
    delete m_tree; m_tree = NULL;
    m_file = TFile::Open(m_pfn.c_str(),
                         IIoSvc::IoTypeName(mode).c_str(),
                         "AthenaRoot event data file");
  }

  if (m_file == NULL) {
    return StatusCode::FAILURE;
  }

  if (m_tree == NULL) {
    RootGlobalsRestore rgr;

    int splitlevel = 0;
    m_file->cd("/");
    m_tree = new TTree(m_treeName.c_str(), "Main event data tree", splitlevel);
    m_tree->SetDirectory(m_file);
    m_tree->Reset();
  }
  return StatusCode::SUCCESS;
}

StatusCode RootConnection::addMetadata (const std::string& key,
                                        const void* obj,
                                        const std::type_info& ti)
{
  // Reject null pointers
  if (obj==0) {
    REPORT_MESSAGE (MSG::ERROR) << "Attempt to write null pointer metadata";
    return StatusCode::FAILURE;
  }
  // Determine directory to write into
  std::string metaname;
  TDirectory* dir = 0;
  if (m_tree != 0) {
    dir = m_tree->GetDirectory();
  }
  else { 
    REPORT_MESSAGE (MSG::WARNING) << "No TTree to navigate from";
    dir = m_file->CurrentDirectory();
    if (dir == 0) {
      REPORT_MESSAGE (MSG::ERROR) << "No file directory to navigate from";
      return StatusCode::FAILURE;
    }
  }
  std::string thekey = key;
  std::string::size_type sep = key.find('/');
  if (sep != std::string::npos) {
    metaname = key.substr (0, sep);

    if (m_tree != 0) {
      // Go to the root directory of the file.
      while (dynamic_cast<TDirectoryFile*> (dir) != 0 &&
             dir->GetMotherDir())
        dir = dir->GetMotherDir();
      thekey = m_tree->GetName();
    }
  }
  else {
    if (m_tree != 0) {
      metaname = m_tree->GetName();
    }
    else {
      metaname = "eventless";
    }
    metaname += "Meta";
  }

  TDirectory::TContext ctx (dir);
  TDirectory* metadir = dir->GetDirectory (metaname.c_str());
  if (!metadir) {
    metadir = dir->mkdir (metaname.c_str());
    if (!metadir) {
      REPORT_MESSAGE (MSG::ERROR)
        << "Can't create metadata dir " << metaname
        << "in dir " << dir->GetName();
      return StatusCode::RECOVERABLE;
    }
  }

  // Set info and casts for appropriate type
  TClass* cls = gROOT->GetClass(ti);
  if (!cls)
    return StatusCode::RECOVERABLE;

  // If we're writing strings in a common directory, make sure name is unique.
  if (ti == typeid(TString) || ti == typeid(std::string)) {
    TObjString ostmp;
    if (ti == typeid(TString)) {
      ostmp.String() = *reinterpret_cast<const TString*> (obj);
      obj = &ostmp;
      cls = gROOT->GetClass ("TObjString");
    }
    else if (ti == typeid(std::string)) {
      ostmp.String() = *reinterpret_cast<const std::string*> (obj);
      obj = &ostmp;
      cls = gROOT->GetClass ("TObjString");
    }
    if (key.size() > 0 && key[key.size()-1] == '/') {
      int i = 1;
      while (metadir->FindObject (thekey.c_str())) {
        ++i;
        std::ostringstream ss;
        ss << m_tree->GetName() << "-" << i;
        thekey = ss.str();
      }
    }

    // Treat like hadd and create new version each time
    if (metadir->WriteObjectAny (obj, cls, thekey.c_str(), "new") == 0) {
      REPORT_MESSAGE (MSG::ERROR)
        << "Can't write metadata object " << thekey
        << " for file " << metadir->GetFile()->GetName();
      return StatusCode::RECOVERABLE;
    }
  }
  else if (ti == typeid(TTree)) {
    TTree* readTree = ((const TTree*)obj)->GetTree();
    metadir->cd();

    // If we're writing in a common directory, make sure name is unique.
    if (key.size() > 0) {
      TTree* outTree = (TTree*)metadir->FindObject (thekey.c_str());
      if (outTree == 0) {
        outTree = readTree->CloneTree();
      }
      else {
        TList tc;
        tc.Add((TObject*)readTree);
        Long64_t temp = outTree->Merge((TCollection*)&tc);
        if (temp==0) {
          REPORT_MESSAGE (MSG::ERROR) << "Unable to merge with existing tree in file";
          return StatusCode::RECOVERABLE;
        }
      }
      outTree->Write();
    }
    else {
      REPORT_MESSAGE (MSG::ERROR) << "Did not use proper key for metadata tree ";
      return StatusCode::RECOVERABLE;
    }
  }
  else {
    REPORT_MESSAGE (MSG::ERROR) << "addMetadata typeid not supported";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode
RootConnection::commit()
{
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

StatusCode
RootConnection::disconnect()
{
  if (!this->commit().isSuccess()) {
    return StatusCode::FAILURE;
  }
  if (m_tree != NULL && m_file->IsWritable()) {
    RootGlobalsRestore rgr;
    m_file->cd("/");
    m_tree->Write(); 
    m_tree = NULL;
  }
  m_file->Close(); m_file = NULL;
  return StatusCode::SUCCESS;
}

bool
RootConnection::isConnected() const
{
   return false;
}

StatusCode
RootConnection::read(void* const /*data*/, size_t /*len*/)
{
   return StatusCode::FAILURE;
}

StatusCode
RootConnection::write(const void* data, unsigned long& len)
{
  if (m_file == 0 || m_tree == 0 || m_branch == 0) {
    return StatusCode::FAILURE;
  }
  BranchDescr* descr = m_descr[m_branch];
  descr->buffer = (void*)data;
  if (descr->typecode == '\0') {
    m_branch->SetAddress(&descr->buffer);
  } else {
    m_branch->SetAddress(descr->buffer);
  }
  int32_t nbytes = m_branch->Fill();
  if (nbytes < 0) {
    return StatusCode::FAILURE;
  }
  len = m_branch->GetEntryNumber();
  return StatusCode::SUCCESS;
}

StatusCode
RootConnection::setContainer(const std::string& container,
                             const std::string& type,
                             const void* data)
{
  if (m_file == 0 || m_tree == 0) {
    return StatusCode::FAILURE;
  }

  const std::string& name = container;
  m_branch = m_tree->GetBranch(name.c_str());
  if (m_branch == 0 && m_file->IsWritable()) {
    int bufsize = 32000;	//FIXME: Make configurable
    int splitlevel = 0;	//FIXME: Make configurable
    RootType root_type = RootType::ByName(type);
    BranchDescr *brdescr = new BranchDescr;
    brdescr->typecode = ::typecode_from_typeid(root_type.TypeInfo());
    brdescr->buffer = ((void*)data);
    if (brdescr->typecode == '\0') {
      m_branch = m_tree->Bronch((name+"").c_str(),
				type.c_str(),
				(void*)&brdescr->buffer,
				bufsize,
				splitlevel);
    } else {
      m_branch = m_tree->Branch(name.c_str(),
				(void*)brdescr->buffer,
				(name+"/"+brdescr->typecode).c_str(),
				bufsize);
    }
    if (m_branch != 0) {
      // let the Athena framework deal with deletion
      m_branch->SetAutoDelete(kFALSE);

      int level = 1;	//FIXME: Make configurable
      m_branch->SetCompressionLevel(level);

      m_descr.insert(std::make_pair(m_branch, brdescr));
    } else {
      delete brdescr; brdescr = NULL;
    }
  }
  if (m_branch == 0) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void
RootConnection::setTreeName(const std::string& n)
{
  m_treeName = n;
}

}//> namespace Athena
