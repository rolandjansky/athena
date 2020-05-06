/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/RootFileManager.h"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include <iostream>

namespace MuonCalib {

  RootFileManager* MuonCalib::RootFileManager::m_instance = 0;

  RootFileManager::RootFileManager() : m_fileOption("RECREATE"), m_outFile(0),  m_key(NULL), m_value(NULL) {
    m_printLevel = 0;
  }

  RootFileManager::RootFileManager(const RootFileManager& /*mgr*/): m_fileOption("RECREATE"), m_outFile(0),  m_key(NULL), m_value(NULL) {
    // empty method, no copying
    m_printLevel = 0;
  }

  TDirectory* RootFileManager::getDirectory( std::string dirname ) {
    if( !m_outFile ){
      return 0;
    }

    // pointer to directory associated with the dirnam
    TDirectory* dir = 0;

    // lookup name in map
    DirectoryIt it = m_dirMap.find( dirname );
    if( it != m_dirMap.end() ){
      // if found return pointer to existing TDirectory
      dir = it->second;
    }else{
      // no found create new directory
      dir = m_outFile->mkdir( dirname.c_str() );
      m_dirMap.insert( std::make_pair( dirname, dir ) );
    }

    // return pointer
    return dir;
  }  //end RootFileManager::getDirectory

  bool RootFileManager::openFile( std::string filename) {

    if (m_outfiles[filename]==NULL) {
      m_outfiles[filename] = new TFile( filename.c_str(), m_fileOption.c_str() );
    }
    // create new root file
    m_outFile = m_outfiles[filename];

    return true;
  }  //end RootFileManager::openFile

  bool RootFileManager::closeFile() {
    // write output and close file
    for(std::map<std::string, TFile *> :: iterator it=m_outfiles.begin(); it!=m_outfiles.end(); it++) {
      it->second->Write();
      it->second->Close();
    }
    // set pointer to zero
    delete m_key; m_key=NULL;
    delete m_value; m_value=NULL;
    m_outfiles.clear();
    m_dirMap.clear();
    return true;
  }  //end RootFileManager::closeFile

  void RootFileManager::WriteMetaData(const std::map<std::string, std::string> &metadata, const std::string &filename) {
    if(m_outfiles.find(filename)==m_outfiles.end()) {
      return;
    }
    TDirectory *current=gDirectory;
    m_outfiles[filename]->cd();
    if(!m_key) m_key = new std::string();
    if(!m_value) m_value = new std::string();
    TTree *meta_data_tree=new TTree("meta_data", "Meta Data");
    m_meta_data_tree[filename] = meta_data_tree;
    meta_data_tree->Branch("key", "std::string", &m_key);
    meta_data_tree->Branch("value", "std::string", &m_value);
    for(std::map<std::string, std::string>::const_iterator it=metadata.begin(); it!=metadata.end(); it++) {
      m_key->assign(it->first);
      m_value->assign(it->second);
      meta_data_tree->Fill();
    }
    meta_data_tree->Write();
    current->cd();
  }  //end RootFileManager::WriteMetaData

}  //end namespace MuonCalib
