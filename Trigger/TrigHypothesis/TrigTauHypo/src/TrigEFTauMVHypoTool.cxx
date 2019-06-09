/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"

#include "TFile.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <PathResolver/PathResolver.h>

#include "AthenaMonitoring/Monitored.h"

#include "TrigEFTauMVHypoTool.h"

using namespace TrigCompositeUtils;

TrigEFTauMVHypoTool::TrigEFTauMVHypoTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) )
{
}

TrigEFTauMVHypoTool::~TrigEFTauMVHypoTool()
{  
  //delete m_OneProngGraph;
  //delete m_MultiProngGraph;
}

StatusCode TrigEFTauMVHypoTool::initialize()
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: TrigEFTauMVHypoTool will cut on "<<endmsg;
  msg() << MSG::INFO << " REGTEST: param NTrackMin " << m_numTrackMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param NTrackMax " << m_numTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param NWideTrackMax " << m_numWideTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param EtCalib " << m_EtCalibMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param Level " << m_level <<endmsg;
  msg() << MSG::INFO << " REGTEST: param Method " << m_method <<endmsg;
  msg() << MSG::INFO << " REGTEST: param Highpt with thrs " << m_highpt << " " << m_highpttrkthr <<  " " << m_highptidthr << " " << m_highptjetthr <<endmsg;
  msg() << MSG::INFO << " REGTEST: param ApplyIDon0p " << m_applyIDon0p <<endmsg;
  msg() << MSG::INFO << " REGTEST: ------ "<<endmsg;
  
  if( (m_numTrackMin >  m_numTrackMax) || m_level == -1 || (m_highptidthr > m_highptjetthr))
    {
      msg() << MSG::ERROR << "TrigEFTauMVHypoTool is uninitialized! " << endmsg;
      return StatusCode::FAILURE;
    }
  
  std::string s_llh_cuts_file = PathResolverFindCalibFile("TrigTauRec/00-11-01/LMTCutsLLHTrigger.root");  
  msg() << MSG::DEBUG << "Try to open root file containing cuts: " << s_llh_cuts_file << endmsg;
 
  TFile* llhcuts = TFile::Open(s_llh_cuts_file.c_str());

  if(!llhcuts)
    {
      msg() << MSG::ERROR << "Could not find file containing cut values. TrigEFTauMVHypoTool is uninitialized! " << endmsg;
      return StatusCode::FAILURE;
    }

  if (m_level == 1)
    {
      m_cut_level      = "medium";
      m_OneProngGraph    = std::unique_ptr<TGraph>{static_cast<TGraph*>((llhcuts->Get("1prong/medium"))->Clone())};
      m_MultiProngGraph  = std::unique_ptr<TGraph>{static_cast<TGraph*>((llhcuts->Get("3prong/medium"))->Clone())};
    }
  else if (m_level == 2)
    {
      m_cut_level      = "medium1";
      m_OneProngGraph    = std::unique_ptr<TGraph>{static_cast<TGraph*>((llhcuts->Get("1prong/medium1"))->Clone())};
      m_MultiProngGraph  = std::unique_ptr<TGraph>{static_cast<TGraph*>((llhcuts->Get("3prong/medium1"))->Clone())};

    }
  else if (m_level == 3)
    {
      m_cut_level      = "tight";
      m_OneProngGraph    = std::unique_ptr<TGraph>{static_cast<TGraph*>((llhcuts->Get("1prong/tight"))->Clone())};
      m_MultiProngGraph  = std::unique_ptr<TGraph>{static_cast<TGraph*>((llhcuts->Get("3prong/tight"))->Clone())};
    }
  else if (m_level == -1111)
    {
      m_cut_level = "none";
    }
  else
    {
      msg() << MSG::ERROR << "Did not configure valid level. TrigEFTauMVHypoTool is uninitialized! " << endmsg;
      return StatusCode::FAILURE;
    }
  
  msg() << MSG::INFO
	<< "Initialization of TrigEFTauMVHypoTool completed successfully"
	<< endmsg;
  
  if (llhcuts) 
    {
      llhcuts->Close();
      delete llhcuts;
    }
  return StatusCode::SUCCESS;
}


bool TrigEFTauMVHypoTool::decide(const ITrigEFTauMVHypoTool::TauJetInfo& input ) const
{

  if(msgLvl(MSG::DEBUG))  msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in execute()" << endmsg;
  // general reset
  bool pass=false;

  using namespace Monitored;

  auto PassedCuts         = Monitored::Scalar<int>( "CutCounter", -1 );
  auto ptAccepted         = Monitored::Scalar<double>( "ptAccepted", -1);
  auto nTrackAccepted	  = Monitored::Scalar<int>( "nTrackAccepted", -1);
  auto nWideTrackAccepted = Monitored::Scalar<int>( "nWideTrackAccepted", -1);
  auto ninputTaus         = Monitored::Scalar<int>( "nInputTaus", -1);

  // general reset
  PassedCuts = 0;

  //get RoI descriptor
  auto roiDescriptor = input.roi;
  float roIEta = roiDescriptor->eta();
  float roIPhi = roiDescriptor->phi();

  ATH_MSG_DEBUG( "Input RoI eta: " << roIEta << " Input RoI phi: " << roIPhi );
  
  
  /*const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(outputTE, roiDescriptor);
  
  if ( roiDescriptor ) 
    {
      if( msgLvl(MSG::DEBUG) ) 
	msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
	      << " located at   phi = " <<  roiDescriptor->phi()
	      << ", eta = " << roiDescriptor->eta() << endmsg;
    } 
  else 
    {
      if( msgLvl(MSG::DEBUG) ) 
	msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }*/
  
  // get tau objects from the trigger element:
  //--------------------------------------------------
  
  //std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
  /*
  status = getFeatures(outputTE, vectorTauContainers);
 
  if(status!=HLT::OK) 
    {
      msg() << MSG::INFO
	    << " REGTEST: Failed to get tauContainer's from the trigger element" 
	    << endmsg;
      return HLT::OK;
    } 
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got " << vectorTauContainers.size() 
	  << " tauContainers's associated to the TE " << endmsg;
  
  if(vectorTauContainers.size() == 0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }
  */
  //const xAOD::TauJetContainer *TauContainer = vectorTauContainers.back();
  
  //msg() << MSG::DEBUG << " REGTEST: number of tau in container "<< TauContainer->size() << endmsg;

  auto TauContainer = input.taujetcontainer;
  ninputTaus = TauContainer->size();
 
  for(auto Tau: *TauContainer){
    
    if( msgLvl(MSG::DEBUG) )
      msg() << MSG::DEBUG << " tauRec candidate "<<endmsg;
    
    PassedCuts++;
    
    double EFet = Tau->pt()*1e-3;

    if( msgLvl(MSG::DEBUG) )
      msg() << MSG::DEBUG << " REGTEST: Et Calib "<<EFet<<endmsg;
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    PassedCuts++;
    ptAccepted = EFet;

    int numTrack     = -100;
    int numWideTrack = -100;

    numTrack = Tau->nTracks();

    //#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    //numWideTrack = (*tauIt)->nTracksIsolation();
    //#else
    numWideTrack = Tau->nTracksIsolation();
    //#endif

    
    if( msgLvl(MSG::DEBUG) ){
      msg() << MSG::DEBUG << " REGTEST: Track size "<<numTrack <<endmsg;	
      msg() << MSG::DEBUG << " REGTEST: Wide Track size "<<numWideTrack <<endmsg;
    }    

    // turn off track selection at highpt
    bool applyTrkSel(true);
    bool applyMaxTrkSel(true);
    if(m_highpt && (EFet > m_highpttrkthr*1e-3) ) applyTrkSel = false;
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) applyMaxTrkSel = false;

    if(applyMaxTrkSel) if( !(numTrack <= m_numTrackMax) ) continue;
    if(applyTrkSel)    if( !(numTrack >= m_numTrackMin) ) continue;
    if(applyTrkSel)    if( !(m_numWideTrack <= m_numWideTrackMax)  ) continue;
   
    PassedCuts++;
    nTrackAccepted = numTrack;
    nWideTrackAccepted = m_numWideTrack;  

    auto local_level = m_level;
    //loosen and turn off ID cut at highpt
    if(m_highpt && (EFet > m_highptidthr*1e-3) && m_level>1) local_level = 1; //works only for BDT, not llh
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) local_level = -1111;
    if(!m_applyIDon0p && numTrack==0) local_level = -1111;
 
    if(m_method == 1 || m_method == 0)
      {
	double llh_cut = 11111.;     
	std::string prong;
	numTrack==1 ?  prong = "1P" : prong = "3P";
	
	if(local_level == -1111){ //noCut, accept this TE
	  pass = true;
	  PassedCuts++;
	  continue;
	}
	
	std::string s_cut_llh = "llh_" + m_cut_level +"_" + prong;
	msg() << MSG::DEBUG << "REGTEST: cut string: " << s_cut_llh << endmsg;
	
	if(numTrack == 1)
	  {
	    llh_cut=m_OneProngGraph->Eval(EFet);
	  }
	else
	  {
	    llh_cut=m_MultiProngGraph->Eval(EFet);	  
	  }
	
	msg() << MSG::DEBUG<<" REGTEST: will cut on llh score at "<< llh_cut <<endmsg;
	double llhscore = Tau->discriminant(xAOD::TauJetParameters::Likelihood);

   //Override this vale for now to get some events passing the hypo
   llhscore = 0.5;
	
	msg() << MSG::DEBUG<<" REGTEST: LLHScore "<<llhscore<<endmsg;
	
	if (llhscore < llh_cut) continue;
	
	PassedCuts++;
      }
    
    else if(m_method == 2 || m_method == 0)
      {
	double bdtscore = Tau->discriminant(xAOD::TauJetParameters::BDTJetScore);
	
	msg() << MSG::DEBUG<<" REGTEST: BDTScore "<<bdtscore<<endmsg;
	
	if(local_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    PassedCuts++;
	    continue;
	  }
	
	if (local_level == 1 && Tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) == 0)
	  continue;
	else if (local_level == 2 && Tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium) == 0)
	  continue;
	else if (local_level == 3  && Tau->isTau(xAOD::TauJetParameters::JetBDTSigTight) == 0)
	  continue;
	
	PassedCuts++;
      }
    else
      {
	msg() << MSG::ERROR << " no valid method defined "<<endmsg;	
	continue;
      }
    
    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    if( msgLvl(MSG::DEBUG) )
      msg() << MSG::DEBUG << " REGTEST: pass taurec is "<<pass<<endmsg;
    
  } // end of loop in tau objects.
  
  if(pass)
    {
      if( msgLvl(MSG::DEBUG) )
	msg() << MSG::DEBUG
	      << " REGTEST: TE accepted !! "
	      << endmsg;
      // activate Trigger Element.
    }
  else
    {
      if( msgLvl(MSG::DEBUG) )
	msg() << MSG::DEBUG
	      << " REGTEST: No good tau found !! TE rejected "
	      << endmsg;
    }
  
  return pass;
  
}

StatusCode TrigEFTauMVHypoTool::decide(  std::vector<TauJetInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
   addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

