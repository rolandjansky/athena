/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoSimulation/AthenaL1TopoHistSvc.h"
#include "TH1.h"
#include "TH2.h"

#include "TrigConfBase/TrigConfMessaging.h"

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
         string fullName( m_baseDir + h->GetName() );
         auto colPos = fullName.find_last_of('/');
         string histName = fullName.substr(colPos+1);
         h->SetName(histName.c_str());
	 std::unique_ptr<TH1> uhist(h);
	 LockedHandle<TH1> lh;
         if( ! m_histSvc->regShared(fullName, std::move(uhist), lh).isSuccess() ) {
	   TRG_MSG_WARNING("Could not register histogram " << fullName << " with " << m_histSvc->name() );
         }  
      } else {
         TRG_MSG_WARNING("No THistSvc available, can't register histogram");
      }
   }

   void registerHist(TH2 * h) {
      TRG_MSG_DEBUG("Registration of " << h->GetName() );
      if(m_histSvc) {
         string fullName( m_baseDir + h->GetName() );
         auto colPos = fullName.find_last_of('/');
         string histName = fullName.substr(colPos+1);
         h->SetName(histName.c_str());
	 std::unique_ptr<TH2> uhist(h);
	 LockedHandle<TH2> lh;
         if( ! m_histSvc->regShared(fullName, std::move(uhist), lh).isSuccess() ) {
	   TRG_MSG_WARNING("Could not register histogram " << fullName << " with " << m_histSvc->name() );
         }  
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
      LockedHandle<TH1> lh;
      string fullName( m_baseDir + histName );
      if( ! m_histSvc->getShared(fullName, lh).isSuccess())
	{ TRG_MSG_WARNING("No histogram found for " << fullName); }
      else
	{ lh->Fill(x); }
   }

   void fillHist2D(const std::string & histName,double x,double y) {
      LockedHandle<TH2> lh;
      string fullName( m_baseDir + histName );
      if( ! m_histSvc->getShared(fullName, lh).isSuccess())
	{ TRG_MSG_WARNING("No histogram found for " << fullName); }
      else
	{ lh->Fill(x,y); }
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

void AthenaL1TopoHistSvc::fillHist1D(const std::string & histName, double x) {
  m_impl->fillHist1D(histName,x);
}

void AthenaL1TopoHistSvc::fillHist2D(const std::string & histName, double x, double y) {
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
