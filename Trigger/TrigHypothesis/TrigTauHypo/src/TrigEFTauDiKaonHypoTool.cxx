/*                                                                                                                                                                                                          
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                                                                   
*/

/********************************************************************
 *
 * NAME:     TrigEFTauDiKaonHypoTool.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   J. Silva based on TrigEFTauMVHypoTool
 * CREATED:  Feb. 17, 2021
 *
  *********************************************************************/

#include <list>
#include <iterator>
#include <sstream>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <PathResolver/PathResolver.h>

#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigEFTauDiKaonHypoTool.h"

using namespace TrigCompositeUtils;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFTauDiKaonHypoTool::TrigEFTauDiKaonHypoTool( const std::string& type,
					  const std::string& name,
					  const IInterface* parent )
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) )
{
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFTauDiKaonHypoTool::~TrigEFTauDiKaonHypoTool()
{
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
StatusCode TrigEFTauDiKaonHypoTool::initialize()
  // ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: EFTauDiKaonHypo will cut on "<<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysMin " << m_massTrkSysMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysMax " << m_massTrkSysMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonMin " << m_massTrkSysKaonMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonMax " << m_massTrkSysKaonMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonPiMin " << m_massTrkSysKaonPiMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonPiMax " << m_massTrkSysKaonPiMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param targetMassTrkSysKaonPi " << m_targetMassTrkSysKaonPi <<endmsg;
  msg() << MSG::INFO << " REGTEST: param leadTrkPtMin " << m_leadTrkPtMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param EtCalibMin " << m_EtCalibMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param nTrackMin (included) " << m_nTrackMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param nTrackMax (included) " << m_nTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param nWideTrackMax (included) " << m_nWideTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param EMPOverTrkSysPMax " << m_EMPOverTrkSysPMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param dRmaxMax " << m_dRmaxMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param etOverPtLeadTrkMin " << m_etOverPtLeadTrkMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param etOverPtLeadTrkMax " << m_etOverPtLeadTrkMax <<endmsg;

  msg() << MSG::INFO << " REGTEST: ------ "<<endmsg;

  
  if( ( m_massTrkSysKaonPiMin >  m_massTrkSysKaonPiMax ) ||  ( m_massTrkSysKaonMin >  m_massTrkSysKaonMax ) || ( m_massTrkSysMin >  m_massTrkSysMax ) || ( m_nTrackMin > m_nTrackMax )  || (m_etOverPtLeadTrkMin > m_etOverPtLeadTrkMax) )
    {
      msg() << MSG::ERROR << "EFTauDiKaonHypo is uninitialized! " << endmsg;
      return StatusCode::FAILURE;
    }
  
  msg() << MSG::INFO
	<< "Initialization of EFTauDiKaonHypo completed successfully"
	<< endmsg;
  
  return StatusCode::SUCCESS;
}



bool TrigEFTauDiKaonHypoTool::decide(const ITrigEFTauMVHypoTool::TauJetInfo& input ) const
{

  ATH_MSG_DEBUG("REGTEST:"<< name() << ": in execute()" );
  bool pass=false;

  using namespace Monitored;

  auto PassedCuts         = Monitored::Scalar<int>( "CutCounter", 0 );
  auto massTrkSysAccepted = Monitored::Scalar<float>("massTrkSysAccepted", -10);
  auto massTrkSysKaonAccepted     = Monitored::Scalar<float>( "massTrkSysKaonAccepted", -10);
  auto massTrkSysKaonPiAccepted = Monitored::Scalar<float>( "massTrkSysKaonPiAccepted", -10);
  auto leadTrkPtAccepted         = Monitored::Scalar<float>( "leadTrkPtAccepted", -10);
  auto ptAccepted          = Monitored::Scalar<float>( "ptAccepted", -10);
  auto nTrackAccepted          = Monitored::Scalar<float>( "nTrackAccepted", -1);
  auto nWideTrackAccepted          = Monitored::Scalar<float>( "nWideTrackAccepted", -1);
  auto dRAccepted          = Monitored::Scalar<float>( "dRAccepted", -1);
  auto etOverPtLeadTrkAccepted          = Monitored::Scalar<float>( "etOverPtLeadTrkAccepted", -10);
  auto EMOverTrkSysPAccepted          = Monitored::Scalar<float>( "EMOverTrkSysPAccepted", -10);
  auto ninputTaus         = Monitored::Scalar<int>( "nInputTaus", -1);
  auto monitorIt    = Monitored::Group(m_monTool, PassedCuts, massTrkSysAccepted, massTrkSysKaonAccepted, massTrkSysKaonPiAccepted, leadTrkPtAccepted, ptAccepted, nTrackAccepted, nWideTrackAccepted , dRAccepted, etOverPtLeadTrkAccepted, EMOverTrkSysPAccepted, ninputTaus);


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

  
  // get tau objects from the trigger element:
  //--------------------------------------------------
  
  auto TauContainer = input.taujetcontainer;
  ninputTaus = TauContainer->size();
  
  for(auto Tau: *TauContainer){  
    
    ATH_MSG_DEBUG( " tauRec candidate ");
    PassedCuts++;    
    
    // cut on calibrated pt:

    double EFet = Tau->pt()*1e-3; //Convert to GeV
    ATH_MSG_DEBUG( " REGTEST: Et Calib "<<EFet);
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    PassedCuts++;
    ptAccepted = EFet;
    
    int numTrack     = -100;
    int numWideTrack = -100;
    // cut on core and wide tracks:
    numTrack = Tau->nTracks();
    #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    numWideTrack = Tau->nWideTracks();
    #else
    numWideTrack = Tau->nTracksIsolation();
    #endif

    ATH_MSG_DEBUG( " REGTEST: Track size "<<numTrack );
    ATH_MSG_DEBUG( " REGTEST: Wide Track size "<<numWideTrack );
 
    if (!( (numTrack >= m_nTrackMin) && (numTrack <= m_nTrackMax)))  continue;
    if (!( numWideTrack <= m_nWideTrackMax ))  continue;
    PassedCuts++;
    nTrackAccepted = numTrack;
    nWideTrackAccepted = numWideTrack;

    // cut on leading track pt:     
    float leadTrkPt = -1.;
    Tau->detail(xAOD::TauJetParameters::leadTrkPt, leadTrkPt);
    ATH_MSG_DEBUG( " REGTEST: leadTrkPt "<< leadTrkPt);
    if(!( leadTrkPt > m_leadTrkPtMin)) continue;
    PassedCuts++;
    leadTrkPtAccepted = leadTrkPt*1e-3; //Convert to GeV

    // cut on massTrkSys:     
    float massTrkSys = -1.;
    Tau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys);

    ATH_MSG_DEBUG(" REGTEST: massTrkSys "<< massTrkSys );
    // for dikaon mass hypothesis, compute invariant mass with kaon mass
    TLorentzVector my_kaons(0.,0.,0.,0.);
    std::vector<TLorentzVector> my_trks;
    // need to add checks for valid link
    for (unsigned int i=0;i<Tau->nTracks();++i) {
      const xAOD::TrackParticle* trk = 0;
      TLorentzVector tmpKaon(0.,0.,0.,0.);

      try 
      {
        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
        trk = Tau->track(i);
	#else
	trk = Tau->track(i)->track();
	#endif
      }
      catch(const std::exception& e)
      {
        msg() << MSG::WARNING << " REGTEST: EFTauDiKaonHypo, failed to get tau track link! " <<endmsg;
      } 

      if(trk) {
         tmpKaon.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), 493.677);
         my_trks.push_back(trk->p4());
      }
      my_kaons = my_kaons + tmpKaon;
    }
    float massTrkSysKaon = my_kaons.M();
    ATH_MSG_DEBUG( " REGTEST: massTrkSys with kaon mass hypo "<< massTrkSysKaon );
    // kaon+pi mass hypo
    double finalKPiMass = 0;
    if(my_trks.size()==2){

          TLorentzVector tmpKaon;
          tmpKaon.SetPtEtaPhiM(my_trks.at(0).Pt(), my_trks.at(0).Eta(), my_trks.at(0).Phi(), 493.677);
          TLorentzVector tmpPion = my_trks.at(1);

          double kPiMass1 = (tmpKaon+tmpPion).M();

          tmpKaon.SetPtEtaPhiM(my_trks.at(1).Pt(), my_trks.at(1).Eta(), my_trks.at(1).Phi(), 493.677);
          tmpPion = my_trks.at(0);

          double kPiMass2 = (tmpKaon+tmpPion).M();

          if(std::abs(kPiMass1 - m_targetMassTrkSysKaonPi) < std::abs(kPiMass2 - m_targetMassTrkSysKaonPi))
          {
            finalKPiMass = kPiMass1;
          }else{
            finalKPiMass = kPiMass2;
          }
    }
    float massTrkSysKaonPi = finalKPiMass;
    ATH_MSG_DEBUG( " REGTEST: massTrkSys with kaon+pi mass hypo "<< massTrkSysKaonPi );
    if (!( (massTrkSys > m_massTrkSysMin) && (massTrkSys < m_massTrkSysMax) ) )  continue;
    PassedCuts++;
    massTrkSysAccepted = massTrkSys*1e-3; //Convert to GeV

    if (!( (massTrkSysKaon > m_massTrkSysKaonMin) && (massTrkSysKaon < m_massTrkSysKaonMax) ) )  continue;
    PassedCuts++;   
    massTrkSysKaonAccepted = massTrkSysKaon*1e-3; //Convert to GeV

    if (!( (massTrkSysKaonPi >= m_massTrkSysKaonPiMin) && (massTrkSysKaonPi < m_massTrkSysKaonPiMax) ) )  continue; //use >= otherwise singlepion chain would fail here!
    PassedCuts++;
    massTrkSysKaonPiAccepted = massTrkSysKaonPi*1e-3; //Convert to GeV

    // cut on EMPOverTrkSysP:     
    float EMPOverTrkSysP = -1.;
    Tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP);
    ATH_MSG_DEBUG( " REGTEST: EMPOverTrkSysP "<< EMPOverTrkSysP);               
    if ( !(EMPOverTrkSysP < m_EMPOverTrkSysPMax) )  continue;
    PassedCuts++;
    EMOverTrkSysPAccepted = EMPOverTrkSysP;

    // cut on etOverPtLeadTrk:
    float etOverPtLeadTrk = -1.;
    Tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etOverPtLeadTrk);
    ATH_MSG_DEBUG( " REGTEST: etOverPtLeadTrk "<< etOverPtLeadTrk);
    if( !( (etOverPtLeadTrk > m_etOverPtLeadTrkMin) && (etOverPtLeadTrk < m_etOverPtLeadTrkMax)  ) ) continue;
    PassedCuts++;
    etOverPtLeadTrkAccepted = etOverPtLeadTrk;

    // cut on dRmax:
    float dRmax = -1.;
    Tau->detail(xAOD::TauJetParameters::dRmax, dRmax);

    ATH_MSG_DEBUG( " REGTEST: dRmax "<< dRmax);
    if( !( dRmax < m_dRmaxMax ) ) continue;
    PassedCuts++;    
    dRAccepted = dRmax; 


    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    ATH_MSG_DEBUG(" REGTEST: pass taurec is "<<pass);
  } // end of loop in    int numWideTrack = -100; tau objects.
  
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

StatusCode TrigEFTauDiKaonHypoTool::decide(  std::vector<TauJetInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
	addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
