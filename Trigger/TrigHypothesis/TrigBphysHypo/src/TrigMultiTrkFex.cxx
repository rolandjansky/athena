// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2MultiMuFex.cxx
 **
 **   Description: EF hypothesis algorithms for 3mu and 4mu signatures 
 ** 
 **  author Olya Igonkina (Nikhef)
 **             
 **************************************************************************/ 
 
#include "TrigMultiTrkFex.h"
#include "TrigBphysHelperUtilsTool.h"

#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "FourMomUtils/P4Helpers.h"


// Define the bins for acceptance-monitoring histogram
#define ACCEPT_Input                  0
#define ACCEPT_hltExecute             1	
#define ACCEPT_PassNL2SAMuons         2	
#define ACCEPT_PassNL2CombMuons       3	
#define ACCEPT_PassNEFMuons           4	
#define ACCEPT_PassNTracks            5	
#define ACCEPT_HighPtTrack            6	
#define ACCEPT_NTrkCharge             7	
#define ACCEPT_NTrkMass               8	
#define ACCEPT_NTrkVertexChi2         9	
#define ACCEPT_PairCharge             10	
#define ACCEPT_PairMass               11
#define ACCEPT_RecordedCollection     12
				      

#define ERROR_AlgorithmProblem           0
#define ERROR_BphysColl_Fails            1


TrigMultiTrkFex::TrigMultiTrkFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator)
  ,m_bphysHelperTool("TrigBphysHelperUtilsTool")
  ,m_BmmHypTot(0)
  
  , m_trackCollectionKey()
  , m_bphysCollectionKey()
  , m_nTrk (2)
  , m_nTrkQ (-1)
  , m_nTrkVertexChi2 (-1)
  , m_trkMass(139.57018)  // pion
  , m_ptTrkMin()
  , m_nTrkMassMin() // this has to be in pair
  , m_nTrkMassMax()
  , m_diTrkMassMin() // this has to be in pair
  , m_diTrkMassMax()
  , m_nEfMuon(0)
  , m_nL2CombMuon (0)
  , m_nL2SAMuon (0)
  , m_ptMuonMin()
  , m_mindR(0.005)
  ,m_acceptAll(false)
				  //counters
  , m_countTotalEvents(0)
  , m_countPassedEvents(0)
  , m_countPassedCombination(0)

{

  // Read cuts

  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("trackCollectionKey", m_trackCollectionKey  = "" );
  declareProperty("bphysCollectionKey", m_bphysCollectionKey  = "MultiTrkFex" );
  declareProperty("nTrk"           , m_nTrk 	     = 2 );
  declareProperty("nTrkCharge"     , m_nTrkQ 	     = -1 );
  declareProperty("nTrkVertexChi2" , m_nTrkVertexChi2  = -1 );
  declareProperty("trkMass"        , m_trkMass    =  139.57018 );  
  declareProperty("ptTrkMin"       , m_ptTrkMin       );
  declareProperty("diTrkMassMin"   , m_diTrkMassMin    );
  declareProperty("diTrkMassMax"   , m_diTrkMassMax    );
  declareProperty("diTrkCharge"    , m_diTrkQ 	     = 0 );
  declareProperty("nTrkMassMin"   , m_nTrkMassMin    );
  declareProperty("nTrkMassMax"   , m_nTrkMassMax    );
  declareProperty("nEfMuon"        , m_nEfMuon    = 0 );
  declareProperty("nL2CombMuon"    , m_nL2CombMuon    = 0 );
  declareProperty("nL2SAMuon"     , m_nL2SAMuon     =0  );
  declareProperty("ptMuonMin"     , m_ptMuonMin      );
  declareProperty("overlapdR"     , m_mindR    = 0.01  );  

  declareMonitoredStdContainer("Errors"        , m_mon_Errors                  , AutoClear);
  declareMonitoredStdContainer("Acceptance"    , m_mon_Acceptance              , AutoClear);
  declareMonitoredStdContainer("NTrkMass",       m_mon_NTrkMass    , AutoClear  );
  declareMonitoredStdContainer("NTrkFitMass",    m_mon_NTrkFitMass    , AutoClear  );
  declareMonitoredStdContainer("NTrkChi2",       m_mon_NTrkChi2    , AutoClear  );
  declareMonitoredStdContainer("PairMass",       m_mon_pairMass     , AutoClear );
  declareMonitoredVariable("NTrk_all",           m_mon_NTrk); 
  declareMonitoredVariable("NTrk_highpt",        m_mon_highptNTrk); 
  declareMonitoredVariable("NTrkHighPt_accepted",  m_mon_accepted_highptNTrk); 
  declareMonitoredVariable("NPair_all",           m_mon_NPair); 
  declareMonitoredVariable("NPair_accepted",      m_mon_acceptedNPair); 
}

TrigMultiTrkFex::~TrigMultiTrkFex()
{ }




bool TrigMultiTrkFex::passNTracks(int nObjMin,
				  const std::vector<float> & ptObjMin,
				  std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
				  std::vector<ElementLink<xAOD::TrackParticleContainer> > & outVec,
				  const std::string&  collectionKey,
				  float mindR) 

{
  if( nObjMin <= 0 ) return true;
  ElementLinkVector<xAOD::TrackParticleContainer> inVecColl;
  outVec.clear();
  
  float mindR2 = mindR*mindR;

  unsigned int nTEs = inputTE.size();
  for ( unsigned int i=0; i < nTEs; ++i) {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << " TE input array " << i << endmsg;
    unsigned int mTEs = inputTE[i].size();
   for ( unsigned int j=0; j < mTEs; ++j) {
     if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << " TE input  " << j << endmsg;
    if(getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(inputTE[i][j], inVecColl, collectionKey)==HLT::OK ) {
      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "got track container  " << inVecColl.size() << endmsg;
      for( const auto & efmu : inVecColl){	//auto const & inVec: inVecColl ) 
	  // check for overlap
	  bool found = false;
	  for(const auto& part : outVec ){
	    if( (*part)->charge() *  (*efmu)->charge() < 0 ) continue;
	    double deta = (*part)->eta() - (*efmu)->eta();
	    double dphi = (*part)->phi() - (*efmu)->phi();
	    double deltaR2 = deta*deta +dphi*dphi;
	    if( deltaR2 <= mindR2){
	      found = true;
 	      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Duplicated track pt/eta/phi/q "
						   << (*efmu)->pt()<< " /  "
						   << (*efmu)->eta()<< " /  "
						   << (*efmu)->phi()<< " /  "
						   << (*efmu)->charge()<< " skip " <<deltaR2 <<  endmsg;
	    }
	  }
	  if( !found ) {
	    outVec.push_back(efmu);	    	
	    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Found track pt/eta/phi/q "
						 << (*efmu)->pt()<< " /  "  
						 << (*efmu)->eta()<< " /  "
						 << (*efmu)->phi()<< " /  "
						 << (*efmu)->charge()<< endmsg; 
	  }
      }//}// end loop over muons in one TE
    } // end getFeaturesLinks
   }} // end loop over all TEs

    //=== check if it is enough muons
  if( (int)outVec.size() < nObjMin ) {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Rejecting: "
    					 <<" #Ntracks= " <<  outVec.size() 
    					 << " while need "<< nObjMin << endmsg;
    return false;
  }
  //== check that muons have correct pts
  //OI easy sorting does not seem to work
  std::sort( std::begin(outVec), std::end(outVec), sortTracks); 


  unsigned int Ncheck = std::min( nObjMin, int(ptObjMin.size()) );
  bool failMuonPt = false;
  for ( unsigned int ipt=0; ipt < Ncheck; ++ipt) {
    float pt = (*outVec[ipt])->pt();
    // check if value in GeV or MeV, if it was >350 GeV and multiplied by 1000, it does not matter
    if( pt < 350. && pt>0.01 ) pt *= 1000.;
    if(  pt < ptObjMin[ipt] ) failMuonPt = true;	  
  }
  if( failMuonPt ){
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail track pt cut" << endmsg;
    return false;
  }
  // here would be good to limit number of objects to the minimum

  return true;
}

/*
template <typename T>
vector<size_t> sortELV(const ElementLinkVector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return (*v[i1])->pt() < (*v[i2])->pt();});

  return idx;
}
*/

HLT::ErrorCode TrigMultiTrkFex::hltInitialize()
{
    m_countTotalEvents = 0;
    m_countPassedEvents = 0;
    m_countPassedCombination = 0;

  if (msgLvl() <= MSG::INFO) {
    //msg() << MSG::INFO << "Initialization ..." << endmsg;
    msg() << MSG::INFO << "AcceptAll            = "
	  << (m_acceptAll==true ? "True" : "False") << endmsg;
    
    
    std::sort(m_ptTrkMin.begin(),m_ptTrkMin.end(), std::greater<float>());
    std::sort(m_ptMuonMin.begin(),m_ptMuonMin.end(), std::greater<float>());
    
    if(  m_trackCollectionKey != "" ) msg() << MSG::INFO << "trackCollectionKey"<< m_trackCollectionKey  << endmsg;
    msg() << MSG::INFO << "Select "           << m_nTrk 	<< " tracks " << endmsg;    
    msg() << MSG::INFO << " with pts ";	
    for(float pt :  m_ptTrkMin)  msg() << MSG::INFO << pt<<", ";
    msg() << MSG::INFO << endmsg;  
  }
  
  if(  m_nTrkMassMin.size() !=  m_nTrkMassMax.size() ){
    msg() << MSG::ERROR << " Unequal size of n-track mass cuts, please fix" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  if(  m_diTrkMassMin.size() !=  m_diTrkMassMax.size() ){
    msg() << MSG::ERROR << " Unequal size of di-track mass cuts, please fix" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if( m_nTrkQ >= 0 ) {
    msg() << MSG::INFO << " N-track system has to have charge "<< m_nTrkQ <<endmsg;
    if( m_nTrkQ%2 != m_nTrk%2 ) {
      msg() << MSG::ERROR << " nTrkCharge parameter makes no sense "<< endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  }
  
  if( m_nTrkVertexChi2 >= 0 ) {
    msg() << MSG::INFO << " N-track system has to have vertex chi2  "<< m_nTrkVertexChi2 <<endmsg;
  }

  if (msgLvl() <= MSG::INFO &&  m_nTrkMassMin.size() > 0 ){
    msg() << MSG::INFO << "Require N-track Mass in ";
    for( size_t imass = 0; imass < m_nTrkMassMin.size(); ++imass )
      msg() << MSG::INFO << "[" << m_nTrkMassMin[imass]<<","<<m_nTrkMassMax[imass] <<"]";
    msg() << MSG::INFO << endmsg;
  }
  
  if (msgLvl() <= MSG::INFO &&  m_diTrkMassMin.size() > 0 ){
    msg() << MSG::INFO << "Require di-track Mass in ";
    for( size_t imass = 0; imass < m_diTrkMassMin.size(); ++imass )
      msg() << MSG::INFO << "[" << m_diTrkMassMin[imass]<<","<<m_diTrkMassMax[imass] <<"]";
    msg() << MSG::INFO << endmsg;
    if( m_diTrkQ >= 0 ) {
      msg() << MSG::INFO << " pair has to have charge "<< m_diTrkQ <<endmsg;
      if( m_diTrkQ != 0 && m_diTrkQ != 2 ) {
	msg() << MSG::ERROR << " diTrkCharge parameter makes no sense "<< endmsg;
	return HLT::BAD_JOB_SETUP;
      }
    }
    
  }
  if (msgLvl() <= MSG::INFO) {

  if( m_nEfMuon ) msg() << MSG::INFO << "require at least "<< m_nEfMuon <<" EF Muons" << endmsg;
  if( m_nL2CombMuon ) msg() << MSG::INFO << "require at least "<< m_nL2CombMuon <<" EF Muons" << endmsg;
  if( m_nL2SAMuon ) msg() << MSG::INFO << "require at least "<< m_nL2SAMuon <<" EF Muons" << endmsg;
  if(  m_nEfMuon ||m_nL2CombMuon ||   m_nL2SAMuon ){
    msg() << MSG::INFO << " Muons should have  pts ";	
    for(float pt :  m_ptMuonMin)  msg() << MSG::INFO << pt <<", ";
    msg() << MSG::INFO << endmsg;
    }
  }
  if (msgLvl() <= MSG::INFO) {
     msg() << MSG::INFO << " Overlap removal dR<"<<m_mindR<< endmsg;
  }

  if ( timerSvc() ) {
    m_BmmHypTot = addTimer("TrigMultiTrkFex");
  }
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endmsg;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endmsg;
    }
    

   
  return HLT::OK;
}

HLT::ErrorCode TrigMultiTrkFex::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endmsg;
  MsgStream log(msgSvc(), name());
  
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigMultiTrkFex -------------|" << endmsg;
  msg() << MSG::INFO << "Run on events " << m_countTotalEvents <<  endmsg;
  msg() << MSG::INFO << "Passed events " << m_countPassedEvents<<  endmsg;
  msg() << MSG::INFO << "Passed combinations " << m_countPassedCombination<<  endmsg;

  return HLT::OK;
}


//-------------------------------------------------------------------------------------
// HLT::ErrorCode TrigMultiTrkFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass) {
  
//   //OI: we do not care from which RoI tracks come from as long as they give good vertex and mass
// m_mon_Acceptance.push_back( ACCEPT_Input );

//    // event info
//     uint32_t runNumber(0), evtNumber(0), lbBlock(0);
//     if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
//         msg() << MSG::WARNING << "Error retriving EventInfo" << endmsg;
// 	m_mon_Errors.push_back( ERROR_No_EventInfo );
//     }

 
//   if ( msgLvl() <= MSG::DEBUG )
//     msg() << MSG::DEBUG << "Running TrigMultiTrkFex::acceptInputs" << endmsg;

//   if ( timerSvc() )    m_BmmHypTot->start();
  
//   m_countTotalEvents++;

//   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Number of input TEs " << inputTE.size() << endmsg;
//   pass=true;
//   return HLT::OK;

// }


HLT::ErrorCode TrigMultiTrkFex::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output)
{
  // start monitoring
  beforeExecMonitors().ignore();

  m_mon_Errors.clear();
  m_mon_Acceptance.clear();
  m_mon_NTrk = 0;
  m_mon_highptNTrk = 0;
  m_mon_accepted_highptNTrk = 0;
  m_mon_NTrkMass.clear();
  m_mon_NTrkFitMass.clear();
  m_mon_NTrkChi2.clear();
  m_mon_NPair = 0;
  m_mon_acceptedNPair = 0;
  m_mon_pairMass.clear();


    m_mon_Acceptance.push_back( ACCEPT_hltExecute );
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigMultiTrk hltExecute" << endmsg;

    
    
    //unsigned int nTEs = inputTE.size();

    // here we loop over tracks
    // Check if muons are required.
        
    //========  check if we have enough SA muons :  =====================
    std::vector<ElementLink<xAOD::L2StandAloneMuonContainer> > l2samuons;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Checking L2SA muon cut"<< endmsg; 
    bool passMuon = passNObjects<xAOD::L2StandAloneMuonContainer,
      std::vector<ElementLink<xAOD::L2StandAloneMuonContainer> > > ( 
			     m_nL2SAMuon, m_ptMuonMin, inputTE, l2samuons, "", m_mindR);
    if( !passMuon ){
      if ( timerSvc() )  m_BmmHypTot->stop(); 
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed L2SA muon cut"<< endmsg; 
      //pass = false;
      afterExecMonitors().ignore();   
      return HLT::OK;
    }else{
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Passed L2SA cut"<< endmsg; 
    }    

    m_mon_Acceptance.push_back( ACCEPT_PassNL2SAMuons );
    //========  check if we have enough L2 Combined muons :  =====================
    std::vector<ElementLink<xAOD::L2CombinedMuonContainer> > l2combmuons;
    //  passMuon = passNObjects<std::vector<const xAOD::L2CombinedMuonContainer*>, 
    // ElementLinkVector<xAOD::L2CombinedMuonContainer> >( 
    passMuon = passNObjects<xAOD::L2CombinedMuonContainer, 
      std::vector<ElementLink<xAOD::L2CombinedMuonContainer> >  >( 
			     m_nL2CombMuon, m_ptMuonMin, inputTE, l2combmuons, "", m_mindR);
    if( !passMuon ){
      if ( timerSvc() )  m_BmmHypTot->stop();
      //pass = false;
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed L2Comb muon cut"<< endmsg; 
      afterExecMonitors().ignore();   
      return HLT::OK;
    }
    m_mon_Acceptance.push_back( ACCEPT_PassNL2CombMuons );

    //========  check if we have enough EF muons :  =====================
    std::vector<ElementLink<xAOD::MuonContainer> > efmuons; // just a collection of pointers, not copies
    passMuon = passNObjects<xAOD::MuonContainer, 
			    std::vector<ElementLink<xAOD::MuonContainer> > >( m_nEfMuon, m_ptMuonMin, 
									      inputTE, efmuons, "", m_mindR);
    if( !passMuon ){
      if ( timerSvc() )  m_BmmHypTot->stop();
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed EF muon cut"<< endmsg; 
      afterExecMonitors().ignore();   
      return HLT::OK;
    }
    m_mon_Acceptance.push_back( ACCEPT_PassNEFMuons );
    //======== now make track container : =======================
    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > tracks;
    bool passTrack = passNTracks( m_nTrk, m_ptTrkMin, inputTE, tracks, m_trackCollectionKey , m_mindR);
    if( !passTrack || int(tracks.size()) < m_nTrk ){
      if ( timerSvc() )  m_BmmHypTot->stop();
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed nTrack cut"<< endmsg;     //pass = false;
      afterExecMonitors().ignore();   
    return HLT::OK;
    }else{
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Passed nTrack cut"<< endmsg; 
    }


    if( (int)tracks.size() < m_nTrk ) {
      if ( timerSvc() )  m_BmmHypTot->stop();
      if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "You should never get to this point - check code"<< endmsg;  
      m_mon_Errors.push_back(ERROR_AlgorithmProblem);
      afterExecMonitors().ignore();   
    return HLT::OK;
    }
    m_mon_Acceptance.push_back( ACCEPT_PassNTracks );
    m_mon_NTrk =  tracks.size() ;
  //============= limit list to Ntrack leading tracks ===========
    bool passHighPtTrack = false;
    bool passNTrkCharge = false;
    bool passNTrkVertexChi2 = false;
    bool passNTrkMass = false;
    bool passPairCharge = false;
    bool passPairMass = false;

    //std::vector<const xAOD::TrackParticle*> highptTracks; 
    std::vector<ElementLink<xAOD::TrackParticleContainer> > highptTracks; 

    // preserve all tracks with pt passing lowest requested
    float lowestPt = m_ptTrkMin.back();
    for(const auto& trk :  tracks ){
      if( (*trk)->pt() < lowestPt ) continue;      
      highptTracks.push_back( trk );

      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "Select track:   "
	      << " pt: " <<  (*trk)->pt()
	      << " eta: " << (*trk)->eta()
	      << " phi: " << (*trk)->phi()
	      << " q: "   << (*trk)->charge()
	      << endmsg;
    }

    m_mon_highptNTrk = highptTracks.size() ;
    passHighPtTrack = true;
    // now all preselection cuts passed, make output collection now

    xAOD::TrigBphysContainer * xAODTrigBphysColl = new xAOD::TrigBphysContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    xAODTrigBphysColl->setStore(&xAODTrigBphysAuxColl);
    std::vector<double> masses(2,m_trkMass);    
 

    //=====make all requested m_nTrk combinations =================

    std::string bitmask( m_nTrk, 1); // K leading 1's
    size_t NtracksTotal = highptTracks.size();
    bitmask.resize(NtracksTotal, 0); // N-K trailing 0's

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Making combinations with "<< m_nTrk<<" tracks out of "<<NtracksTotal<<endmsg;

    // permute bitmask
    std::vector<ElementLink<xAOD::TrackParticleContainer> > thisIterationTracks; 
    do {
      
      thisIterationTracks.clear();
      std::ostringstream combi;
      std::ostringstream hpts;
      for (size_t i = 0; i < NtracksTotal; ++i) // [0..N-1] integers	
	{
	  if (bitmask[i]) {
	    thisIterationTracks.push_back( highptTracks[i] );// std::cout << " " << i;
	    combi << i << " ";  // for debug
	    hpts << (*highptTracks[i])->pt() << ", " ;
	  }
	}
      
      //=== now check if combination is OK
      ATH_MSG_DEBUG( " Combination "<< combi.str() 
                     << ", Ntrk="<< thisIterationTracks.size() 
                     << " pts=["<< hpts.str() << " ]" );
      

      //==== re-check pts
      bool passPts = true;
      for(size_t ipt = 0; ipt <  thisIterationTracks.size(); ++ipt )
	if( (*thisIterationTracks[ipt])->pt() < m_ptTrkMin[ipt] ){
	  passPts = false;
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " fail pt cuts" << endmsg;
	  break;
	}
      if( !passPts ) continue;

      //== first check charge
      if( m_nTrkQ >= 0 ){
	int charge = 0;
	//for(const auto& trk : thisIterationTracks)
 	for(ElementLink<xAOD::TrackParticleContainer> trk : thisIterationTracks) 
	  charge += ( (*trk)->charge() > 0 ) ? 1 : -1 ;
	if( abs(charge) != m_nTrkQ ){
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " |Charge| = "<< abs(charge)
					   << " required "<<m_nTrkQ << " thus fail" << endmsg;
	  continue;
	}else{
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " |Charge| = "<< abs(charge) 
					   << " required "<<m_nTrkQ << " thus pass" << endmsg;
	}	
	passNTrkCharge = true;
      }
    
      // now check if muonMatch is requested, and if we have enough muons here




      //=== calculate total mass of m_nTrk objects ================
      double totalMass = 0;
      float trkMassSqr = m_trkMass*m_trkMass;
      
      if( m_nTrkMassMin.size() > 0 ){
	double px =0;
	double py =0;
	double pz =0;
	double E =0;
       for( size_t itrk0 = 0; itrk0 <thisIterationTracks.size() ; ++itrk0){
	 auto mom = (*thisIterationTracks[itrk0])->p4();

	 px += mom.Px();
	 py += mom.Py();
	 pz += mom.Pz();
	 // treat them all as muons
	 double e2 =  mom.P()*mom.P() + trkMassSqr; // 105.65^2 
	 if( e2 > 0 ) E += sqrt(e2);
       }
       double m2 = E*E - px*px - py*py -pz*pz;
       totalMass = (m2 < 0) ? 0 : sqrt(m2);
       m_mon_NTrkMass.push_back(totalMass*0.001);
       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "combination mass = "<< totalMass << endmsg;
       // check mass window now
       bool foundMass = false;
       if( m_nTrkMassMin.size() == 0 ) foundMass = true;
       for(size_t im = 0; im < m_nTrkMassMin.size(); ++im)
	 if( totalMass >=m_nTrkMassMin[im] &&  totalMass <= m_nTrkMassMax[im])
	   { foundMass = true;   break; }
       if( !foundMass ){
	 if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " combination Mass Failed " << endmsg;
	 continue; // loop over Ntrk combinations
       }else{
	 if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " combination Mass Accepted " << endmsg;
       }
       passNTrkMass =true;
     }

       //== calculate mass of pairs if requested ================
       
       std::vector<int> index_0;
       std::vector<int> index_1;
       std::vector<double> dimasses;
       
       int npair = 0; 
       int npairAcc = 0;

       if( m_diTrkMassMin.size() > 0 ){
	 bool foundPair = false;
	 for( int itrk0 = 0; itrk0 < m_nTrk && !foundPair; ++itrk0){
	   ElementLink<xAOD::TrackParticleContainer> trk0 = thisIterationTracks[itrk0];
	   
	   for( int itrk1 = itrk0+1; itrk1 < m_nTrk && !foundPair; ++itrk1) {
	     ElementLink<xAOD::TrackParticleContainer> trk1 = thisIterationTracks[itrk1];
	     npair++;
	     if( m_diTrkQ >= 0 ){	       
	       int charge = 0;
	       charge += ( (*trk0)->charge() > 0 ) ? 1 : -1 ;
	       charge += ( (*trk1)->charge() > 0 ) ? 1 : -1 ;
	       if( abs(charge) != m_diTrkQ ){
		 if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "| Charge| = "<< abs(charge) 
						  << " required "<<m_diTrkQ << " thus fail" << endmsg;
		 continue;
	       }else{
		 if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " |Charge| = "<< abs(charge) 
						  << " required "<<m_diTrkQ << " thus pass" << endmsg;
	       }
	     }
	     passPairCharge = true;
	     
	     // simple mass
	     double dimass = m_bphysHelperTool->invariantMass(*trk0,*trk1,masses[0], masses[1]);
	     m_mon_pairMass.push_back(dimass);

	     if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pair pt1=" << (*trk0)->pt() << ", pt2="<< (*trk1)->pt()
					      <<", mass = "<<  dimass << endmsg;
	     // check di-mass now
	     for(size_t im = 0; im < m_diTrkMassMin.size(); ++im) 
	       if( dimass >=m_diTrkMassMin[im] &&  dimass <= m_diTrkMassMax[im])
		 { 

		   foundPair = true;
		   index_0.push_back(itrk0);
		   index_1.push_back(itrk1);
		   dimasses.push_back(dimass);	
		 }
	   }} // end loop over track pairs
	 
	 npairAcc += dimasses.size();

	 if( !foundPair ) {
	   if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " di-Mass Failed " << endmsg;
	   continue; // 
	 }else{
	   if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " di-Mass cut succeeded " << endmsg;
	 }
	 passPairMass = true;
       }
    

       m_mon_NPair += npair;
       m_mon_acceptedNPair += npairAcc;

       //  first add big object and then di-masses
       xAOD::TrigBphys* xaodobj = new xAOD::TrigBphys;
       xAODTrigBphysColl->push_back(xaodobj);
       xaodobj->initialise(0, 0., 0.,0., xAOD::TrigBphys::MULTIMU, totalMass,xAOD::TrigBphys::EF);
       if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Adding TrigBphys xAOD obj with mass "<< totalMass << endmsg;
       
       std::vector<double> masses1(thisIterationTracks.size(), m_trkMass);
       if (m_bphysHelperTool->vertexFit(xaodobj,thisIterationTracks,masses1).isFailure()) {
	 delete xaodobj; xaodobj = nullptr;
	 if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Problems with vertex fit in TrigMultiTrkFex"  << endmsg;
	 continue;
       }else{
	 // check chi2 
	 if( m_nTrkVertexChi2 >= 0 ){
	   float chi2 = xaodobj->fitchi2();
	   if( chi2 > m_nTrkVertexChi2 ){  
	     if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " nTrk Vertex chi2  "<< chi2
					      << " required "<<m_nTrkVertexChi2 << " thus fail" << endmsg;
	     //delete created object
	     //xAODTrigBphysColl->pop_back() ;
	     //delete xaodobj; xaodobj = nullptr;
	     continue;
	   }else{
	     if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " nTrk Vertex chi2  "<< chi2
					      << " required "<<m_nTrkVertexChi2 << " is good" << endmsg;
	   }
	 }	
       } // end of vertexFit.isFailure

       passNTrkVertexChi2 = true;

    
       m_mon_NTrkFitMass.push_back(xaodobj->mass()*0.001 );
       m_mon_NTrkChi2.push_back(xaodobj->fitchi2() );
       //m_bphysHelperTool->fillTrigObjectKinematics(xaodobj,{mutrk,trk});

        //std::cout << std::endl;


    m_mon_accepted_highptNTrk =  highptTracks.size() ;

    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
  //------------- end of combination loop

    if(  passHighPtTrack )   m_mon_Acceptance.push_back( ACCEPT_HighPtTrack );
    if(  passNTrkCharge  )   m_mon_Acceptance.push_back( ACCEPT_NTrkCharge  );
    if(  passNTrkVertexChi2)   m_mon_Acceptance.push_back( ACCEPT_NTrkVertexChi2  );
    if(  passNTrkMass    )   m_mon_Acceptance.push_back( ACCEPT_NTrkMass    );
    if(  passPairCharge  )   m_mon_Acceptance.push_back( ACCEPT_PairCharge  );
    if(  passPairMass    )   m_mon_Acceptance.push_back( ACCEPT_PairMass    );


    // record collection now
    if ( timerSvc() )  m_BmmHypTot->stop();
    
    HLT::TriggerElement* outputTE = addRoI(output);  

    if (xAODTrigBphysColl && xAODTrigBphysColl->size()) {
        if ( msgLvl() <= MSG::DEBUG ) 
	  msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " 
		 << xAODTrigBphysColl->size() << endmsg;
        
	// OI do we always need to create outputTE? or only when there is something to write ?
	outputTE->setActiveState(true);

        HLT::ErrorCode sc = attachFeature(outputTE, xAODTrigBphysColl, m_bphysCollectionKey );
        if(sc != HLT::OK) {
            msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endmsg;
            m_mon_Errors.push_back( ERROR_BphysColl_Fails );
            delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr; // assume deletion responsibility
	    afterExecMonitors().ignore();   
            return HLT::ERROR;
        }
	m_mon_Acceptance.push_back( ACCEPT_RecordedCollection );
	m_countPassedCombination += xAODTrigBphysColl->size();
    } else {
        if ( msgLvl() <= MSG::DEBUG ) 
	  msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endmsg;
        delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr;
    }

    m_countPassedEvents++;


 // stop monitoring
    afterExecMonitors().ignore();   
    return HLT::OK;
}




