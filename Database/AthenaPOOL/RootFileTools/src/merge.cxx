/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/Guid.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TLeaf.h"
#include "TKey.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTreeCloner.h"
#include <iostream>
#include <iomanip>

// C++ includes 
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

static bool s_dbg = true;

typedef int DbStatus;
enum { ERROR=0, SUCCESS=1 };

struct DbContainerSection {
  int start;
  int length;
  int offset;
};

class DbDatabaseMerger {

  typedef std::vector<DbContainerSection>           DbContainerSections;
  typedef std::map<std::string,DbContainerSections> DbDatabaseSections;

  DbDatabaseSections m_sections;
  std::map<std::string,size_t> m_sectionsCounts;
  std::set<std::string> m_fids;
  TFile*             m_output;
  size_t             m_sectionsMin;
  size_t             m_sectionsMax;
  TTree*             m_sectionTree;
  TBranch*           m_sectionBranch;
  size_t             m_paramsMin;
  TTree*             m_paramTree;
  TBranch*           m_paramBranch;

public:
  /// Standard constructor
  DbDatabaseMerger();
  /// Default destructor
  virtual ~DbDatabaseMerger();
  /// Check if input needs adding
  bool empty(const std::string& fid, const std::set<std::string>& exclTrees, bool dbg=true) const;
  /// Check if a database exists
  bool exists(const std::string& fid, bool dbg=true) const;
  /// Create new output file
  DbStatus create(const std::string& fid);
  /// Attach to existing output file for further merging
  DbStatus attach(const std::string& fid);
  DbStatus addBranches(TObjArray *from, TObjArray *to, Long64_t fromSize);
  /// Close output file
  DbStatus close();
  /// Merge new input to existing output
  DbStatus merge(const std::string& fid, const std::set<std::string>& exclTrees);
  /// Dump collected database sections
  void dumpSections();
  /// Save new sections to the output file
  DbStatus saveSections();
  DbStatus createFID();
};

namespace {
  const char* getLinkContainer(char* txt) {
    char* p = ::strstr(txt,"[CNT=");
    if ( p ) {
      char* q = strchr(p+4,']');
      *q = 0;
      return p+5;
    }
    return 0;
  }
  string getRootContainer(const char* t) {
    string r;
    for(const char* p=t; *p; ++p) {
      r += (*p == '/') ? '_' : *p;
    }
    return r;
  }
}

/// Standard constructor
DbDatabaseMerger::DbDatabaseMerger() : m_output(0), m_sectionsMin(0), m_sectionsMax(0), m_sectionTree(0), m_sectionBranch(0), m_paramsMin(0), m_paramTree(0), m_paramBranch(0) {
}

/// Default destructor
DbDatabaseMerger::~DbDatabaseMerger() {
  close();
}

/// Check if input needs adding
bool DbDatabaseMerger::empty(const std::string& fid, const std::set<std::string>& exclTrees, bool dbg) const {
  TFile* source = TFile::Open(fid.c_str());
  if ( source && !source->IsZombie() ) {
    TIter nextkey(source->GetListOfKeys());
    for ( TKey* key = (TKey*)nextkey(); key; key = (TKey*)nextkey() ) {
	  const string name = key->GetName();
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TTree")) {
        if (exclTrees.find(name) != exclTrees.end()) continue;
        if (name.substr(0, 2) == "##") continue;
        return false;
      }
    }
    if ( dbg ) cout << "file " << fid << " HAS NO ENTRIES!" << endl;
    delete source;
    return true;
  }
  cout << "file " << fid << " DOES NOT EXIST!" << endl;
  delete source;
  return true;
}

/// Check if a database exists
bool DbDatabaseMerger::exists(const std::string& fid, bool dbg) const {
  Bool_t result = gSystem->AccessPathName(fid.c_str(), kFileExists);
  if ( result == kFALSE ) {
    if ( s_dbg ) cout << "file " << fid << " EXISTS!" << endl;
  } else if ( dbg ) {
    cout << "file " << fid << " DOES NOT EXIST!" << endl;
  }
  return result == kFALSE;
}

/// Attach to existing output file for further merging
DbStatus DbDatabaseMerger::attach(const string& fid) {
  if ( !exists(fid) ) {
    cout << "+++ Cannot attach output file " << fid << " --- file does not exist." << endl;
    return ERROR;
  }
  if ( !m_output ) {
    m_output = TFile::Open(fid.c_str(),"UPDATE");
    if ( !m_output ) {
      cout << "+++ Failed to open new output file " << fid << "." << endl;
      return ERROR;
    }
    if ( s_dbg ) cout << "+++ Opened new output file " << fid << "." << endl;
    m_paramTree = (TTree*)m_output->Get("##Params");
    if ( 0 != m_paramTree ) m_paramBranch = m_paramTree->GetBranch("db_string");
    m_sectionTree = (TTree*)m_output->Get("##Sections");
    if ( 0 != m_sectionTree ) m_sectionBranch = m_sectionTree->GetBranch("db_string");
  }
  if ( 0 == m_paramBranch || 0 == m_sectionBranch ) {
    close();
    cout << "+++ Existing output file " << fid << " is no POOL file --- Request denied." << endl;
    delete m_output;
    m_output = 0;
    return ERROR;
  }
  if ( m_output && !m_output->IsZombie() ) {
    char text[1024];
    m_paramBranch->SetAddress(text);
    for(size_t n=(size_t)(m_paramBranch->GetEntries()); m_paramsMin<n; ++m_paramsMin) {
      m_paramBranch->GetEntry(m_paramsMin);
      string dsc = text;
      size_t id1 = dsc.find("[NAME=FID");
      if (id1 != string::npos) m_sectionsMax++;
    }
    m_sectionBranch->SetAddress(text);
    for(size_t n=(size_t)(m_sectionBranch->GetEntries()); m_sectionsMin<n; ++m_sectionsMin) {
      m_sectionBranch->GetEntry(m_sectionsMin);
      string dsc = text;
      size_t id1 = dsc.find("[CNT=");
      size_t id2 = dsc.find("]");
      if (id1 != string::npos && id2 != string::npos) {
        string cnt = dsc.substr(id1+5, id2-5);
        std::map<std::string, size_t>::iterator it = m_sectionsCounts.find(cnt);
        if (it != m_sectionsCounts.end()) {
          it->second++;
        } else {
          m_sectionsCounts[cnt] = 1;
        }
      }
    }
    return SUCCESS;
  }
  cout << "+++ Failed to open new output file " << fid << "." << endl;
  return ERROR;
}

/// Create new output file
DbStatus DbDatabaseMerger::create(const string& fid) {
  if ( m_output ) {
    cout << "+++ Another output file " << m_output->GetName() << " is already open. Request denied." << endl;
    return ERROR;
  } else if ( exists(fid) ) {
    cout << "+++ Cannot create output file " << fid << " --- file already exists." << endl;
    return ERROR;
  }
  m_output = TFile::Open(fid.c_str(),"RECREATE");
  if ( m_output && !m_output->IsZombie() )     {
    m_sectionTree   = new TTree("##Sections","##Sections");
    m_sectionBranch = m_sectionTree->Branch("db_string",0,"db_string/C");
    m_paramTree   = new TTree("##Params","##Params");
    m_paramBranch = m_paramTree->Branch("db_string",0,"db_string/C");
    if ( s_dbg ) cout << "+++ Opened new output file " << fid << "." << endl;
    return SUCCESS;
  }
  cout << "+++ Failed to open new output file " << fid << "." << endl;
  return ERROR;
}

/// Close output file
DbStatus DbDatabaseMerger::createFID() {
  if ( m_output ) {
    if ( m_paramTree && m_paramBranch ) {
      Guid uuid;
      Guid::create(uuid);
      std::string text = "[NAME=FID][VALUE=" + uuid.toString() + "]";
      m_paramBranch->SetAddress((void*)text.c_str());
      m_paramTree->Fill();
      if ( s_dbg ) cout << "+++ Added new GUID " << text << " to merge file." << endl;
      return SUCCESS;
    }
  }
  cout << "+++ Failed to add new GUID to merge file." << endl;
  return ERROR;
}

/// Close output file
DbStatus DbDatabaseMerger::close() {
  if ( m_output ) {
    m_output->Write();
    m_output->Purge();
    if ( s_dbg ) m_output->ls();
    m_output->Close();
    delete m_output; m_output = 0;
  }
  return SUCCESS;
}

/// Save new sections to the output file
DbStatus DbDatabaseMerger::saveSections()     {
  if ( m_sectionBranch ) {
    char text[1024];
    m_sectionBranch->SetAddress(text);
    for(DbDatabaseSections::const_iterator i=m_sections.begin(); i!= m_sections.end();++i) {
      int cnt = 0;
      string cont = (*i).first;
      const DbContainerSections& cntSects = (*i).second;
      for(DbContainerSections::const_iterator j=cntSects.begin(); j != cntSects.end();++j, ++cnt) {
	::sprintf(text,"[CNT=%s][OFF=%d][START=%d][LEN=%d]", cont.c_str(),(*j).offset,(*j).start,(*j).length); 
	m_sectionTree->Fill();
      }
    }
    TTree *links = (TTree*)m_output->Get("##Links");
    if ( !links ) {
      return ERROR;
    }
    Long64_t len = links->GetEntries();
    TBranch* blinks = links->GetBranch("db_string");
    if ( !blinks ) {
      return ERROR;
    }
    blinks->SetAddress(text);
    for(Long64_t i=0; i<len; ++i) {
      links->GetEntry(i);
      if ( strstr(text,"CNT=##Sections") != 0 ) {
	return SUCCESS;
      }
    }
    ::sprintf(text,"[DB=00000000-0000-0000-0000-000000000000][CNT=##Sections][CLID=DA8F479C-09BC-49D4-94BC-99D025A23A3B][TECH=00000200][OID=%08X-FFFFFFFF]",
	      int(len+2));
    links->Fill();
    //links->Write();
    return SUCCESS;
  }
  return ERROR;
}

/// Dump collected database sections
void DbDatabaseMerger::dumpSections() {
  for(DbDatabaseSections::const_iterator i=m_sections.begin(); i!= m_sections.end();++i) {
    int cnt = 0;
    string prefix = (*i).first;
    const DbContainerSections& cntSects = (*i).second;
    for(DbContainerSections::const_iterator j=cntSects.begin(); j != cntSects.end();++j, ++cnt) {
      char text[32];
      ::sprintf(text,"'][%d]",cnt); 
      if ( s_dbg ) {
	cout << "+++ " << setw(60) << left << "section['"+prefix+text
	     << "  Link offset:" << setw(5) << right << (*j).offset 
	     << "  Start:" << setw(8) << right << (*j).start 
	     << " ... "    << setw(8) << right << (*j).start+(*j).length 
	     << " ["       << (*j).length << "  entries]." << endl;
      }
    }
  }
}

DbStatus DbDatabaseMerger::addBranches(TObjArray *from, TObjArray *to, Long64_t fromSize) {
  Int_t fnb = from->GetEntries();
  Int_t tnb = to->GetEntries();
  Int_t fi = 0;
  Int_t ti = 0;
  while (ti < tnb) {
    TBranch* fb = (TBranch*) from->UncheckedAt(fi);
    TBranch* tb = (TBranch*) to->UncheckedAt(ti);
    Int_t firstfi = fi;
    while (strcmp(fb->GetName(), tb->GetName())) {
      ++fi;
      if (fi >= fnb) {
        // continue at the beginning
        fi = 0;
      }
      if (fi==firstfi) {
        // We tried all the branches and there is not match.
        fb = 0;
	if ( s_dbg ) cout << "+++ Created new Branch for " << tb->GetName() << endl;
        for (Long64_t i=0; i<fromSize; i++) {
          tb->SetAddress(0);
          tb->Fill();
        }
        break;
      }
      fb = (TBranch*) from->UncheckedAt(fi);
    }
    ++ti;
  }
  return SUCCESS;
}

/// Merge new input to existing output
DbStatus DbDatabaseMerger::merge(const string& fid, const std::set<std::string>& exclTrees) {
  if ( m_output ) {
    TFile* source = TFile::Open(fid.c_str());
    if ( source && !source->IsZombie() ) {
      char text[1024];
      if ( m_paramTree && m_paramBranch ) {
        m_paramBranch->SetAddress(text);
        for(size_t i=0, n=(size_t)(m_paramTree->GetEntries()); i<n; ++i) {
          m_paramBranch->GetEntry(i);
          string dsc = text;
          if (dsc.find("[NAME=FID") != string::npos) m_fids.insert(dsc.substr(17, 36));
        }
      }

      TTree *src_params = (TTree*)source->Get("##Params");
      if ( src_params ) {
        TBranch* src_bparams = src_params->GetBranch("db_string");
        src_bparams->SetAddress(text);
        if (src_params->GetEntries() > 4) {
	  cout << "+++ Detected fast merged input file -- operation failed." << endl;
	  return ERROR;
        }
        for(size_t i=0, n=(size_t)(src_params->GetEntries()); i<n; ++i) {
          src_bparams->GetEntry(i);
          string dsc = text;
          if (dsc.find("[NAME=FID") != string::npos && m_fids.insert(dsc.substr(17, 36)).second == 0) {
	    cout << "+++ Detected duplicated file GUID -- operation failed." << endl;
	    return ERROR;
          }
        }
      }
      TTree *links = (TTree*)m_output->Get("##Links");
      Long64_t lnk_offset = links ? links->GetEntries() : 0;
      TTree* src_links = (TTree*)source->Get("##Links");
      TBranch* src_blinks = src_links->GetBranch("db_string");
      src_blinks->SetAddress(text);
      map<string,string> redirect;
      redirect["##Links"]  = "##Links";
      redirect["##Shapes"] = "##Shapes";
      for(size_t i=0, n=(size_t)(src_links->GetEntries()); i<n; ++i) {
	src_links->GetEntry(i);
	if ( s_dbg ) cout << text << endl;
	const char* l = getLinkContainer(text);
	if ( l ) redirect[getRootContainer(l)] = l;
      }
      src_blinks->ResetAddress();
      m_output->cd();
      TIter nextkey(source->GetListOfKeys());
      for(TKey* key = (TKey*)nextkey(); key; key = (TKey*)nextkey() ) {
	const char *classname = key->GetClassName();
	TClass *cl = gROOT->GetClass(classname);
	if (!cl) continue;
	if (cl->InheritsFrom("TTree")) {
	  const string name = key->GetName();
	  if (exclTrees.find(name) != exclTrees.end()) continue;
	  source->cd();
	  TTree *src_tree = (TTree*)source->Get(key->GetName());
	  Long64_t src_entries = src_tree->GetEntries();
	  m_output->cd();
	  DbContainerSection s;
	  s.start = 0;
	  s.length = (int)src_entries;
	  s.offset = (int)lnk_offset;
	  DbContainerSection s0;
	  s0.start = 0;
	  s0.length = 0;
	  s0.offset = 0;

	  TTree *out_tree = (TTree*)m_output->Get(key->GetName());
	  if ( out_tree == 0 ) {
	    out_tree = src_tree->CloneTree(-1,"fast");
	    if ( s_dbg ) cout << "+++ Created new Tree " << out_tree->GetName() << endl;
	  } else {
	    Long64_t out_entries = out_tree->GetEntries();
	    m_output->GetObject(key->GetName(),out_tree);
            if (name == "##Params") {
              if ( s_dbg ) cout << "+++ Slow merge for " << name << endl;
              Long64_t out_entries = out_tree->GetEntries();
              s.start = (int)out_entries;
              out_tree->CopyAddresses(src_tree);
              for (Long64_t i=0; i<src_entries; i++) {
                src_tree->GetEntry(i);
                out_tree->Fill();
              }
              src_tree->ResetBranchAddresses();
            } else {
              addBranches(out_tree->GetListOfBranches(), src_tree->GetListOfBranches(), out_entries);
	      TTreeCloner cloner(src_tree,out_tree,"fast", TTreeCloner::kNoWarnings|TTreeCloner::kIgnoreMissingTopLevel);
	      if (cloner.IsValid()) {
	        Long64_t out_entries = out_tree->GetEntries();
	        s.start = (int)out_entries;
	        out_tree->SetEntries(out_entries+src_entries);
	        Bool_t res = cloner.Exec();
	        if ( s_dbg ) cout << "+++ Merged tree: " << out_tree->GetName() << " res=" << res << endl;
	      } else {
	        // Fast cloning is not possible for this input TTree.
	        // ... see TTree::CloneTree for example of recovery code ...
	        cout << "+++ Got a tree where fast cloning is not possible -- operation failed." << endl;
	        return ERROR;
	      }
              addBranches(src_tree->GetListOfBranches(), out_tree->GetListOfBranches(), src_entries);
            }
	  }
	  map<string,string>::const_iterator ir=redirect.find(name);
	  if ( ir != redirect.end() ) {
            while (m_sectionsCounts[(*ir).second] + m_sections[(*ir).second].size() < m_sectionsMax) {
	      m_sections[(*ir).second].push_back(s0);
            }
	    m_sections[(*ir).second].push_back(s);
	  } else {
	    for (int ileaf = 0; ileaf < src_tree->GetListOfLeaves()->GetEntries(); ileaf++) {
	      TLeaf* leaf = (TLeaf*)src_tree->GetListOfLeaves()->At(ileaf);
	      TBranch* branch = leaf->GetBranch();
	      map<string,string>::const_iterator ir=redirect.find(name + "(" + branch->GetName() + ")");
	      if ( ir != redirect.end() ) {
                while (m_sectionsCounts[(*ir).second] + m_sections[(*ir).second].size() < m_sectionsMax) {
	          m_sections[(*ir).second].push_back(s0);
                }
		m_sections[(*ir).second].push_back(s);
	      }
	    }
	  }
	}
	else   {
	  cout << "+++ Ignore key " << key->GetName() << endl;
	}
      }
      delete source;
      m_output->SaveSelf(kTRUE);
      m_output->Purge();
      m_output->cd();
      return SUCCESS;
    }
    cout << "+++ Cannot open input file:" << source << endl;
    m_output->cd();
    return ERROR;
  }
  cout << "+++ No valid output file present. Merge request refused for fid:" << fid << endl;
  return ERROR;
}

static int usage() {
  cout << "POOL merge facility for ROOT tree based files.\n"
    " Usage: \n"
    "mergePool -o <output-file> -i <input-file 1> [ -i <input-file 2> ...] [-e <exclude-tree 1>]\n\n"
    "input- and output files may consist of any legal file name.\n" 
    " -debug       Switch debug flag on.\n"
       << endl;
  return 2;
}

int main(int argc, char** argv) {
  bool dbg = false;
  string output;
  vector<string> input;
  set<string> exclTrees;
  for(int i=1; i < argc; ++i) {
    if ( *argv[i] == '-' ) {
      switch(::toupper(*(argv[i]+1))) {
      case 'D':
	dbg = true;
	break;

      case 'O':
	if ( i+1 < argc ) output = argv[i+1];
	++i;
	break;
	
      case 'I':
	if ( i+1 < argc ) input.push_back(argv[i+1]);
	++i;
	break;

      case 'E':
	if ( i+1 < argc ) exclTrees.insert(argv[i+1]);
	++i;
	break;
	
      default:
	return usage();
      }
    }
  }
  if ( input.empty() ) {
    cout << "No input files supplied" << endl;
    return usage();
  } else if ( output.empty() ) {
    cout << "No output file supplied" << endl;
    return usage();
  }
  s_dbg = dbg;
  gROOT->SetBatch(kTRUE);
  DbDatabaseMerger m;
  for (size_t i=0; i<input.size();++i)  {
    const string& in = input[i];
    bool fixup = ((i+1)==input.size());
    if ( m.empty(in, exclTrees, s_dbg) ) {
      if ( fixup ) m.createFID();
      continue;
    }
    DbStatus ret = m.exists(output.c_str(), s_dbg) ? m.attach(output) : m.create(output);
    if ( ret == SUCCESS ) {
      ret = m.merge(in, exclTrees);
      if ( ret == SUCCESS ) {
        m.dumpSections();
        if ( fixup ) m.createFID();
        continue;
      }
    }
    m.close();
    cout << "+++ Cannot open output file:" << output << endl;
    cout << "File merge failed after " << i+1 << " files.";
    return 2;
  }
  m.saveSections();
  if ( m.close() == ERROR ) {
    cout << "File merge failed to close output file.";
    return 2;
  }
  return 0;
}
