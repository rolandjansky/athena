/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//File Wrapper class to keep track of histograms and manage memory
//Required due to problems with root's intrinsic memory management
//Author: Matt King (matthew.king@cern.ch)
//        Copied from code by Justin Griffiths
//Date:   May 2010
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <string>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TTree.h"
#include "TBranch.h"
#include "TGraph.h"

namespace dqutils {

  MonitoringFile::PostProcessorFileWrapper::PostProcessorFileWrapper(const std::string & inFilename, const std::string & name) :
    TFile( inFilename.c_str(), "UPDATE" ),
    m_name(name),
    m_error(0)
  {    
    if(this->IsOpen()){
      //std::cout << "Begin " << m_name << " MonitoringFile: " << this->GetName() << " Size: " << this->GetSize() << std::endl;
    }else{
      std::cerr << "ERROR " << m_name << " Could not open MonitoringFile: " << inFilename << std::endl;
    }
  }

  MonitoringFile::PostProcessorFileWrapper::~PostProcessorFileWrapper(){
    //std::cout << "Destructing " << m_name << " Collected " << m_garbage.size() << " Objects to be deleted" << std::endl;
    if(!m_error){
      this->Write(); 
    }else{
      std::cerr << "ERROR " << m_name << " Chose Not To Write " << this->GetName() << std::endl;
    }
    this->Close();
    for(std::set<TObject*>::const_iterator git = m_garbage.begin(); git != m_garbage.end(); ++git){
      delete *git;
    }
    //std::cout << "\n";
  }

  /* 
  // get() Defined in header file, otherwise it can't use a template.
  // If it can't use a template, then it can't pass a reference to an arbitrary object pointer, 
  // this solution isn't perfect, but it seems to be the best available.
  template <class T>
  void MonitoringFile::PostProcessorFileWrapper::getOriginal(const char* namecycle, T* &ptr, TDirectory* baseDir, TDirectory* target){
    
    if (baseDir) baseDir->GetObject( namecycle, ptr ); // suggested root function
    else this->GetObject( namecycle, ptr ); //suggested root function
    if(!ptr) return;

    if((T::Class())->InheritsFrom("TH1")){
      TH1 *obj = reinterpret_cast<TH1*> (ptr);
      
      obj->SetDirectory(target);
      if(target == 0) m_garbage.insert(obj);
    }
    else{
      TObject *obj = reinterpret_cast<TObject*>(ptr);
      
      if(target == 0) m_garbage.insert(obj);
    }
  }
  */  
  
  void MonitoringFile::PostProcessorFileWrapper::error(){ m_error = true; }

  void MonitoringFile::PostProcessorFileWrapper::setDirectory( TH1* h, TDirectory* dir ){
    h->SetDirectory(dir);
    if ( dir == 0 ){ m_garbage.insert(h);}
  }

  void MonitoringFile::PostProcessorFileWrapper::setDirectory( TH2* h, TDirectory* dir ){
    h->SetDirectory(dir);
    if ( dir == 0 ){ m_garbage.insert(h);}
  }
  
  void MonitoringFile::PostProcessorFileWrapper::setDirectory( TGraph* g, TDirectory* dir){
    // Cannot set directory for TGraph
    if ( dir == 0 ){ m_garbage.insert(g);}
  }

  void MonitoringFile::PostProcessorFileWrapper::setMetaData(TDirectory* targetDir, TH1* h1, TH1* h2, TH1* h3) { 
    //we will collect the garbage and set the directory to zero    
    h1->SetDirectory(0);
    m_garbage.insert(h1);
    if(h2) { h2->SetDirectory(0); m_garbage.insert(h2); }
    if(h3) { h3->SetDirectory(0); m_garbage.insert(h3); }
    //now call old function being sure not to fill a garbage set
    return MonitoringFile::TGCSetMetaData( targetDir, h1, h2, h3);
  }
  
  void MonitoringFile::TGCSetMetaData(TDirectory* targetDir, TH1* h1, TH1* h2, TH1* h3){
    TTree* metadata_overview_tr = 0;
    if(!targetDir || !h1) return;
    targetDir->GetObject("metadata",metadata_overview_tr);
    //Fill Metadata for new objs.
    std::string name_;
    std::string intervalData_="run";
    std::string chainData_="<none>";
    std::string mergeData_="<default>";
    if(metadata_overview_tr){
      OutputMetadata metadata_overview(metadata_overview_tr);
      char name[99];
      char intervalData[99];
      char chainData[99];
      char mergeData[99];
      TBranch* b_name;
      TBranch* b_intervalData;
      TBranch* b_chainData;
      TBranch* b_mergeData;
      TTree* tClone = (TTree*) metadata_overview_tr->Clone();
      metadata_overview_tr->SetDirectory(0);
      tClone->SetBranchAddress("Name",name,&b_name);
      tClone->SetBranchAddress("Interval",intervalData,&b_intervalData);
      tClone->SetBranchAddress("TriggerChain",chainData,&b_chainData);
      tClone->SetBranchAddress("MergeMethod",mergeData,&b_mergeData);
      //Check if Metadata Already Exists
      bool isMeta = false;
      for(int i = 0; i != tClone->GetEntries(); ++i){
	tClone->GetEntry(i);
	intervalData_ = intervalData;
	chainData_ = chainData;
	mergeData_ = mergeData;
	name_ = name;
	if(name_ == (std::string) h1->GetName()) isMeta = true;
	
      }
      
      delete tClone;
      if(isMeta) {
 	metadata_overview_tr->SetDirectory(0);
	delete metadata_overview_tr;
	return;
      }
      metadata_overview.fill(h1->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
      if(h2) metadata_overview.fill(h2->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
      if(h3) metadata_overview.fill(h3->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
      //set new hists to 0 dir
      TDirectory* tempDir = gDirectory;
      targetDir->cd();
      metadata_overview_tr->Write("",TObject::kOverwrite);
      tempDir->cd();
    }
    else {
      TTree*  metadata_overview_tr = new TTree( "metadata", "Monitoring Metadata" );
      OutputMetadata metadata_overview(metadata_overview_tr);      
      if(h1) metadata_overview.fill( h1->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
      TDirectory* tempDir = gDirectory;
      targetDir->cd();
      h1->SetDirectory(0);
      metadata_overview_tr->Write("",TObject::kOverwrite);
      metadata_overview_tr->SetDirectory(0);//make this safe to delete
      delete metadata_overview_tr;
      tempDir->cd();      
    }
  }
}//namespace
