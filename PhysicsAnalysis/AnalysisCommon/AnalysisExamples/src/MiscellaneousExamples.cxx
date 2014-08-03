/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// head file
#include "MiscellaneousExamples.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "Navigation/NavigationToken.h"

/// the track object
#include "Particle/TrackParticleContainer.h"

/// ESD objects
#include "CaloEvent/CaloClusterContainer.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"

/// composite particles
#include "CompositeParticleEvent/CompositeParticle.h"

/// analysis tools
#include "AnalysisUtils/AnalysisCombination.h"

#include <algorithm>
#include <math.h>
#include <functional>

/// declare some helper functions
bool selectJet(MiscellaneousExamples *self, const JetVect &jj);
bool selectParticle(MiscellaneousExamples *self, const ParticleVect &jj);

/// some user defined constants
static const double mW         = 80.4*CLHEP::GeV;
static const double topMass    = 175.0*CLHEP::GeV;

using namespace Analysis;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

MiscellaneousExamples::MiscellaneousExamples(const std::string& name,
  ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator),
  m_analysisTools( "AnalysisTools", this ) {

  /// switches to control the analysis through job options
  declareProperty( "AnalysisTools", m_analysisTools );
  declareProperty("JetContainerName", m_jetContainerName = "JetCollection");
  declareProperty("ElectronContainerName", m_electronContainerName = "ElectronCollection");
  declareProperty("MuonContainerName", m_muonContainerName = "MuonCollection");
  declareProperty("EventWeight", m_eventWeight = 1.0);

  /// the cuts - default values - to be modified in job options
  declareProperty("JetPtCut", m_jetPtCut = 20.0*CLHEP::GeV);

  /// selection for mjj
  declareProperty("DeltaMjj", m_deltaMjj = 25.0*CLHEP::GeV);

}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

MiscellaneousExamples::~MiscellaneousExamples() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate
/// get a handle on the analysis tools
/// book histograms

StatusCode MiscellaneousExamples::initialize() {

  MsgStream mLog( messageService(), name() );

  mLog << MSG::INFO
       << "Initializing MiscellaneousExamples"
       << endreq;

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
     mLog << MSG::ERROR
          << "Unable to retrieve pointer to StoreGateSvc"
          << endreq;
     return sc;
  }
  
  /// get a handle on the analysis tools
  sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
      mLog << MSG::ERROR << "Can't get handle on analysis tools" << endreq;
      return sc;
  }

  /// Retrieve a pointer to THistSvc
  sc = service("THistSvc", m_thistSvc);
  if(sc.isFailure() ){
    mLog << MSG::ERROR
         << "Unable to retrieve pointer to THistSvc"
         << endreq;
    return sc;
  }

  /// reconstructed jet histograms
  m_histNumberOfJets   = new TH1F("NumberOfJets","N jets",20,0,20);
  sc = m_thistSvc->regHist("/AANT/jets/NumberOfJets", m_histNumberOfJets);
  m_histJetPt          = new TH1F("jet_pt","pt of jet",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/jets/jet_pt", m_histJetPt);
  m_histJetEta         = new TH1F("jet_eta","eta of jet",100,-5.0,5.0);
  sc = m_thistSvc->regHist("/AANT/jets", m_histJetEta);
  m_histMjj            = new TH1F("jj mass","mjj",80,0,160*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/jets/jj mass", m_histMjj);
  m_histSumEtCell      = new TH1F("jet_et","Sum Et Cells",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/jets/jet_et", m_histSumEtCell);
  m_histSymLinkPt      = new TH1F("jetSimLink_pt","pt of symLinkjet",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/jets/jetSimLink_pt", m_histSymLinkPt);
  m_histSymLinkMjj    =  new TH1F("SimLink_Mjj","Mjj of symLinkjets",80,0,160.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/jets/SimLink_Mjj", m_histSymLinkMjj);

  m_histDataType     = new TH1F("data type","dataType",5,0,5);
  sc = m_thistSvc->regHist("/AANT/general/data type", m_histDataType);
  m_histElecTrackP   = new TH1F("elecTrackP","e Track Momentum",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/ectrons/elecTrackP", m_histElecTrackP);
  m_histElecClusterE = new TH1F("elecClusterE","e Cluster Energy",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/electrons/elecClusterE", m_histElecClusterE);

  m_histCombinedMuonTrackP     = new TH1F("CombinedTrackP","CombinedTrackP",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/muons/CombinedTrackP", m_histCombinedMuonTrackP);
  m_histExtrapolatedMuonTrackP = new TH1F("ExtrapTrackP","ExtrapTrackP",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/muons/ExtrapTrackP", m_histExtrapolatedMuonTrackP);
  m_histMuonSpectrometerTrackP = new TH1F("MuonSpectroTrackP","MuonSpectroTrackP",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/muons/MuonSpectroTrackP", m_histMuonSpectrometerTrackP);
  m_histInDetTrackP            = new TH1F("InDetTrackP","InDetTrackP",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/muons/InDetTrackP", m_histInDetTrackP);
  m_histMuonEnergyLoss         = new TH1F("eLossCalo","eLossCalo",100,0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/muons/eLossCalo", m_histMuonEnergyLoss);
  if( sc.isFailure() ){
      mLog << MSG::ERROR << "ROOT Hist MuonEnergyLoss registration failed" << endreq;
      return sc; 
  }
  /// create the container of W->jj
  m_WjjContainer = new IParticleContainer();

  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode MiscellaneousExamples::finalize() {
  MsgStream mLog( messageService(), name() );
  
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode MiscellaneousExamples::execute() {

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "in execute()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  /// do w->jj reconstruction
  sc = wjj();
  if (sc.isFailure()) {
     mLog << MSG::ERROR << "Failed to reconstruct W->jj" << endreq;
     return sc;
  }

  /// access to track and navigation tools
  sc = trackParticleAndNavigation();
  if (sc.isFailure()) {
     mLog << MSG::ERROR << "Failed to execute trackParticleAndNavigation()" << endreq;
     return sc;
  }

  // a symLink example
  sc = symLinkExample();
  if (sc.isFailure()) {
     mLog << MSG::ERROR << "Failed to execut symLinkExample()" << endreq;
     return sc;
  }

  return sc;
}

//////////////////////////////////////////////////////////////////////////////////
/// wjj() for the reconstruction of W->jj
/// called by execute() on event by event basis

StatusCode MiscellaneousExamples::wjj() {

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "in wjj()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  const JetCollection* jetTES=0;
  sc=m_storeGate->retrieve( jetTES, m_jetContainerName);
  if( sc.isFailure()  ||  !jetTES ) {
     mLog << MSG::WARNING
          << "No AOD jet container found in TDS "<<m_jetContainerName
          << endreq; 
     return StatusCode::SUCCESS;
  }  
  mLog << MSG::DEBUG << "JetCollection successfully retrieved " << m_jetContainerName<<endreq;
  
  /// number of jets in the jets AOD container
  m_histNumberOfJets->Fill( 1.0*jetTES->size(), m_eventWeight );

  /// the data type - the AOD knows how it was created
  /// this should be executed only once during the entire run
  if (jetTES->size() > 0) {
    m_dataType = (*jetTES)[0]->dataType();
    m_histDataType->Fill( 1.0*m_dataType, m_eventWeight );
  }
  
  /// iterators over the container 
  JetCollection::const_iterator jetItr  = jetTES->begin();
  JetCollection::const_iterator jetItrE = jetTES->end();
 
  for (; jetItr != jetItrE; ++jetItr) {
      m_histJetPt->Fill( (*jetItr)->pt(), m_eventWeight);
      m_histJetEta->Fill( (*jetItr)->eta(), m_eventWeight);

       /// constituent navigation
       NavigationToken<CaloCell,double> cellToken;
       (*jetItr)->fillToken(cellToken,double(1.));
       NavigationToken<CaloCell, double>::const_iterator c =  cellToken.begin();
       NavigationToken<CaloCell,double>::const_iterator cend = cellToken.end();
       mLog << MSG::DEBUG << "# cells found " << cellToken.size() <<  endreq;
       double etSum = 0;
       for(; c != cend; ++c) {
         const CaloCell* thisCell = *c;
         double weight = cellToken.getParameter(thisCell);
         double et    = weight * thisCell->et();
         etSum += et;
       }
       m_histSumEtCell->Fill(etSum, m_eventWeight);

  }

  /// m-jj variant mass with a selection on the jet transverse momenta
  AnalysisUtils::Combination<const JetCollection> combination(jetTES,2);
  JetVect jetPair;
  while (combination.goodOnes(this, jetPair, selectJet)) {
     CLHEP::HepLorentzVector jet1_fourMomentum = jetPair[0]->hlv();
     CLHEP::HepLorentzVector jet2_fourMomentum = jetPair[1]->hlv();
     double mjj = (jet1_fourMomentum+jet2_fourMomentum).m();
     m_histMjj->Fill(mjj, m_eventWeight);
  }

  mLog << MSG::DEBUG << "wjj() succeeded" << endreq;
  		
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// TrackParticle and Navigation
/// called by execute() on event by event basis

StatusCode MiscellaneousExamples::trackParticleAndNavigation() {

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "in trackParticleAndNavigation()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  const ElectronContainer* elecTES=0;
  sc=m_storeGate->retrieve( elecTES, m_electronContainerName);
  if( sc.isFailure()  ||  !elecTES ) {
     mLog << MSG::WARNING
          << "No AOD electron container found in TDS"
          << endreq; 
     return StatusCode::SUCCESS;
  }  
  mLog << MSG::DEBUG << "ElectronContainer successfully retrieved" << endreq;

  /// iterators over the container 
  ElectronContainer::const_iterator elecItr  = elecTES->begin();
  ElectronContainer::const_iterator elecItrE = elecTES->end();
 
  for (; elecItr != elecItrE; ++elecItr) {
     const Rec::TrackParticle *trackParticle = (*elecItr)->trackParticle();
     if (trackParticle != 0) { 
         double trackP = trackParticle->p();
        m_histElecTrackP->Fill(trackP, m_eventWeight);
     }

     /// back navigation to ESD to access Electron's egamma object
     const CaloCluster *cluster = (*elecItr)->cluster();
     if(cluster) { 
        double et = (cluster)->et();
        double eta = (cluster)->eta();
        double clusterE = et*cosh(eta);
        m_histElecClusterE->Fill(clusterE, m_eventWeight);
     }
  }

  /// access to MuonTracks
  const MuonContainer* muonTES=0;
  sc=m_storeGate->retrieve( muonTES, m_muonContainerName);
  if( sc.isFailure()  ||  !muonTES ) {
     mLog << MSG::WARNING
          << "No AOD muon container found in TDS"
          << endreq; 
     return StatusCode::SUCCESS;
  }  
  mLog << MSG::DEBUG << "MuonContainer successfully retrieved" << endreq;

  /// iterators over the container 
  MuonContainer::const_iterator muonItr  = muonTES->begin();
  MuonContainer::const_iterator muonItrE = muonTES->end();
 
  for (; muonItr != muonItrE; ++muonItr) {

     /// the combined muon spectrometer/InnerDet track
     if ( (*muonItr)->hasCombinedMuonTrackParticle() ) {
       const Rec::TrackParticle * combinedTrack = (*muonItr)->combinedMuonTrackParticle();
       double trackP = combinedTrack->p();
       m_histCombinedMuonTrackP->Fill(trackP, m_eventWeight);
       mLog << MSG::DEBUG << "CombinedMuon TrackParticle accessed" << endreq;
     }

     /// the muon extrapolated track to the vertex
     if ( (*muonItr)->hasMuonExtrapolatedTrackParticle() ) {
       const Rec::TrackParticle * extrapTrack = (*muonItr)->muonExtrapolatedTrackParticle();
       double trackP = extrapTrack->p();
       m_histExtrapolatedMuonTrackP->Fill(trackP, m_eventWeight);
       mLog << MSG::DEBUG << "Extrapolated Muon TrackParticle accessed" << endreq;
       /** get the TrackParameters at the Muon Spectrometer */
       const auto& trackParametersAtSpectrometer = extrapTrack->trackParameters().front();
       //Trk::GlobalMomentum momentum = trackParametersAtSpectrometer->momentum();
       trackP = trackParametersAtSpectrometer->momentum().mag();
       m_histMuonSpectrometerTrackP->Fill(trackP, m_eventWeight);
     }

     /// the InnerDet track
     if ( (*muonItr)->hasInDetTrackParticle() ) {
       const Rec::TrackParticle * inDetTrack = (*muonItr)->inDetTrackParticle();
       double trackP = inDetTrack->p();
       m_histInDetTrackP->Fill(trackP, m_eventWeight);
       mLog << MSG::DEBUG << "Inner Detector TrackParticle accessed" << endreq;
     }

     std::pair<double, double> eloss_error = (*muonItr)->energyLoss();
     double eLoss = eloss_error.first;
     ///double error = eLoss_error.second;
     m_histMuonEnergyLoss->Fill(eLoss, m_eventWeight);
     mLog << MSG::DEBUG << "Muon energy loss in Calo access" << endreq;
  }

  mLog << MSG::DEBUG << "trackParticleAndNavigation() succeeded" << endreq;

  return sc;
}

template<typename InContainer, typename SymContainer>
StatusCode create_symlink(MsgStream& msg, 
			  StoreGateSvc* sg, 
			  const std::string& sgkey)
{
  const InContainer * in  = 0;
  if ( !sg->retrieve (in, sgkey).isSuccess() ||
       0 == in ) {
    msg << MSG::ERROR
	<< "Could not retrieve " << ClassName<InContainer>::name() 
	<< " at [" << sgkey << "] !!"
	<< endreq;
    return StatusCode::FAILURE;
  }

  const SymContainer* sym = 0;
  if ( sg->retrieve (sym, sgkey).isSuccess() ) {
    // already symlinked...
    return StatusCode::SUCCESS;
  }

  if ( !sg->symLink (in, sym).isSuccess() ) {
    msg << MSG::ERROR
	<< "Could not create symlink to " << ClassName<SymContainer>::name()
	<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////
/// symLink and CompositeParticle example
/// called by execute() on event by event basis

StatusCode MiscellaneousExamples::symLinkExample() {

  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "in symLinkExample()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  /// make sure the W ->jj container is empty
  m_WjjContainer->clear();

  // FIXME: remove when proper JetCollection impl. available.
  if ( !create_symlink<JetCollection,IParticleContainer>
       (mLog, m_storeGate, m_jetContainerName).isSuccess() ) {
    mLog<< MSG::WARNING << "Could not create symLink for JetCollection "<<endreq;
    return StatusCode::FAILURE;
  }

  const IParticleContainer* iparticles = 0;
  sc=m_storeGate->retrieve( iparticles, m_jetContainerName);
  if( sc.isFailure()  ||  !iparticles ) {
     mLog << MSG::WARNING
          << "No AOD jet container found in TDS as symLink "<<m_jetContainerName 
          << endreq; 
     return StatusCode::SUCCESS;
  }  
  mLog << MSG::DEBUG << "IParticleContainer successfully retrieved as symLink " << m_jetContainerName<<endreq;

  /// ease handwriting:
  typedef ElementLink<IParticleContainer>  IParticleLink_t;

  /// iterators over the container 
  IParticleContainer::const_iterator partItr  = iparticles->begin();
  IParticleContainer::const_iterator partItrE = iparticles->end();

  for (; partItr != partItrE; ++partItr) {
    m_histSymLinkPt->Fill( (*partItr)->pt(), m_eventWeight);
  }

  /// m-jj variant mass with a selection on the jet transverse momenta
  AnalysisUtils::Combination<const IParticleContainer> combination(iparticles,2);
  ParticleVect jetPair;
  while (combination.goodOnes(this, jetPair, selectParticle)) {

     /// the composite particle - this snippet of code should be modified for 9.0.0 and above
     /// due to a modification of the CompositeParticle class
     CompositeParticle * Wjj = new CompositeParticle;
     Wjj->add( INav4MomLink(&*jetPair[0], *iparticles),
	       INav4MomLink(&*jetPair[1], *iparticles) );

     Wjj->set_charge(1);            /// this is arbitrary :: I will assume W->lnu has charge=-1
     Wjj->set_pdgId(PDG::W_plus);   /// this arbitrary :: I will assume W_minus -> lnu
     Wjj->set_dataType(m_dataType);
     double massW = Wjj->m();
     m_histSymLinkMjj->Fill(massW, m_eventWeight);
     using std::abs;
     if ( abs(massW-mW) < m_deltaMjj) m_WjjContainer->push_back(Wjj);	
  }   

  mLog << MSG::DEBUG << "symLinkExample() succeeded" << endreq;

  return sc;
}


/// jet selection function in jet-jet combination
/// this function a friend of MiscellaneousExamples

bool selectJet(MiscellaneousExamples *self, const JetVect &jj) {
  bool test = (jj[0]->pt() > self->m_jetPtCut) &&
              (jj[1]->pt() > self->m_jetPtCut);
  return (test);
}

/// jet selection function in jet-jet combination
/// this function a friend of MiscellaneousExamples

bool selectParticle(MiscellaneousExamples *self, const ParticleVect &jj) {
  bool test = (jj[0]->pt() > self->m_jetPtCut) &&
              (jj[1]->pt() > self->m_jetPtCut);
  return test;
}

