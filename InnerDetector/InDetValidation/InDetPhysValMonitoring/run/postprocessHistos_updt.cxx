/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ____________________________________________________________________________

  merge root files from several (grid) runs
  dedicated treatment of profiles and efficiencies
  Author: Liza Mijovic
   ____________________________________________________________________________
*/

#define RLN cout << "l" << __LINE__ << ": ";
// 0..3 in increasing verbosity
#define PRINTDBG 2

#include <iostream>
#include <iterator>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TEfficiency.h"
#include "TMath.h"
#include "TROOT.h"
#include "TKey.h"
#include "TClass.h"
#include "TObject.h"
#include "TObjString.h"

using namespace std;

namespace po = boost::program_options;

int process_th1(TH1* p_h1, TH1* p_h2) {
  p_h1->Add(p_h2);
  return 0;
}

int process_efficiency(TEfficiency* p_e1, TEfficiency* p_e2) {
  if (PRINTDBG>2)
    cout << "adding efficiency " << p_e1->GetName() << endl;
  double pre1 = p_e1->GetTotalHistogram()->GetEntries();
  double pre2 = p_e2->GetTotalHistogram()->GetEntries();
  p_e1->Add(*p_e2);
  double post = p_e1->GetTotalHistogram()->GetEntries();
  if (PRINTDBG>2)
    cout << "pre1 pre2 post " << pre1 << " + " << pre2 << " = " << post << endl;
  return 0;  
}

int process_profile(TProfile* p_p1,TProfile* p_p2) {
  // TODO
  return 0;
}
int process_object(string p_objname,TFile* p_ftarget,TFile* p_ftosumm,bool p_second_pass) {
  int ret=0;
  if (PRINTDBG>2)
    cout << "process_object p_objname " << p_objname << endl;
  TObject* o1 = (TObject*)p_ftarget->Get(p_objname.c_str());
  TObject* o2 = (TObject*)p_ftosumm->Get(p_objname.c_str());
  if (!o1 || !o2) 
    return 1;
  if (p_second_pass) {
    if (o1->IsA()->InheritsFrom(TProfile::Class())) {
      if (PRINTDBG>2)
	cout << "profile " << o1->GetName() << endl;
      TProfile *p1 = dynamic_cast<TProfile*>(o1);
      TProfile *p2 = dynamic_cast<TProfile*>(o2);     
      ret=process_profile(p1,p2);
      if (!ret)
	 p1->Write(p_objname.c_str(),TObject::kOverwrite);
    }
  }
  else {
    if (o1->IsA()->InheritsFrom(TEfficiency::Class())) {
      if (PRINTDBG>2)
	cout << "TEfficiency " << o1->GetName() << endl;
      TEfficiency *e1 = dynamic_cast<TEfficiency*>(o1);
      TEfficiency *e2 = dynamic_cast<TEfficiency*>(o2);
      ret=process_efficiency(e1,e2);
      double post = e1->GetTotalHistogram()->GetEntries();
      if (PRINTDBG>2)
	cout << "postwrote " << post << endl;
      if (!ret)
	e1->Write(p_objname.c_str(),TObject::kOverwrite);
    }
    else if (o1->IsA()->InheritsFrom(TH1::Class())) {
      if (PRINTDBG>2)
	cout << "TH1 " << o1->GetName() << endl;
      TH1* h1 = dynamic_cast<TH1*>(o1);
      TH1* h2 = dynamic_cast<TH1*>(o2);
      ret=process_th1(h1,h2);
      if (!ret)
	h1->Write(p_objname.c_str(),TObject::kOverwrite);
    }
  }
  return ret;
}


void get_all_objects(TDirectory* const p_source, vector<string>& p_all_objects,string p_dirup="") {

  TIter nextkey(p_source->GetListOfKeys());
  TDirectory *savdir = gDirectory;
  TKey *key;
  while ((key=(TKey*)nextkey())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      p_source->cd(key->GetName());
      TDirectory *subdir = gDirectory;
      string oneup=subdir->GetName();
      string dirup = (""!=p_dirup) ? p_dirup + "/" + oneup : oneup;
      get_all_objects(subdir,p_all_objects,dirup);      
    }
    p_all_objects.push_back(p_dirup+"/"+string(key->GetName()));
    savdir->cd();
  }
  return;
}

bool file_exists(const string p_name) {
  return (gSystem->AccessPathName(p_name.c_str(), kFileExists))?
    false : true;    
}

int process_histos(string p_ofile, string p_infile) {
  if (PRINTDBG>0)
    cout << "merging file " << p_infile << " into "<< p_ofile << endl;
  
  // output file pre-exists by construction;  
  // at start of summing the output is created copy of 1st input
  TFile* ofile = new TFile(p_ofile.c_str(),"UPDATE"); 
  if (! ofile ) {
    cout << "could not open file for update "<< endl;
    cout << p_ofile << endl;
    return 1;
  }

  vector<string> all_objects;
  TDirectory* topdir=gDirectory;
  get_all_objects(topdir,all_objects);

  TFile* const infile = new TFile(p_infile.c_str(),"READ"); 
  if (! infile ) {
    cout << "could not open input file for reading "<< endl;
    cout << p_infile << endl;
    return 1;
  }

  // process histograms and efficiencies, that can be summed directly
  bool second_pass = false;
  for (auto obj : all_objects) {
    if (PRINTDBG>2)
      cout << "running object  " << obj << endl;
    int ret=process_object(obj,ofile,infile,second_pass);
    if (0!=ret) {
      cout << "Error processing " << obj << endl;
    }
  }
  // process TProfiles, that require pre-summed 2d-histograms
  second_pass = true;
  for (auto obj : all_objects) {
    if (PRINTDBG>2)
      cout << "running object 2nd pass " << obj;
    int ret=process_object(obj,ofile,infile,second_pass);
    if (0!=ret) {
      cout << "Error processing " << obj << endl;
    }  
  }  
  if (PRINTDBG>1)
    cout << "loop all done " << endl;

  ofile->Close();
  infile->Close();

  return 0;
}

int main(int ac, char* av[]) {
  
  vector<string> infiles;
  string ofile="";
  
  try {
    po::options_description desc("Allowed arguments: ");
    desc.add_options()
      ("h", "print help")
      ("o", po::value<string>(), "output root file 'summ.root'")
      ("i", po::value<string>(), "inputs 'file1.root,file2.root,file3.root'")
      ;
    
    po::variables_map vm;        
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    
    
    if (vm.count("h")) {

      cout << "\n" << desc << "\n" << endl;
      const char* ex_run = "./postprocessHistos_updt --i \"summ.root\" "
	"--o \"file1.root,file2.root,file3.root\" ";
      cout << "\n------- run -------" << endl;
      cout << ex_run << endl;
      cout << "\n";
      return 0;
    }
    
    if (vm.count("o")) {
      ofile=vm["o"].as<string>();
      cout << "Output file set to "<< ofile << endl;
    }
    
    if (vm.count("i")) {
      string infiles_cl=vm["i"].as<string>();
      boost::split(infiles , infiles_cl, boost::is_any_of(","));
      cout << "Summing input files ";
      for (auto inf : infiles)
       	cout << inf << " " ;
      cout << endl;
    }

    if ("" == ofile) {
      cout << "Invalid arguments. No output file passed.";
      cout << "\n" << desc <<"\n" << endl;
      return 1;
    }
    
    if (infiles.empty()) {
      cout << "Invalid arguments. No inputs to summ passed.";
      cout << desc <<" \n " << endl;
      return 1;
    }
    
  }
  catch(exception& e) {
    cerr << __FILE__  << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << __FILE__  << "Exception of unknown type!\n";
    return 1;
  }

  if (file_exists(ofile)) {
    cout << "\n" << __FILE__ << "Target file exist: " << ofile << endl;
    cout << "Please provide an empty target file \n"<< endl;
    return 1;
  }
    
  int processed = 0; 
  for (auto infile : infiles) {
    if (0==processed) {
      if (!file_exists(infile)) {
      	cout << __FILE__ << "Error: input file does not exist: " << infile << endl;
	return 1;	
      }     
      cout << "\n" << __FILE__ << " Creating initial contents of " << ofile << endl;
      cout << "from " << infile << endl;
      gSystem->CopyFile(infile.c_str(),ofile.c_str());
    }
    else {
      int ret = process_histos(ofile,infile);
      if (0 != ret) {
	cout << "\n" << __FILE__ << " Non-0 return of process_histos for file " << endl;
	cout << infile << endl;
	cout << "Terminate postprocessing \n" << endl;
	return 1;
      }
    }
    ++processed;
    cout << "\n" << __FILE__ << " Post-processed " << processed << " files " << endl; 
  }
  cout << "\n" << __FILE__ << " Merging all done \n " << endl;
  return 0;
  
}

