/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <TFile.h>
#include "TrigFTKSim/FTKRootFile.h"

/* package FTKRootFile
 *
 * Common interface to open TFile objects by name
 * This offer the possibility to implement different protocols
 * At present, only access to the local file system is implemented
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

using namespace std;

//================== class FTKRootFile =================

FTKRootFile *FTKRootFile::Instance(void) {
   if(!s_theFileAccess) {
      s_theFileAccess=new FTKRootFile();
   }
   return s_theFileAccess;
}

FTKRootFile *FTKRootFile::s_theFileAccess=0;

FTKRootFile::FTKRootFile(void) {
   // do nothing
}

TDirectory *FTKRootFile::OpenRootFileReadonly(char const *path) {
   TFile *r=new TFile(path,"READ");
   if(r && r->IsZombie()) {
      // if TFile can not be opened, return 0
      delete r;
      r=0;
   }
   return r;
}

TDirectory *FTKRootFile::CreateRootFile(char const *path,int compression) {
   TFile *r=new TFile(path,"RECREATE","",compression);
   if(r && r->IsZombie()) {
      // if TFile can not be opened, return 0
      delete r;
      r=0;
   }
   return r;
}

bool FTKRootFile::IsRootFile(char const *path) {
   TFile *r=new TFile(path,"READ");
   bool test= r && !r->IsZombie();
   delete r;
   return test;
}

//================== class FTKRootFileChain =================

 void FTKRootFileChain::AddFile(char const *fileName) {
   m_fileNames.push_back(fileName); 
   if ( (int)m_fileNames.size() >= m_maxOpenFiles ) {
      Warning("AddFile") << "Number of files in chain exceeds maximum number of open files (maxOpen="<<m_maxOpenFiles<<")."<<endl;
      Warning("AddFile") << "This will cause large time consumption by frequently re-opening and closing files."<<endl;
      }
}


bool FTKRootFileChain::Open(std::string const &fileName) {
   bool r=false;
   if(fileName != m_lastFileName) {
      // directory has changed
      // check whether there is an open file with that name
      if(m_openDirectories.find(fileName)==m_openDirectories.end()) {
         // file not opened
         if((int)m_openDirectories.size()>=m_maxOpenFiles) {
            // no space left in table, have to close another file
            // here: use the "first" slot
            // could be changed to use the recently used slot
            // or the least used slot
            // or the smallest file or ...
            map<std::string,TDirectory *>::iterator j=m_openDirectories.begin();
            delete (*j).second;
            m_openDirectories.erase(j);
         }
         // open a new file
	 m_openDirectories[fileName] = OpenRootFileReadonly(fileName.c_str());
      }
      m_lastFileName=fileName;
      r=true;
   }
   return r;
}

TDirectory *FTKRootFileChain::OpenRootFileReadonly
   (char const *fileName) {
   TDirectory *r=FTKRootFile::Instance()->OpenRootFileReadonly(fileName);
   return r;
}

TDirectory *FTKRootFileChain::GetDirectory(void) const {
   std::map<std::string,TDirectory *>::const_iterator
      i=m_openDirectories.find(m_lastFileName);
   if(i==m_openDirectories.end()) return 0;
   return (*i).second;
}

FTKRootFileChain::~FTKRootFileChain() {
   while(m_openDirectories.begin()!=m_openDirectories.end()) {
      delete (*m_openDirectories.begin()).second;
      m_openDirectories.erase(m_openDirectories.begin());
   }
}
