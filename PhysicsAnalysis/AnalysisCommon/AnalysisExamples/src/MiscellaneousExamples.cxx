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
//static const double topMass    = 175.0*CLHEP::GeV;

using namespace Analysis;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

MiscellaneousExamples::MiscellaneousExamples(const std::string& name,
  ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_analysisTools( "AnalysisTools", this ),
    m_thistSvc(nullptr),
    m_dataType(),
    m_WjjContainer(nullptr),
    m_histNumberOfJets(nullptr),
    m_histJetPt(nullptr),
    m_histSymLinkPt(nullptr),
    m_histSymLinkMjj(nullptr),
    m_histJetEta(nullptr),
    m_histMjj(nullptr),
    m_histDataType(nullptr),
    m_histSumEtCell(nullptr),
    m_histElecTrackP(nullptr),
    m_histElecClusterE(nullptr),
    m_histCombinedMuonTrackP(nullptr),
    m_histExtrapolatedMuonTrackP(nullptr),
    m_histMuonSpectrometerTrackP(nullptr),
    m_histInDetTrackP(nullptr),
    m_histMuonEnergyLoss(nullptr)
{

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

  ATH_MSG_INFO ( "Initializing MiscellaneousExamples" );

  ATH_CHECK(  m_analysisTools.retrieve() );
  ATH_CHECK( service("THistSvc", m_thistSvc) );

  /// reconstructed jet histograms
  m_histNumberOfJets   = new TH1F("NumberOfJets","N jets",20,0,20);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets/NumberOfJets", m_histNumberOfJets) );
  m_histJetPt          = new TH1F("jet_pt","pt of jet",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets/jet_pt", m_histJetPt) );
  m_histJetEta         = new TH1F("jet_eta","eta of jet",100,-5.0,5.0);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets", m_histJetEta) );
  m_histMjj            = new TH1F("jj mass","mjj",80,0,160*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets/jj mass", m_histMjj) );
  m_histSumEtCell      = new TH1F("jet_et","Sum Et Cells",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets/jet_et", m_histSumEtCell) );
  m_histSymLinkPt      = new TH1F("jetSimLink_pt","pt of symLinkjet",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets/jetSimLink_pt", m_histSymLinkPt) );
  m_histSymLinkMjj    =  new TH1F("SimLink_Mjj","Mjj of symLinkjets",80,0,160.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/jets/SimLink_Mjj", m_histSymLinkMjj) );

  m_histDataType     = new TH1F("data type","dataType",5,0,5);
  ATH_CHECK( m_thistSvc->regHist("/AANT/general/data type", m_histDataType) );
  m_histElecTrackP   = new TH1F("elecTrackP","e Track Momentum",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/ectrons/elecTrackP", m_histElecTrackP) );
  m_histElecClusterE = new TH1F("elecClusterE","e Cluster Energy",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/electrons/elecClusterE", m_histElecClusterE) );

  m_histCombinedMuonTrackP     = new TH1F("CombinedTrackP","CombinedTrackP",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/muons/CombinedTrackP", m_histCombinedMuonTrackP) );
  m_histExtrapolatedMuonTrackP = new TH1F("ExtrapTrackP","ExtrapTrackP",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/muons/ExtrapTrackP", m_histExtrapolatedMuonTrackP) );
  m_histMuonSpectrometerTrackP = new TH1F("MuonSpectroTrackP","MuonSpectroTrackP",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/muons/MuonSpectroTrackP", m_histMuonSpectrometerTrackP) );
  m_histInDetTrackP            = new TH1F("InDetTrackP","InDetTrackP",100,0,500.*CLHEP::GeV);

  ATH_CHECK( m_thistSvc->regHist("/AANT/muons/InDetTrackP", m_histInDetTrackP) );
  m_histMuonEnergyLoss         = new TH1F("eLossCalo","eLossCalo",100,0,500.*CLHEP::GeV);
  ATH_CHECK( m_thistSvc->regHist("/AANT/muons/eLossCalo", m_histMuonEnergyLoss) );
  /// create the container of W->jj
  m_WjjContainer = new IParticleContainer();

  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode MiscellaneousExamples::finalize() {
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode MiscellaneousExamples::execute() {

  ATH_MSG_DEBUG ( "in execute()" );

  ATH_CHECK( wjj() );
  ATH_CHECK( trackParticleAndNavigation() );
  ATH_CHECK( symLinkExample() );

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// wjj() for the reconstruction of W->jj
/// called by execute() on event by event basis

StatusCode MiscellaneousExamples::wjj() {

  ATH_MSG_DEBUG ( "in wjj()" );

  const JetCollection* jetTES=0;
  StatusCode sc=evtStore()->retrieve( jetTES, m_jetContainerName);
  if( sc.isFailure()  ||  !jetTES ) {
    ATH_MSG_WARNING ( "No AOD jet container found in TDS "<<m_jetContainerName);
    return StatusCode::SUCCESS;
  }  
  ATH_MSG_DEBUG ( "JetCollection successfully retrieved " << m_jetContainerName);
  
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
       ATH_MSG_DEBUG ( "# cells found " << cellToken.size() );
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

  ATH_MSG_DEBUG ( "wjj() succeeded" );
  		
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// TrackParticle and Navigation
/// called by execute() on event by event basis

StatusCode MiscellaneousExamples::trackParticleAndNavigation() {

  ATH_MSG_DEBUG ( "in trackParticleAndNavigation()" );

  const ElectronContainer* elecTES=0;
  StatusCode sc=evtStore()->retrieve( elecTES, m_electronContainerName);
  if( sc.isFailure()  ||  !elecTES ) {
     ATH_MSG_WARNING ( "No AOD electron container found in TDS" );
     return StatusCode::SUCCESS;
  }  
  ATH_MSG_DEBUG ( "ElectronContainer successfully retrieved" );

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
  sc=evtStore()->retrieve( muonTES, m_muonContainerName);
  if( sc.isFailure()  ||  !muonTES ) {
     ATH_MSG_WARNING ( "No AOD muon container found in TDS" );
     return StatusCode::SUCCESS;
  }  
  ATH_MSG_DEBUG ( "MuonContainer successfully retrieved" );

  /// iterators over the container 
  MuonContainer::const_iterator muonItr  = muonTES->begin();
  MuonContainer::const_iterator muonItrE = muonTES->end();
 
  for (; muonItr != muonItrE; ++muonItr) {

     /// the combined muon spectrometer/InnerDet track
     if ( (*muonItr)->hasCombinedMuonTrackParticle() ) {
       const Rec::TrackParticle * combinedTrack = (*muonItr)->combinedMuonTrackParticle();
       double trackP = combinedTrack->p();
       m_histCombinedMuonTrackP->Fill(trackP, m_eventWeight);
       ATH_MSG_DEBUG ( "CombinedMuon TrackParticle accessed" );
     }

     /// the muon extrapolated track to the vertex
     if ( (*muonItr)->hasMuonExtrapolatedTrackParticle() ) {
       const Rec::TrackParticle * extrapTrack = (*muonItr)->muonExtrapolatedTrackParticle();
       double trackP = extrapTrack->p();
       m_histExtrapolatedMuonTrackP->Fill(trackP, m_eventWeight);
       ATH_MSG_DEBUG ( "Extrapolated Muon TrackParticle accessed" );
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
       ATH_MSG_DEBUG ( "Inner Detector TrackParticle accessed" );
     }

     std::pair<double, double> eloss_error = (*muonItr)->energyLoss();
     double eLoss = eloss_error.first;
     ///double error = eLoss_error.second;
     m_histMuonEnergyLoss->Fill(eLoss, m_eventWeight);
     ATH_MSG_DEBUG ( "Muon energy loss in Calo access" );
  }

  ATH_MSG_DEBUG ( "trackParticleAndNavigation() succeeded" );

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
    msg << MSG::ERROR<< "Could not retrieve " << ClassName<InContainer>::name() 
	<< " at [" << sgkey << "] !!" << endreq;
    return StatusCode::FAILURE;
  }

  const SymContainer* sym = 0;
  if ( sg->retrieve (sym, sgkey).isSuccess() ) {
    // already symlinked...
    return StatusCode::SUCCESS;
  }

  if ( !sg->symLink (in, sym).isSuccess() ) {
    msg <<  "Could not create symlink to " << ClassName<SymContainer>::name()
        << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////
/// symLink and CompositeParticle example
/// called by execute() on event by event basis

StatusCode MiscellaneousExamples::symLinkExample() {

  ATH_MSG_DEBUG ( "in symLinkExample()" );

  /// make sure the W ->jj container is empty
  m_WjjContainer->clear();

  // FIXME: remove when proper JetCollection impl. available.
  if ( !create_symlink<JetCollection,IParticleContainer>
       (msg(), &*evtStore(), m_jetContainerName).isSuccess() ) {
    ATH_MSG_WARNING ( "Could not create symLink for JetCollection ");
    return StatusCode::FAILURE;
  }

  const IParticleContainer* iparticles = 0;
  StatusCode sc=evtStore()->retrieve( iparticles, m_jetContainerName);
  if( sc.isFailure()  ||  !iparticles ) {
    ATH_MSG_WARNING
      ( "No AOD jet container found in TDS as symLink "<<m_jetContainerName );
     return StatusCode::SUCCESS;
  }  
  ATH_MSG_DEBUG ( "IParticleContainer successfully retrieved as symLink " << m_jetContainerName);

  /// ease handwriting:
  //typedef ElementLink<IParticleContainer>  IParticleLink_t;

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

  ATH_MSG_DEBUG ( "symLinkExample() succeeded" );

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

