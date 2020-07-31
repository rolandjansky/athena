/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./AthenaL1TopoHistSvc.h"
#include "TH1.h"
#include "TH2.h"

#include "TrigConfBase/TrigConfMessaging.h"


#include "AthenaMonitoring/IMonitorToolBase.h"

#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/ReadHandleKey.h"


#include <iostream>
using namespace std;


class AthenaL1TopoHistSvc::AthenaL1TopoHistSvcImpl : public TrigConf::TrigConfMessaging
{
public:
   
   AthenaL1TopoHistSvcImpl(ServiceHandle<ITHistSvc> histSvc) :
      TrigConfMessaging("AthenaL1TopoHistSvc"),
      m_histSvc(histSvc)
   {
      TRG_MSG_INFO("Activating");
   }

   ~AthenaL1TopoHistSvcImpl()
   {}


  void registerHist(TH1 * h) {
     TRG_MSG_DEBUG("Registration of " << h->GetName() );
     if(m_histSvc) {
       string histName = h->GetName();
       auto colPos = histName.find_first_of('/');
       //Key will use original name when all algorithms changed with bookHist()
       const string key = histName.substr(colPos+1);
       const string fullName( m_baseDir + histName );
       h->SetName(key.c_str());
       std::unique_ptr<TH1> uhist(h);
       LockedHandle<TH1> lh;
       if( ! m_histSvc->regShared(fullName, std::move(uhist), lh).isSuccess() ) {
     	TRG_MSG_WARNING("Could not register histogram " << fullName << " with " << m_histSvc->name() );
       }
       else
     	{ m_hist1D[key] = lh; }
     } else {
       TRG_MSG_WARNING("No THistSvc available, can't register histogram");
     }
  }

   void registerHist(TH2 * h) {
      TRG_MSG_DEBUG("Registration of " << h->GetName() );
      if(m_histSvc) {
        string histName = h->GetName();
        auto colPos = histName.find_first_of('/');
	//Key will use original name when all algorithms changed with bookHist()
        const string key = histName.substr(colPos+1);
        const string fullName( m_baseDir + histName );
        h->SetName(key.c_str());
        std::unique_ptr<TH2> uhist(h);
        LockedHandle<TH2> lh;
        if( ! m_histSvc->regShared(fullName, std::move(uhist), lh).isSuccess() ) {
      	 TRG_MSG_WARNING("Could not register histogram " << fullName << " with " << m_histSvc->name() );
        }
        else
      	 { m_hist2D[key] = lh; }
      } else {
        TRG_MSG_WARNING("No THistSvc available, can't register histogram");
      }
   }

   TH1 * findHist(const std::string & histName) {
      TH1 * h;
      string fullName( m_baseDir + histName );
      if ( ! m_histSvc->getHist(fullName, h).isSuccess() )
	{ TRG_MSG_WARNING("Could not find histogram with name : " << fullName  ); }
      else
	{ TRG_MSG_DEBUG("findHist(" << histName << ") found: " << (void*)h); }
      return h;
   }

   void fillHist1D(const std::string & histName,double x) {
     //This will be removed when all algorithms changed with bookHist()
      auto colPos = histName.find_first_of('/');
      const string key = histName.substr(colPos+1);
      if(m_hist1D.find(key) == m_hist1D.end()) {
	TRG_MSG_ERROR("1D-hist with registration key " << key << " does not exist");
      }
      else { m_hist1D[key]->Fill(x); }
   }

   void fillHist2D(const std::string & histName,double x,double y) {
     //This will be removed when all algorithms changed with bookHist()
      auto colPos = histName.find_first_of('/');
      const string key = histName.substr(colPos+1);
      if(m_hist2D.find(key) == m_hist2D.end()) {
	TRG_MSG_ERROR("2D-hist with registration key " << key << " does not exist");
      }
      else
	{ m_hist2D[key]->Fill(x,y); }
   }

   void setBaseDir(const std::string & baseDir) {
      auto colPos = baseDir.find_last_of(':');
      if( colPos != string::npos ) {
         m_baseDir = baseDir.substr(colPos+1);
      } else {
         m_baseDir = baseDir;
      }
      if( '/' != m_baseDir[m_baseDir.size()-1] ) {
         // add a '/' at the end
         m_baseDir += "/";
      }
   }

private:
   ServiceHandle<ITHistSvc>   m_histSvc;
   string                     m_baseDir {""};

   std::unordered_map<std::string, LockedHandle<TH1>> m_hist1D  ATLAS_THREAD_SAFE;
   std::unordered_map<std::string, LockedHandle<TH2>> m_hist2D  ATLAS_THREAD_SAFE;


};


AthenaL1TopoHistSvc::AthenaL1TopoHistSvc(ServiceHandle<ITHistSvc> histSvc) :
   m_impl(new AthenaL1TopoHistSvc::AthenaL1TopoHistSvcImpl(histSvc))
{}

AthenaL1TopoHistSvc::~AthenaL1TopoHistSvc()
{}

void
AthenaL1TopoHistSvc::registerHist(TH1 * h) {
   m_impl->registerHist(h);
}

void
AthenaL1TopoHistSvc::registerHist(TH2 * h) {
   m_impl->registerHist(h);
}

TH1 *
AthenaL1TopoHistSvc::findHist(const std::string & histName) {
   return m_impl->findHist( histName );
}

void
AthenaL1TopoHistSvc::fillHist1D(const std::string & histName, double x) {
   m_impl->fillHist1D(histName,x);
}

void
AthenaL1TopoHistSvc::fillHist2D(const std::string & histName, double x, double y) {
   m_impl->fillHist2D(histName,x,y);
}

void
AthenaL1TopoHistSvc::setBaseDir(const std::string & baseDir) {
   m_impl->setBaseDir( baseDir );
}

void
AthenaL1TopoHistSvc::save() {
   // not implemented
}
