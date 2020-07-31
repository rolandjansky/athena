/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "xAODTau/TauJetContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "AthenaMonitoringKernel/Monitored.h"

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
}

StatusCode TrigEFTauMVHypoTool::initialize()
{
  
  ATH_MSG_INFO( "in initialize()" );
  
  ATH_MSG_INFO( " REGTEST: TrigEFTauMVHypoTool will cut on ");
  ATH_MSG_INFO( " REGTEST: param NTrackMin " << m_numTrackMin );
  ATH_MSG_INFO( " REGTEST: param NTrackMax " << m_numTrackMax );
  ATH_MSG_INFO( " REGTEST: param NWideTrackMax " << m_numWideTrackMax );
  ATH_MSG_INFO( " REGTEST: param EtCalib " << m_EtCalibMin );
  ATH_MSG_INFO( " REGTEST: param Level " << m_level );
  ATH_MSG_INFO( " REGTEST: param Method " << m_method );
  ATH_MSG_INFO( " REGTEST: param Highpt with thrs " << m_highpt << " " << m_highpttrkthr <<  " " << m_highptidthr << " " << m_highptjetthr );
  ATH_MSG_INFO( " REGTEST: param ApplyIDon0p " << m_applyIDon0p );
  ATH_MSG_INFO( " REGTEST: ------ ");

  if( (m_numTrackMin >  m_numTrackMax) || m_level == -1 || (m_highptidthr > m_highptjetthr))
    {
      ATH_MSG_ERROR( "TrigEFTauMVHypoTool is uninitialized! " );
      return StatusCode::FAILURE;
    }
  
  // Likelihood ID no longer supported
  if( m_method == 1)
    {
      ATH_MSG_ERROR( "Likelihood identification discontinued!" );
      return StatusCode::FAILURE;
    }
  else if( m_method == 0 && m_level != -1111)
    {
      ATH_MSG_ERROR( "Incorrect combination of Method and Level." );
      return StatusCode::FAILURE;
    }else if((m_level<0 && m_level!=-1111) || m_level>3 )
    {
      ATH_MSG_ERROR( "Incorrect Level value provided.");
      return StatusCode::FAILURE;
   }

  ATH_MSG_INFO( "Initialization of EFTauMVHypo completed successfully." );

  return StatusCode::SUCCESS;
}


bool TrigEFTauMVHypoTool::decide(const ITrigEFTauMVHypoTool::TauJetInfo& input ) const
{

  ATH_MSG_DEBUG("REGTEST:"<< name() << ": in execute()" );
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

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    pass = false;
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }

  //get RoI descriptor
  auto roiDescriptor = input.roi;
  float roIZ   = roiDescriptor->zed();
  float roIEta = roiDescriptor->eta();
  float roIPhi = roiDescriptor->phi();

  ATH_MSG_DEBUG( "Input RoI eta: " << roIEta << " Input RoI phi: " << roIPhi << " Input RoI z: " << roIZ);

  auto TauContainer = input.taujetcontainer;
  ninputTaus = TauContainer->size();
 
  for(auto Tau: *TauContainer){
    
    ATH_MSG_DEBUG( " tauRec candidate ");
    
    PassedCuts++;
    
    double EFet = Tau->pt()*1e-3;

    ATH_MSG_DEBUG( " REGTEST: Et Calib "<<EFet);
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    PassedCuts++;
    ptAccepted = EFet;

    int numTrack     = -100;
    int numWideTrack = -100;

    numTrack = Tau->nTracks();

    numWideTrack = Tau->nTracksIsolation();

    
    ATH_MSG_DEBUG( " REGTEST: Track size "<<numTrack );	
    ATH_MSG_DEBUG( " REGTEST: Wide Track size "<<numWideTrack );

    // turn off track selection at highpt
    bool applyTrkSel(true);
    bool applyMaxTrkSel(true);
    if(m_highpt && (EFet > m_highpttrkthr*1e-3) ) applyTrkSel = false;
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) applyMaxTrkSel = false;

    if(applyMaxTrkSel) if( !(numTrack <= m_numTrackMax) ) continue;
    if(applyTrkSel)    if( !(numTrack >= m_numTrackMin) ) continue;
    if(applyTrkSel)    if( !(numWideTrack <= m_numWideTrackMax)  ) continue;
   
    PassedCuts++;
    nTrackAccepted = numTrack;
    nWideTrackAccepted = numWideTrack;  

    auto local_level = m_level;
    //loosen and turn off ID cut at highpt
    if(m_highpt && (EFet > m_highptidthr*1e-3) && m_level>1) local_level = 1; //works only for BDT, not llh
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) local_level = -1111;
    if(!m_applyIDon0p && numTrack==0) local_level = -1111;

    ATH_MSG_DEBUG( " REGTEST: Local level " << local_level );
 
    //No tau ID
    if(m_method == 0)
      {
	  pass = true;
	  PassedCuts++;
	  break;
	}
    else if(m_method == 2)
      {
	double bdtscore = Tau->discriminant(xAOD::TauJetParameters::BDTJetScore);
	
	ATH_MSG_DEBUG( " REGTEST: BDTScore "<<bdtscore);
	
	if(local_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    PassedCuts++;
	    break;
	  }
	
	if (local_level == 1 && Tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) == 0)
	  continue;
	else if (local_level == 2 && Tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium) == 0)
	  continue;
	else if (local_level == 3  && Tau->isTau(xAOD::TauJetParameters::JetBDTSigTight) == 0)
	  continue;
	
	PassedCuts++;
      }    // RNN
    else if(m_method == 3)
      {
   if(!Tau->hasDiscriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans))
     ATH_MSG_WARNING( "RNNJetScoreSigTrans not available. Make sure TauWPDecorator is run for RNN!" );
    
   ATH_MSG_DEBUG( "REGTEST: RNNJetScoreSigTrans "<< Tau->discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans) );
    
   if(local_level == -1111)
     { //noCut, accept this TE
       pass = true;
       PassedCuts++;
       break;
     }
   if (local_level == 0 && Tau->isTau(xAOD::TauJetParameters::JetRNNSigVeryLoose) == 0)  
     continue;
   else if (local_level == 1 && Tau->isTau(xAOD::TauJetParameters::JetRNNSigLoose) == 0)
     continue;
   else if (local_level == 2 && Tau->isTau(xAOD::TauJetParameters::JetRNNSigMedium) == 0)
     continue;
   else if (local_level == 3  && Tau->isTau(xAOD::TauJetParameters::JetRNNSigTight) == 0)
     continue;
   
   PassedCuts++;
      }
    else
      {
	ATH_MSG_ERROR( " no valid method defined ");	
	continue;
      }
    
    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    ATH_MSG_DEBUG( " REGTEST: pass taurec is "<<pass);
    
  } // end of loop in tau objects.
  
  if(pass)
    {
	   ATH_MSG_DEBUG( " REGTEST: TE accepted !! ");
      // activate Trigger Element.
    }
  else
    {
	   ATH_MSG_DEBUG( " REGTEST: No good tau found !! TE rejected ");
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

