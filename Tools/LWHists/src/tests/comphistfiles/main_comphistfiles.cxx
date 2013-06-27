/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "readfile.h"
#include "comphists.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sys/stat.h>

void classifyByKey( const KeyToHistMap& hists1,KeyToHistMap& hists2,
		    std::vector<std::pair<std::string,TH1*> >& histsOnlyIn1,
		    std::vector<std::pair<std::string,TH1*> >& histsOnlyIn2,
		    std::vector<std::pair<std::string,std::pair<TH1*,TH1*> > >& histsInBoth )
{
  KeyToHistMap::const_iterator it1(hists1.begin()), it1E(hists1.end());
  for (;it1!=it1E;++it1) {
  KeyToHistMap::iterator it2=hists2.find(it1->first);
    if (it2!=hists2.end()) {
      histsInBoth.push_back(std::make_pair(it1->first,std::pair<TH1*,TH1*>(it1->second,it2->second)));
      hists2.erase(it2);
    } else {
      histsOnlyIn1.push_back(*it1);
    }
  }
  KeyToHistMap::const_iterator it2(hists2.begin()), it2E(hists2.end());
  histsOnlyIn2.reserve(hists2.size());
  for (;it2!=it2E;++it2)
    histsOnlyIn2.push_back(*it2);
}

void usage(char**argv,int exitcode) {
  std::cout << "Usage:"<<std::endl;
  std::cout << ""<<std::endl;
  std::cout << argv[0]<<" [flags] histfile1.root histfile2.root [outputfile.root]"<<std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"Program which checks for consistency (up to floating point accuracy)"<<std::endl;
  std::cout <<"between ROOT histograms found in two ROOT files and outputs a small report."<<std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"Supplying an outputfile.root argument will result in copies of inconsistent"<<std::endl;
  std::cout <<"histograms written to this file."<<std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"Unless -h/--help is requested, exit-code 0 implies complete consistency."<<std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"Optional flags:"<<std::endl;
  std::cout <<"              -h, --help : This help"<<std::endl;
  std::cout <<"           -v, --verbose : Verbose output"<<std::endl;
  std::cout <<"             --silentnan : Suppress warnings regarding NaN values (when in both hists compared)."<<std::endl;
  std::cout <<"      --ignore-stylevars : Ignore certain unsupported style-vars (ndivisions, ticklength, ..)."<<std::endl;
  std::cout <<" --ignore-sumw2-nonalloc : Ignore case where one histogram has not allocated a sumw2 (error) array."<<std::endl;
  std::cout <<""<<std::endl;
  exit(exitcode);
}

bool fileExists(const std::string& filename) {
  struct stat stFileInfo;
  return (stat(filename.c_str(),&stFileInfo) == 0);
}

//Returns 0 if check ok, 1 if problems getting histograms from one of the files, 2 if check not ok.
int main(int argc,char**argv){
  //Decode arguments:
  bool verbose(false);
  std::vector<std::string> rootfiles;
  cfg_report_NaN = true;
  cfg_ignore_unsupported_style_vars = false;
  cfg_ignore_errors_related_to_a_non_existant_sumw2_array = false;
  for (int i=1; i<argc;++i) {
    std::string arg(argv[i]);
    if (arg=="-h"||arg=="--help")
      usage(argv,0);
    if (arg=="-v"||arg=="--verbose") {
      if (verbose)
	usage(argv,1);
      verbose = true;
      continue;
    }
    if (arg=="--silentnan") {
      if (!cfg_report_NaN)
	usage(argv,1);
      cfg_report_NaN = false;
      continue;
    }
    if (arg=="--ignore-stylevars") {
      if (cfg_ignore_unsupported_style_vars)
	usage(argv,1);
      cfg_ignore_unsupported_style_vars = true;
      continue;
    }
    if (arg=="--ignore-sumw2-nonalloc") {
      if (cfg_ignore_errors_related_to_a_non_existant_sumw2_array)
	usage(argv,1);
      cfg_ignore_errors_related_to_a_non_existant_sumw2_array = true;
      continue;
    }
    if (arg.empty()||arg[0]=='-')
      usage(argv,1);
    rootfiles.push_back(arg);
  }
  if (rootfiles.size()<2||rootfiles.size()>3)
    usage(argv,1);
  const std::string file1 = rootfiles.at(0);
  const std::string file2 = rootfiles.at(1);
  const std::string outputfile = (rootfiles.size()>2 ? rootfiles.at(2): "");
  if (!fileExists(file1)) {
    std::cout<<"Error: File "<<file1<<" not found."<<std::endl;
    usage(argv,1);
  }
  if (!fileExists(file2)) {
    std::cout<<"Error: File "<<file2<<" not found."<<std::endl;
    usage(argv,1);
  }
  if (!outputfile.empty()&&fileExists(outputfile)) {
    std::cout<<"Error: File "<<outputfile<<" already exists."<<std::endl;
    usage(argv,1);
  }

  //Parameters:
  //  const char * file1 = "Monitor1.root";
  //  const char * file2 = "Monitor1_copy.root";
  //const char * file2 = "Monitor2.root";
  //  const bool verbose = false;
  //  const std::string outputfile="";//"hists_noncompat.root";
  //read both:
  KeyToHistMap hists1, hists2;

  if (!getAllHistsInRootFile(file1,hists1)) {
    std::cout<<"Problems getting histograms from file "<<file1<<std::endl;
    return 1;
  }
  if (!getAllHistsInRootFile(file2,hists2)) {
    std::cout<<"Problems getting histograms from file "<<file2<<std::endl;
    return 1;
  }
  std::vector<std::pair<std::string,TH1*> > histsOnlyIn1;
  std::vector<std::pair<std::string,TH1*> > histsOnlyIn2;
  std::vector<std::pair<std::string,std::pair<TH1*,TH1*> > > histsInBoth;

  classifyByKey( hists1,hists2,
		 histsOnlyIn1, histsOnlyIn2, histsInBoth );
  bool allok(true);
  if (!histsOnlyIn1.empty()) {
    if (verbose)
      for (unsigned i=0;i<histsOnlyIn1.size();++i)
	std::cout<<"Only found in "<<file1<<": "<<histsOnlyIn1.at(i).first<<std::endl;
    allok=false;
  }
  if (!histsOnlyIn2.empty()) {
    if (verbose)
      for (unsigned i=0;i<histsOnlyIn2.size();++i)
	std::cout<<"Only found in "<<file2<<": "<<histsOnlyIn2.at(i).first<<std::endl;
    allok=false;
  }

  unsigned nincompat(0);
  std::vector<TH1*> incompathists;
  if (!histsInBoth.empty()) {
    for (unsigned i=0;i<histsInBoth.size();++i) {
      if (verbose)
	std::cout<<"Found in both files: "<<histsInBoth.at(i).first<<std::endl;
      if (!histsCompatible(histsInBoth.at(i).second.first,histsInBoth.at(i).second.second)) {
	std::cout<<"  => Incompatibilities detected in "<<histsInBoth.at(i).first<<std::endl;
	incompathists.push_back(histsInBoth.at(i).second.first);
	incompathists.push_back(histsInBoth.at(i).second.second);
	allok=false;
	++nincompat;
      }
    }
  }
  //Summary:
  std::cout<<"============== Summary =============="<<std::endl;
  if (!histsOnlyIn1.empty())
    std::cout<<histsOnlyIn1.size()<<" histograms only found in "<<file1<<std::endl;
  if (!histsOnlyIn2.empty())
    std::cout<<histsOnlyIn2.size()<<" histograms only found in "<<file2<<std::endl;
  if (histsInBoth.size()-nincompat>0)
    std::cout<<histsInBoth.size()-nincompat<<" common and compatible histograms found in both files"<<std::endl;
  if (nincompat>0)
    std::cout<<nincompat<<" common but incompatible histograms found in both files"<<std::endl;
  //nincompat
  std::cout<<"====================================="<<std::endl;

  if (nincompat>0&&!outputfile.empty()) {
    std::cout<<"Writing copies of the "<<nincompat<<" (both versions) incompatible histograms to file "<<outputfile<<std::endl;
    TFile * file = new TFile(outputfile.c_str(),"NEW");
    if (!file->IsOpen()) {
      std::cout<<"ERROR: Problems opening file"<<std::endl;
      return 1;
    } else {
      for (unsigned i=0;i<incompathists.size();++i)
	incompathists.at(i)->Write();
    }
    file->Close();
    std::cout<<"Done writing file "<<outputfile<<std::endl;
  }
  exit(allok ? 0 : 1);
  return allok ? 0 : 1;
}
