/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonDQAUtils/MuonDQAHistTool.h"
 
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"

namespace Muon {
  /***********************************************************/	
  MuonDQAHistTool::MuonDQAHistTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_log(0)
  {
    declareInterface<MuonDQAHistTool>(this);
  }
  MuonDQAHistTool::~MuonDQAHistTool(){}
  /***********************************************************/	
  StatusCode MuonDQAHistTool::initialize()
  {    
    m_log = new MsgStream(msgSvc(),name());

    StatusCode sc = AlgTool::initialize(); 
    if ( sc.isFailure() ) {
      *m_log<<MSG::FATAL<<"Could not initialize AlgTool " <<endreq; 
      return sc;
    }
    return sc;
  } 
  /***********************************************************/	  
  StatusCode MuonDQAHistTool::finalize()
  {
    delete m_log;
    StatusCode sc = AlgTool::finalize(); 
    if( sc.isFailure() ) return sc;
    return sc;
  }

  /***********************************************************/
  TH1F* MuonDQAHistTool::makeAndRegisterTH1F(ManagedMonitorToolBase::MonGroup& mon, const char* hName, std::string hTitle, int nBins, float minX, float maxX) {
    TH1F* h = new TH1F(hName,hTitle.c_str(),nBins,minX,maxX);
    h->Sumw2();
    if (mon.regHist(h).isFailure()) {
      *m_log<<MSG::WARNING << "Unable to book histogram with name = "+std::string(hName) << endreq;
    }
    return h;
  }
	
  /***********************************************************/	
  TH2F* MuonDQAHistTool::makeAndRegisterTH2F(ManagedMonitorToolBase::MonGroup& mon,
					     const char* hName, std::string hTitle,
					     int nBinsX, float minX, float maxX,
					     int nBinsY, float minY, float maxY) {
    TH2F* h = new TH2F(hName,hTitle.c_str(),nBinsX,minX,maxX,nBinsY,minY,maxY);
    h->Sumw2();
    h->SetOption("colz");
    if (mon.regHist(h).isFailure()) {
      *m_log<<MSG::WARNING << "Unable to book histogram with name = "+std::string(hName) << endreq;
    }
    return h;
  }

} //namespace
