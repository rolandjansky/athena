/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"

#include "Particle/TrackParticle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

/// analysis tools
#include "AnalysisUtils/AnalysisCombination.h"

/// the header file
#include "ZeeOnESD.h"

#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>

/// Declare a local helped function
bool selectEgamma(ZeeOnESD * self, const egammaPair &ll);

using namespace Analysis;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

ZeeOnESD::ZeeOnESD(const std::string& name,
  ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_thistSvc(nullptr),
    m_esd_egamma_pt(nullptr),
    m_esd_egamma_eta(nullptr),
    m_esd_egamma_overp(nullptr),
    m_esd_egamma_isEM(nullptr),
    m_esd_zee_mass_hist(nullptr),
    m_histEgammaTrackP(nullptr),
    m_histEgammaClusterE(nullptr)
{
  /// switches to control the analysis through job options :: these are the default
  /// to changed in the job options

  /// The egamma ESD container name & selection cuts
  declareProperty("egammaContainer", m_egammaContainerName = "egammaCollection");
  declareProperty("TrackMatchContainer", m_trkMatchContainerName="EMTrackMatchContainer");
  declareProperty("egammaEtCut", m_etEgammaCut = 20.0*CLHEP::GeV);
  declareProperty("egammaEtaCut", m_etaEgammaCut = 2.5);

  }


/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

ZeeOnESD::~ZeeOnESD() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate
/// get a handle on the analysis tools
/// book histograms

StatusCode ZeeOnESD::initialize() {

  ATH_MSG_INFO("Initializing ZeeOnESD");

  /*
  // don't need this
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
     return sc;
  }
  */
  StatusCode sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
     return sc;
  }
      
  /// the histograms

  /// Electron histogram booking
  m_esd_egamma_pt      = new TH1F("esd_egamma_pt","esd pt eg",50,0,250.*CLHEP::GeV);
  sc =  m_thistSvc->regHist("/AANT/egamma/esd_egamma_pt",m_esd_egamma_pt);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_pt registration failed");
     return sc;
  }

  m_esd_egamma_eta     = new TH1F("esd_egamma_eta","esd eta eg",70,-3.5,3.5);
  sc =  m_thistSvc->regHist("/AANT/egamma/esd_egamma_eta",m_esd_egamma_eta);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_eta registration failed");
     return sc;
  }

  m_esd_egamma_overp   = new TH1F("esd_egamma_eoverp","ead E/p eg",50,0,2.);
  sc =  m_thistSvc->regHist("/AANT/egamma/esd_egamma_eoverp",m_esd_egamma_overp);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_eoverp registration failed");
     return sc;
  }

  m_esd_egamma_isEM    = new TH1F("esd_egamma_isEM_bits","egamma isEM bit Pattern",20,-1.5,18.5);
  sc =  m_thistSvc->regHist("/AANT/egamma/esd_egamma_isEM_bits",m_esd_egamma_isEM);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_isEM registration failed");
     return sc;
  }

  m_esd_zee_mass_hist  = new TH1F("Mee_ESD","Mee_ESD",50,0,250.*CLHEP::GeV);
  sc =  m_thistSvc->regHist("/AANT/egamma/Mee_ESD",m_esd_zee_mass_hist);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_Mee registration failed");
     return sc;
  }

  m_histEgammaTrackP   = new TH1F("egammaTrackP","eg Track Momentum",100,0,500.*CLHEP::GeV);
  sc =  m_thistSvc->regHist("/AANT/egamma/esd_eta_eg",m_histEgammaTrackP);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_TrackP registration failed");
     return sc;
  }

  m_histEgammaClusterE = new TH1F("egammaClusterE","eg Cluster Energy",100,0,500.*CLHEP::GeV);
  sc =  m_thistSvc->regHist("/AANT/egamma/egammaCkusterE",m_histEgammaClusterE);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("ROOT Hist m_esd_egamma_ClusterE registration failed");
     return sc;
  }
  
  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode ZeeOnESD::finalize() {
  MsgStream mLog( messageService(), name() );
  
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - called by the event loop on event by event

StatusCode ZeeOnESD::execute() {

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "execute()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  /// do the Z->ee reconstruction on ESD
  sc = zee_on_esd();
  if ( sc.isFailure() ) {
    mLog << MSG::FATAL << "Z->ee reconstruction on ESD failed" << endreq;
    return StatusCode::FAILURE;
  }

  return sc;

}

//////////////////////////////////////////////////////////////////////////////////
/// zee on esd: called by execute()

StatusCode ZeeOnESD::zee_on_esd() {


  ATH_MSG_DEBUG("zee_on_esd");

  StatusCode sc = StatusCode::SUCCESS;

  /// read the ESD egamma container from Storegate
  const ElectronContainer* egammaTES = 0;
  sc=evtStore()->retrieve( egammaTES, m_egammaContainerName);
  if( sc.isFailure()  ||  !egammaTES ) {
     ATH_MSG_FATAL("No ESD egamma container found in StoreGate");
     return StatusCode::FAILURE;
  }  
  ATH_MSG_DEBUG("egammaContainer successfully retrieved. Size = " << egammaTES->size());
  
  /// iterators over the container 
  ElectronContainer::const_iterator egammaItr  = egammaTES->begin();
  ElectronContainer::const_iterator egammaItrE = egammaTES->end();

  /// loop over the ESD electron container
  /// and fill the egamma pt, eta and E/P histograms
  for (; egammaItr != egammaItrE; ++egammaItr) {
    int bitPosition = 1;
    
    int isEM = (*egammaItr)->isem();
    if ( isEM == 0 ) m_esd_egamma_isEM->Fill( (isEM-1.0), 1.0);
    if (isEM > 0) {
       for (int i=0; i<16; ++i) {
         if (isEM & bitPosition) m_esd_egamma_isEM->Fill(i, 1.0);
         bitPosition *= 2;
       }
    }
    const CaloCluster* cluster = (*egammaItr)->cluster();
    const EMTrackMatch* trkMatch = (*egammaItr)->detail<EMTrackMatch>(m_trkMatchContainerName);
    ATH_MSG_DEBUG("egamma isEM/pt/trkMatch " << (*egammaItr)->isem()<<","<<cluster->pt()<<","<<trkMatch);
    if((*egammaItr)->isem()%16 == 0) ATH_MSG_DEBUG("2: egamma isEM/pt/trkMatch " << (*egammaItr)->isem()<<","<<cluster->pt()<<","<<trkMatch);

    if( trkMatch && (*egammaItr)->isem()%16 == 0 && cluster){
      m_esd_egamma_pt->Fill( cluster->pt(), 1.);
      m_esd_egamma_eta->Fill( cluster->eta(), 1.);

      // we need to calculate E/P ourselves
      double eOp=0;
      if((*egammaItr)->trackParticle() ) {
        double p = ((*egammaItr)->trackParticle()->pt())*(cosh((*egammaItr)->trackParticle()->eta()));
        double e = (*egammaItr)->cluster()->et()*(cosh((*egammaItr)->cluster()->eta()));
        eOp = p>0. ? e/p:0.;
      }

      m_esd_egamma_overp->Fill( eOp, 1.);
    }
  }

  /// ee invariant mass reconstruction
  /// use the Analysis Utility to get combinations of 2 electrons from the AOD container
  /// and the selection
  /// retain the best combination
  AnalysisUtils::Combination<const ElectronContainer> comb(egammaTES,2);
  egammaPair egPair; 
  double mee = -1.0;
  while (comb.goodOnes(this, egPair, selectEgamma)) {
    mee = (egPair[0]->hlv()+egPair[1]->hlv()).m();
    m_esd_zee_mass_hist->Fill(mee);
  }
  		
  return StatusCode::SUCCESS;
}


/// this function is a friend of ZeeOnESD
/// electron selection - test for the charges - cut on pt - cut on eta
/// more sophisticated cuts such as shower shape cut should be considered
/// cut on the isEM 
bool selectEgamma(ZeeOnESD * self, const egammaPair &ll) {

  bool test1 = false;
  if (ll[0]->trackParticle() && ll[1]->trackParticle())
     test1 = ll[0]->trackParticle()->charge() == -(ll[1]->trackParticle()->charge());

  bool test2 = false;
  bool test3 = false;
  if (ll[0]->cluster() && ll[1]->cluster()) {
     test2 = (ll[0]->cluster()->et() > self->m_etEgammaCut) &&
                  (ll[1]->cluster()->et() > self->m_etEgammaCut);
     test3 = (fabs(ll[0]->cluster()->eta()) < self->m_etaEgammaCut ) &&
                  (fabs(ll[1]->cluster()->eta()) < self->m_etaEgammaCut );
  }
  bool test4 = ( (ll[0]->isem()%16)==0 && (ll[1]->isem()%16) == 0);
  return (test1 && test2 && test3 && test4);
}
