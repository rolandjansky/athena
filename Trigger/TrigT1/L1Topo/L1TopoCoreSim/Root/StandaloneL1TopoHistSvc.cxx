/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "L1TopoCoreSim/StandaloneL1TopoHistSvc.h"
#include "TFile.h"
#include "TH1.h"

#include <iostream>
#include <map>

#include "TrigConfBase/TrigConfMessaging.h"

using namespace std;

class StandaloneL1TopoHistSvc::StandaloneL1TopoHistSvcImpl : public TrigConf::TrigConfMessaging
{
public:

   StandaloneL1TopoHistSvcImpl() :
      TrigConfMessaging("StandaloneL1TopoHistSvc")
   {}

   ~StandaloneL1TopoHistSvcImpl() {
      for( auto h : m_hists ) {
         delete h.second;
      }
   }

   void registerHist(TH1 * h) {
      if(h != nullptr) {
         //cout << "JOERG registering histogram " << h->GetName() << " at " << h << endl;
         h->SetDirectory(0);
         TRG_MSG_DEBUG("Registering histogram " << h->GetName());
         const string key = h->GetName();
         if( m_hists.find(key)  == end(m_hists) ) {
            m_hists[key] = h;
         } else {
            TRG_MSG_WARNING("StandaloneL1TopoHistSvc::registerHist: a histogram " << key << " exists already. Will keep the first one and delete the newly requested.");
            delete h;
         }
      }
   }

   TH1 * findHist(const string & histName) {
      auto h = m_hists.find(histName);
      if( h == end(m_hists) ) {
         return nullptr;
      } else {
         return h->second;
      }
   }

   void setBaseDir(const string & baseDir) {
      m_baseDir = baseDir;
   }

   void save() {

      string filename = "L1Topo.root";
      string basepath = "";

      auto colPos = m_baseDir.find_last_of(':');
      if( colPos != string::npos ) {
         filename = m_baseDir.substr(0, colPos);
         basepath = m_baseDir.substr(colPos+1);
      } else {
         basepath = m_baseDir;
      }


      TFile * f = TFile::Open(filename.c_str(),"RECREATE");
      for( auto h : m_hists ) {

         string fullName(h.second->GetName());
         string path(basepath);

         auto slashPos = fullName.find_last_of('/');
         if(slashPos != string::npos) {
            if(path!="")
               path += "/";
            path += fullName.substr(0,slashPos);
            // set the name
            h.second->SetName( fullName.substr(slashPos+1).c_str() );
         }

         const char* dir = path.c_str();
         if( ! f->GetDirectory(dir)) {
            f->mkdir(dir);
         }
         f->cd(dir);
         h.second->Write();
      }
      f->Write();
      f->Close();
      TRG_MSG_INFO("Wrote " << m_hists.size() << " l1topo algorithm histograms to file " << filename);
   }

private:

   map<string,TH1*>   m_hists;
   string             m_baseDir {""};

};




// the interface class

StandaloneL1TopoHistSvc::StandaloneL1TopoHistSvc() : 
   m_impl(new StandaloneL1TopoHistSvc::StandaloneL1TopoHistSvcImpl())
{}

StandaloneL1TopoHistSvc::~StandaloneL1TopoHistSvc()
{}

// forwarding the calls from the interface class

void
StandaloneL1TopoHistSvc::registerHist(TH1 * h) {
   //cout << "Called StandaloneL1TopoHistSvc::registerHist(" << h->GetName() << ")" << endl;
   m_impl->registerHist(h);
}

TH1 * 
StandaloneL1TopoHistSvc::findHist(const std::string & histName) {
   //cout << "Called StandaloneL1TopoHistSvc::findHist(" << histName << ")" << endl;
   return m_impl->findHist(histName);
}

void
StandaloneL1TopoHistSvc::setBaseDir(const std::string & baseDir) {
   m_impl->setBaseDir(baseDir);
}


void
StandaloneL1TopoHistSvc::save() {
   m_impl->save();
}



