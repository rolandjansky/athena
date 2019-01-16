/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PathFinder.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TTree.h"

#include "Regexer.h"

using std::vector;

PathFinder::PathFinder(string inputFilePath, string runNumber, string inclExpr, string exclExpr) : 
  expr_incl(inclExpr), expr_excl(exclExpr), runNo(runNumber)
{
  in_file = new TFile(inputFilePath.c_str(), "READ");
  if(!in_file){
    std::cout<<"ERROR: Cannot open input file. exiting. "<<std::endl;
    exit(-1);
  }
}

PathFinder::~PathFinder(void){
  in_file->Close();
}

void PathFinder::CopyHistograms(string outputfile){

  //set up regexer to split path into folders
  Regexer* exer = new Regexer();

  //attempt to open output file
  TFile* fout = new TFile(outputfile.c_str(),"RECREATE");
 
  //check that input path begins with "run_". this is prepended by the T0 merging which places the HLT/ folder
  //underneath. otherwise we are probably dealing with the output of a standalone test and should prepend it in
  //the ouput manually, soas to allow standard han-configs to work with this file.
  // - note: we assume here that there was at least one 1d histogram registered.
  bool prependRun = false;
  string runFolder = "run_"+runNo;
  if(histoPaths1D.at(0).find("run_")==std::string::npos){
    prependRun = true;
  }

  //loop through lists
  for(unsigned int i=0; i<histoPaths1D.size(); i++){
    //go to root directory of file
    TDirectory* wd = fout->GetDirectory(NULL);
    wd->cd();
    //split path into folders
    vector<string> dirs = exer->SplitPathIntoDirs(histoPaths1D.at(i));
    //reassemble fullpath
    string fullpath = "";
    if(prependRun){
      TDirectory* td;
      if(!(td = wd->GetDirectory(runFolder.c_str()))){
        td = wd->mkdir(runFolder.c_str());
      }
      wd = td;
    }
    //loop down directories and make sure they all exist
    unsigned int n_dirs = dirs.size();
    for(unsigned int j=1; j<n_dirs-1; j++){
      TDirectory* td;
      std::string dir = dirs.at(j);
      if(td = wd->GetDirectory(dir.c_str())){
        //std::cout<<"directory "<<dir<<" exists"<<std::endl;
      }
      else{
        //std::cout<<"creating directory "<<dir<<std::endl;
        td = wd->mkdir(dir.c_str());
      }
      wd = td;
      //append current directory to fullpath
      fullpath+="/"+dir;
    }
    string name = dirs.at(n_dirs-1);
    //now copy histogram
    //std::cout<<"copying histogram "<<dirs.at(n_dirs-1)<<std::endl;
    TDirectory* sd = in_file->GetDirectory(fullpath.c_str());
    TObject* obj = NULL;
    sd->GetObject(name.c_str(),obj);
    //std::cout<<sd<<"\t"<<obj<<std::endl;
    wd->WriteTObject(obj);
  }
  //loop through lists
  for(unsigned int i=0; i<histoPaths2D.size(); i++){
    //go to root directory of file
    TDirectory* wd = fout->GetDirectory(NULL);
    wd->cd();
    //split path into folders
    vector<string> dirs = exer->SplitPathIntoDirs(histoPaths2D.at(i));
    //reassemble fullpath
    string fullpath = "";
    if(prependRun){
      TDirectory* td;
      if(!(td = wd->GetDirectory(runFolder.c_str()))){
        td = wd->mkdir(runFolder.c_str());
      }
      wd = td;
    }
    //loop down directories and make sure they all exist
    unsigned int n_dirs = dirs.size();
    for(unsigned int j=1; j<n_dirs-1; j++){
      TDirectory* td;
      std::string dir = dirs.at(j);
      if(td = wd->GetDirectory(dir.c_str())){
        //std::cout<<"directory "<<dir<<" exists"<<std::endl;
      }
      else{
        //std::cout<<"creating directory "<<dir<<std::endl;
        td = wd->mkdir(dir.c_str());
      }
      wd = td;
      fullpath+="/"+dir;
    }
    string name = dirs.at(n_dirs-1);
    //now copy histogram
    //std::cout<<"copying histogram "<<dirs.at(n_dirs-1)<<std::endl;
    TDirectory* sd = in_file->GetDirectory(fullpath.c_str());
    TObject* obj = NULL;
    sd->GetObject(name.c_str(),obj);
    //std::cout<<sd<<"\t"<<obj<<std::endl;
    wd->WriteTObject(obj);
  }

  //clean up
  fout->Write();
  fout->Close();
  delete exer;


}

void PathFinder::FilterList(void){

  //set up regular expression matcher
  Regexer* exer = new Regexer(expr_incl,expr_excl);

  //create temporary result holders
  vector<string> temp1d;
  vector<string> temp2d;

  for(unsigned int i=0; i<histoPaths1D.size(); i++){
    if(exer->Match(histoPaths1D.at(i))){
      temp1d.push_back(histoPaths1D.at(i));
    }
  }
  for(unsigned int i=0; i<histoPaths2D.size(); i++){
    if(exer->Match(histoPaths2D.at(i))){
      temp2d.push_back(histoPaths2D.at(i));
    }
  }

  //switch to filtered lists
  histoPaths1D.clear();
  histoPaths1D = temp1d;
  histoPaths2D.clear();
  histoPaths2D = temp2d;

  //clean up
  delete exer;
}

/*
void PathFinder::DumpHistoList(void){

  if(out_list==""){
    std::cerr<<"ERROR: dumping of histogram list requested but no outputpath specified. exiting."<<std::endl;
    exit(-1);
  }

  std::ofstream fout(out_list.c_str());
  for(unsigned int i=0; i<histoPaths1D.size(); i++){
    fout<<histoPaths1D.at(i)<<std::endl;
  }
  for(unsigned int i=0; i<histoPaths2D.size(); i++){
    fout<<histoPaths2D.at(i)<<std::endl;
  }
  fout.close();

}
*/

void PathFinder::WalkDirectory(TDirectory* dir){

  //fetch directory if not already given
  if(dir==NULL){ dir = in_file->GetDirectory(NULL);}

  //issue progress update
  //std::cout<<"Walking directory "<<dir->GetTitle()<<std::endl; 

  //find path and strip filename
  std::string path_full = dir->GetPath();
  std::string path = path_full.substr(path_full.find(":")+1);

  //loop over keys
  TIter nextkey(dir->GetListOfKeys());
  TKey* key;
  while (key = (TKey*)nextkey()) {
    //find classname (doing checks on this TODO change over to key->Class() comparisons once you figured them out)
    std::string classname = key->GetClassName();
    //check if this is a folder and walk it (skip TTrees, because they're nasty)
    if(key->IsFolder() && classname!="TTree" ){
      WalkDirectory((TDirectory*)key->ReadObj());
    }
    //else add path to list
    else{
      //grab title
      std::string name = key->GetName();
      //decide on 1d/2d
      if(classname.find("TH1")==0){
        histoPaths1D.push_back(path+"/"+name);
      }
      if(classname.find("TH2")==0){
        histoPaths2D.push_back(path+"/"+name);
      }
    }

  }//done looping over keys
}
