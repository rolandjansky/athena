/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_TrackAnalysisCXX
#define MuonCalib_TrackAnalysisCXX

#include "MuonCalibStandAloneExtraTools/MdtDqaTrackAnalysis.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"
#include "TH1.h"
#include "TProfile.h"

using namespace std;

namespace MuonCalib {

MdtDqaTrackAnalysis::MdtDqaTrackAnalysis(RegionSelectionSvc *punt, int trkauthor, HistogramManager *histoman, bool verbose) {
  m_TrkAuthor=trkauthor;
  m_histoManager=histoman;
  p_reg_sel_svc=punt;
  m_verbose=verbose;
  //  if(m_TrkAuthor==0) cout<<"tracking with MuID algorithm"<<endl;
  //  if(m_TrkAuthor==100) cout<<"tracking with STACO algorithm"<<endl;
}

void MdtDqaTrackAnalysis::handleEvent(const MuonCalibEvent &event,  int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position) {

  // SERVE QUESTO ????
  //  if( !p_reg_sel_svc){
  //  std::cout << "Region Selection Service NOT INITIALIZED " << std::endl;
  //  return;
  //  }
 
  string histoType;

  const MuonCalibEvent_E& extra_event(dynamic_cast<const MuonCalibEvent_E&>(event));

  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::::::::: ... preparing histos ::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  histoType="mbts1_time"; TH1F *h1tmbts1 = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="mbts2_time"; TH1F *h1tmbts2 = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="mbts1VSmbts2"; TH2F *h2mbts1VSmbts2 = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="mbts_Dtime"; TH1F *h1Dtmbts = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);

  histoType="trk_ID_mult"; TH1F *h1trkIDmult = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);

  histoType="seg_nb"; TH1F *h1segments = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_nb"; TH1F *h1tracks = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_nb_sel"; TH1F *h1tracks_sel = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_authorMoore"; TH1F *h1author1 = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_authorMuBoy"; TH1F *h1author2 = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_pt"; TH1F *h1pt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_eta"; TH1F *h1eta = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_phi"; TH1F *h1phi = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_etaVSphi"; TH2F *h2etaVSphi = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_d0VSz0"; TH2F *h2d0VSz0 = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_yVSx"; TH2F *h2yVSx = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_qOverP"; TH1F *h1qoverp = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_chi2dof"; TH1F *h1chi2dof = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_chi2dof_VS_author"; TH2F *h2chi2dof_VS_author = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_chi2dof_VS_eta"; TH2F *h2chi2dof_VS_eta = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_d0"; TH1F *h1d0 = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_z0ip"; TH1F *h1z0ip = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_mdthits_VS_eta"; TH2F *h2mdthits_VS_eta = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_mdthits_VS_phi"; TH2F *h2mdthits_VS_phi = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="hit_mdt"; TH1F *h1mdt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);   
  histoType="hit_rpc"; TH1F *h1rpc = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);   
  histoType="hit_mdtVSrpc"; TH2F *h2mdtVSrpc = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);   
  histoType="hit_tgc"; TH1F *h1tgc = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);   
  histoType="hit_csc"; TH1F *h1csc = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);   
  histoType="hit_driftR"; TH1F *h1driftR = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);   
  histoType="hit_pull"; TH1F *h1pull = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType); 

  histoType="trk_Ev_pveto"; TH1F *h1_Ev_pveto = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Ev_trkveto"; TH1F *h1_Ev_trkveto = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_p"; TH1F *h1_ID_p = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_ntrack"; TH1F *h1_ID_ntrack = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_npixel"; TH1F *h1_ID_npixel = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_nsct"; TH1F *h1_ID_nsct = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_ntrt"; TH1F *h1_ID_ntrt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_pixsctVStrt"; TH2F *h2_ID_pixsctVStrt = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_ID_pixVSsct"; TH2F *h2_ID_pixVSsct = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Tag_ntrack"; TH1F *h1_Tag_ntrack = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Tag_npixel"; TH1F *h1_Tag_npixel = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Tag_nsct"; TH1F *h1_Tag_nsct = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Tag_ntrt"; TH1F *h1_Tag_ntrt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Tag_pixsctVStrt"; TH2F *h2_Tag_pixsctVStrt = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_Tag_pixVSsct"; TH2F *h2_Tag_pixVSsct = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_SA_ntrack"; TH1F *h1_SA_ntrack = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_SAIP_ntrack"; TH1F *h1_SAIP_ntrack = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_CB_ntrack"; TH1F *h1_CB_ntrack = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_CB_npixel"; TH1F *h1_CB_npixel = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_CB_nsct"; TH1F *h1_CB_nsct = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_CB_ntrt"; TH1F *h1_CB_ntrt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_CB_pixsctVStrt"; TH2F *h2_CB_pixsctVStrt = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_CB_pixVSsct"; TH2F *h2_CB_pixVSsct = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  
  histoType="trk_cutflow"; TH2F *h2_cutflow = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);

  histoType="trk_selected_cut5_pt"; TH1F *h1_selected_cut5_pt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_selected_cut5_p"; TH1F *h1_selected_cut5_p = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_selected_cut5_etaVSphi"; TH2F *h2_selected_cut5_etaVSphi = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  
  histoType="trk_selected_pt"; TH1F *h1_selected_pt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_selected_p"; TH1F *h1_selected_p = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_selected_eta"; TH1F *h1_selected_eta = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_selected_phi"; TH1F *h1_selected_phi = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_selected_etaVSphi"; TH2F *h2_selected_etaVSphi = (TH2F*) m_histoManager->GetHisto("TRACKS",histoType);
  
  histoType="trk_associated_Deltapt"; TH1F *h1_associated_Deltapt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_associated_Deltap"; TH1F *h1_associated_Deltap = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_associatedIP_Deltapt"; TH1F *h1_associatedIP_Deltapt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_associatedIP_Deltap"; TH1F *h1_associatedIP_Deltap = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_SAIP_Deltapt"; TH1F *h1_SAIP_Deltapt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_SAIP_Deltap"; TH1F *h1_SAIP_Deltap = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_SAIP2_Deltapt"; TH1F *h1_SAIP2_Deltapt = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);
  histoType="trk_SAIP2_Deltap"; TH1F *h1_SAIP2_Deltap = (TH1F*) m_histoManager->GetHisto("TRACKS",histoType);

  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::comparison MuID(id<100) and STACO (id>=100)::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::MuonBoy           <-> Moore::::::::::::::::::::::::::::::::*/
  /*:::::::::::::StacoMuonSA       <-> MuidSA:::::::::::::::::::::::::::::::*/
  /*:::::::::::::StacoMuonTag      <-> MuTagIMO:::::::::::::::::::::::::::::*/
  /*:::::::::::::StacoMuonCombined <-> MuidCB:::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/    
  
  //  bool DEBUG_DEVEL = false;

  //  if (DEBUG_DEVEL) {
  //    cout << " " << std::endl;
  //    cout << "===== Handling Event " << extra_event.eventInfo().eventNumber() << " " 
  //         << extra_event.eventInfo().tag() << " =====" << endl;
  //    cout << " Nr of PhiPattern: " << extra_event.nrPhiPat() << endl;
  //    cout << " Nr of Tracks (all authors) : " << extra_event.nrTracks() << endl;
  //    cout << " Nr of MBTS hits : " << extra_event.nrMBTS() << endl;
  //  }
  MuonCalibEvent_E::TrackVec::const_iterator trk=extra_event.beginTrack();
  MuonCalibEvent_E::TrackVec::const_iterator trk_end=extra_event.endTrack();
  
  EventHandler m_EventHandler;
  m_EventHandler.setEvent(extra_event);

  MuonCalibSelector::TrackVec trk_selected = m_EventHandler.extendedTracks(m_TrkAuthor);
  MuonCalibSelector::TrackVec trk_inner = m_EventHandler.extendedTracks(1000);
  MuonCalibSelector::TrkCit track_it = trk_selected.begin();
  MuonCalibSelector::TrkCit track_it_end = trk_selected.end();
  //    MuonCalibSelector::TrkCit track_inner = trk_inner.begin();
  //    MuonCalibSelector::TrkCit track_inner_end = trk_inner.end();
 
  int Id_TrackID = 1000 ;   // ID Track
  int Id_TrackSA    = 0 ;   // Moore     (Moore - Default)
  int Id_TrackSA_IP = 10;   // MuidSA    (Moore - Default)
  int Id_TrackTag   = 30;   // MuTagIMO  (Moore - Default)
  int Id_TrackCB    = 20;   // MuidCB    (Moore - Default)
  if (m_TrkAuthor==100) {
    Id_TrackSA    = 100;  // MuonBoy
    Id_TrackSA_IP = 141;  // StacoMuonSA
    Id_TrackTag   = 142;  // StacoMuonTag
    Id_TrackCB    = 140;  // StacoMuonCB
  }

  MuonCalibSelector::TrackVec trk_ID    = m_EventHandler.extendedTracks(Id_TrackID);
  MuonCalibSelector::TrackVec trk_SA    = m_EventHandler.extendedTracks(Id_TrackSA);
  MuonCalibSelector::TrackVec trk_SA_IP = m_EventHandler.extendedTracks(Id_TrackSA_IP);
  MuonCalibSelector::TrackVec trk_Tag   = m_EventHandler.extendedTracks(Id_TrackTag);
  MuonCalibSelector::TrackVec trk_CB    = m_EventHandler.extendedTracks(Id_TrackCB);

  MuonCalibSelector::TrkCit trk_ID_it = trk_ID.begin();
  MuonCalibSelector::TrkCit trk_ID_end = trk_ID.end();
  MuonCalibSelector::TrkCit trk_SA_it = trk_SA.begin();
  MuonCalibSelector::TrkCit trk_SA_end = trk_SA.end();
  MuonCalibSelector::TrkCit trk_SAIP_it = trk_SA_IP.begin();
  MuonCalibSelector::TrkCit trk_SAIP_end = trk_SA_IP.end();
  MuonCalibSelector::TrkCit trk_Tag_it = trk_Tag.begin();
  MuonCalibSelector::TrkCit trk_Tag_end = trk_Tag.end();
  MuonCalibSelector::TrkCit trk_CB_it = trk_CB.begin();
  MuonCalibSelector::TrkCit trk_CB_end = trk_CB.end();
  
  MuonCalibSelector::TrkCit trk_ID_it2 = trk_ID.begin();
  MuonCalibSelector::TrkCit trk_ID_end2 = trk_ID.end();
  MuonCalibSelector::TrkCit trk_SA_it2 = trk_SA.begin();
  MuonCalibSelector::TrkCit trk_SA_end2 = trk_SA.end();
  MuonCalibSelector::TrkCit trk_SAIP_it2 = trk_SA_IP.begin();
  MuonCalibSelector::TrkCit trk_SAIP_end2 = trk_SA_IP.end();
  MuonCalibSelector::TrkCit trk_Tag_it2 = trk_Tag.begin();
  MuonCalibSelector::TrkCit trk_Tag_end2 = trk_Tag.end();
  MuonCalibSelector::TrkCit trk_CB_it2 = trk_CB.begin();
  MuonCalibSelector::TrkCit trk_CB_end2 = trk_CB.end();
  
  MuonCalibSelector::TrkCit trk_SAIP_it3 = trk_SA_IP.begin();
  MuonCalibSelector::TrkCit trk_SAIP_end3 = trk_SA_IP.end();
  
  //    MuonCalibTriggerInfo bunch;
  //    if( bunch.bcIndex() >0) cout << bunch.bcIndex() << endl;

  int i_trk_sel = 0;
    
  /*::::::::::::::::::::::::::::::::::::::::::*/
  /*:::info from scintillators::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::*/
  MuonCalibEvent_E::MBTSVec::const_iterator mbhit = extra_event.beginMBTS();
  MuonCalibEvent_E::MBTSVec::const_iterator mbhit_end = extra_event.endMBTS();
  int nMBTS1 = 0;
  double chargeMBTS1[100];
  double timeMBTS1[100];
  int nMBTS2 = 0;
  double chargeMBTS2[100];
  double timeMBTS2[100];
  Amg::Vector3D posMBTS(0.,0.,0.);

  for(; mbhit != mbhit_end; ++mbhit ){ // Loop over MBTS hits
    if ( (*mbhit)->charge() > 0.) {
      posMBTS = (*mbhit)->position();
      if( posMBTS.z() > 1000. ) {
	chargeMBTS1[nMBTS1] = (*mbhit)->charge();
	timeMBTS1[nMBTS1] = (*mbhit)->time();
	nMBTS1++;
      }
      if( posMBTS.z() < -1000. ) {
	chargeMBTS2[nMBTS2] = (*mbhit)->charge();
	timeMBTS2[nMBTS2] = (*mbhit)->time();
	nMBTS2++;
      }
      // if (h1tmbts) h1tmbts->Fill( timeMBTS );  
    }
  } // end loop over MBTS hits

  double time1_mean = 0.;
  double chargeWeight1 = 0.;
  for (int i=0; i<nMBTS1; i++){
    time1_mean += timeMBTS1[i]*chargeMBTS1[i];
    chargeWeight1 += chargeMBTS1[i];
  }
  time1_mean = time1_mean/chargeWeight1;
  
  double time2_mean = 0.;
  double chargeWeight2 = 0.;
  for (int i=0; i<nMBTS2; i++){
    time2_mean += timeMBTS2[i]*chargeMBTS2[i];
    chargeWeight2 += chargeMBTS2[i];
  }
  time2_mean = time2_mean/chargeWeight2;

  double dTimeMBTS = -1000.;
  if (nMBTS1>0 && h1tmbts1 ) h1tmbts1->Fill(time1_mean);
  if (nMBTS2>0 && h1tmbts2 ) h1tmbts2->Fill(time2_mean);
  if (nMBTS1>0 && nMBTS2>0) {
    if (h2mbts1VSmbts2) h2mbts1VSmbts2->Fill(time2_mean,time1_mean);
    dTimeMBTS = time1_mean-time2_mean;
    if (h1Dtmbts) h1Dtmbts->Fill(dTimeMBTS);
  }

  /*:::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::loop to check the authors:::::::::::*/
  /*:::::::::::::::::::::::::::::::::::::::::::*/
  
  for(;trk!=trk_end;++trk) {
    if(*trk) {

      if (h1author1) {
	if ((*trk)->author() == 1000) h1author1->Fill(0.); // ID
	if ((*trk)->author() == 0)    h1author1->Fill(1.); // Moore
	if ((*trk)->author() == 1) h1author1->Fill(2.);    // BackExtrapolated SA
	if ((*trk)->author() == 2) h1author1->Fill(3.);    // BackExtrapolated SA calo energy correction
	if ((*trk)->author() == 10) h1author1->Fill(4.);   // MuidSA
	if ((*trk)->author() == 20) h1author1->Fill(5.);   // MuidCB
	if ((*trk)->author() == 25) h1author1->Fill(6.);   // MuonCombined ???
	if ((*trk)->author() == 30) h1author1->Fill(7.);   // MuTagIMO
	if ((*trk)->author() == 35) h1author1->Fill(8.);   // MuGirl
	if ((*trk)->author() == 41) h1author1->Fill(9.);   // AODMuidSA
	if ((*trk)->author() == 40) h1author1->Fill(10.);  // AODMuidCB
	if ((*trk)->author() == 42) h1author1->Fill(11.);  // AODMuTagIMO
	if ((*trk)->author() == 43) h1author1->Fill(12.);  // AODMuGirl
      }
	  
      if (h1author2) {
	if ((*trk)->author() == 1000) h1author2->Fill(0.); // ID
	if ((*trk)->author() == 100) h1author2->Fill(1.);  // MuonBoy
	if ((*trk)->author() == 120) h1author2->Fill(2.);  // Staco
	if ((*trk)->author() == 130) h1author2->Fill(3.);  // MuTagTracks
	if ((*trk)->author() == 141) h1author2->Fill(4.);  // StacoMuonSA
	if ((*trk)->author() == 140) h1author2->Fill(5.);  // StacoMuonCB
	if ((*trk)->author() == 142) h1author2->Fill(6.);  // StacoMuonTag
      }
      if (h2chi2dof_VS_author) h2chi2dof_VS_author->Fill( (*trk)->chi2()/(*trk)->ndof(),(*trk)->author() );
    }	
  }

  if (h1trkIDmult) h1trkIDmult->Fill(trk_ID.size());
  
  //  if (DEBUG_DEVEL) cout << " DEBUG ID momenta : " << endl;

  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::::::::::::ALGORITHM DEPENDENT (MUID or STACO)::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  
  int ID_ntrack = 0;
  int SA_ntrack = 0;
  int SAIP_ntrack = 0;
  int Tag_ntrack = 0;
  int CB_ntrack = 0;
  int Ev_pveto = 0;
  int Ev_ngoodtrack = 0;
  int Ev_npixel_veto = 0;
  int Ev_nsct_veto   = 0;
  int ID_npixel = 0;
  int ID_nsct   = 0;
  int ID_ntrt   = 0;
  //    int ID_ntrtb  = 0;
  int Tag_npixel= 0;
  int Tag_nsct  = 0;
  int Tag_ntrt  = 0;
  //    int Tag_ntrtb = 0;
  int CB_npixel = 0;
  int CB_nsct   = 0;
  int CB_ntrt   = 0;
  //    int CB_ntrtb  = 0;
  
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::INNER DETECTOR :::::::::::::::::::*/
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  for(; trk_ID_it != trk_ID_end; ++trk_ID_it ){
    ID_ntrack++;
    ID_npixel = 0;
    ID_nsct = 0;
    ID_ntrt = 0;
    //      ID_ntrtb = 0;
    ID_npixel = number_IDhits( trk_ID_it, 1);     
    ID_nsct   = number_IDhits( trk_ID_it, 2);
    ID_ntrt   = number_IDhits( trk_ID_it, 3);
    //      ID_ntrtb  = number_IDhits( trk_ID_it, 4);
    
    if(ID_npixel>0) Ev_npixel_veto++;
    if(ID_nsct>5) Ev_nsct_veto++;
    if(ID_npixel>0 && ID_nsct>5) Ev_ngoodtrack++;
    if( h1_ID_npixel ) h1_ID_npixel->Fill(ID_npixel);
    if( h1_ID_nsct ) h1_ID_nsct->Fill(ID_nsct);
    if( h1_ID_ntrt ) h1_ID_ntrt->Fill(ID_ntrt);
    if( h2_ID_pixsctVStrt ) h2_ID_pixsctVStrt->Fill(ID_npixel+ID_nsct, ID_ntrt); 
    if( h2_ID_pixVSsct ) h2_ID_pixVSsct->Fill(ID_npixel,ID_nsct); 
    
    float  ID_p   = (*trk_ID_it)->p()/1000.;
    if(h1_ID_p) h1_ID_p->Fill(ID_p); 
    if(ID_p>4) Ev_pveto++;
  } // end loop on tracks
  if ( h1_ID_ntrack ) h1_ID_ntrack->Fill(ID_ntrack);
  if ( h1_Ev_pveto ) h1_Ev_pveto->Fill(Ev_pveto);
  if ( h1_Ev_trkveto ) h1_Ev_trkveto->Fill(Ev_ngoodtrack);
  
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::MUON SPECTROMETER STANDALONE:::::::::::::*/
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/

  for(; trk_SA_it != trk_SA_end; ++trk_SA_it ){
    SA_ntrack++;
  }
  if ( h1_SA_ntrack ) h1_SA_ntrack->Fill(SA_ntrack); 

  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::MUON SPECTROMETER STANDALONE  IP:::::::::*/
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/

  for(; trk_SAIP_it != trk_SAIP_end; ++trk_SAIP_it ){
    SAIP_ntrack++;
  }
  if ( h1_SAIP_ntrack ) h1_SAIP_ntrack->Fill(SAIP_ntrack); 

  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::: TAG (ID + segment in the SM)::::::::::::*/
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  
  for(; trk_Tag_it != trk_Tag_end; ++trk_Tag_it ){
    Tag_ntrack++;
    Tag_npixel = 0;
    Tag_nsct = 0;
    Tag_ntrt = 0;
    //      Tag_ntrtb = 0;
    Tag_npixel = number_IDhits( trk_Tag_it, 1);
    Tag_nsct   = number_IDhits( trk_Tag_it, 2);
    Tag_ntrt   = number_IDhits( trk_Tag_it, 3);
    //      Tag_ntrtb  = number_IDhits( trk_Tag_it, 4);
    
    if ( h1_Tag_npixel ) h1_Tag_npixel->Fill(Tag_npixel);
    if ( h1_Tag_nsct ) h1_Tag_nsct->Fill(Tag_nsct);
    if ( h1_Tag_ntrt ) h1_Tag_ntrt->Fill(Tag_ntrt);
    if( h2_Tag_pixsctVStrt ) h2_Tag_pixsctVStrt->Fill(Tag_npixel+Tag_nsct, Tag_ntrt); 
    if( h2_Tag_pixVSsct ) h2_Tag_pixVSsct->Fill(Tag_npixel,Tag_nsct); 
    
  }
  if ( h1_Tag_ntrack ) h1_Tag_ntrack->Fill(Tag_ntrack);

  /*:::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::::COMBINED MUONS::::::::::::::::::*/
  /*:::::::::::::::::::::::::::::::::::::::::::::::*/

  for(; trk_CB_it != trk_CB_end; ++trk_CB_it ){
    CB_ntrack++;      
    CB_npixel = 0;
    CB_nsct = 0;
    CB_ntrt = 0;
    //      CB_ntrtb = 0;
    CB_npixel = number_IDhits( trk_CB_it, 1);
    CB_nsct   = number_IDhits( trk_CB_it, 2);
    CB_ntrt   = number_IDhits( trk_CB_it, 3);
    //      CB_ntrtb  = number_IDhits( trk_CB_it, 4);
    
    if ( h1_CB_npixel ) h1_CB_npixel->Fill(CB_npixel);
    if ( h1_CB_nsct ) h1_CB_nsct->Fill(CB_nsct);
    if ( h1_CB_ntrt ) h1_CB_ntrt->Fill(CB_ntrt);
    if( h2_CB_pixsctVStrt ) h2_CB_pixsctVStrt->Fill(CB_npixel+CB_nsct,CB_ntrt); 
    if( h2_CB_pixVSsct ) h2_CB_pixVSsct->Fill(CB_npixel,CB_nsct); 
    if ( h1_CB_ntrack ) h1_CB_ntrack->Fill(CB_ntrack);      
  }
  
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::::::::::CUT FLOW:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/  
  
  bool IsCombined = false;    
  //    MuonCalibSelector::TrkCit trk_ID_it3 = trk_ID.begin();
  //    MuonCalibSelector::TrkCit trk_ID_end3 = trk_ID.end();

  for(; trk_CB_it2 != trk_CB_end2; ++trk_CB_it2 ) {
    double eta = 0;
    double phi = 0;
    double pt  = 0;
    double p   = 0;

    phi = (*trk_CB_it2)->phi();
    eta = -log(tan((*trk_CB_it2)->theta()/2.));
    pt  = (*trk_CB_it2)->pt()/1000;
    p   = (*trk_CB_it2)->p()/1000;      
    int CB_npixel = 0;
    int CB_nsct = 0;
    CB_npixel = number_IDhits( trk_CB_it2, 1);
    CB_nsct   = number_IDhits( trk_CB_it2, 2);      
    int CB_HasCutLevel = -10;
    CB_HasCutLevel = cutflow( trk_CB_it2, dTimeMBTS, Ev_ngoodtrack, Ev_pveto, CB_npixel, CB_nsct);
    for(int i =0;i<=CB_HasCutLevel;i++){
      if(h2_cutflow) h2_cutflow->Fill(5,i);
    }
    /*:::: BASIC DISTRIBUTIONS OF THE SELECTED COMBINED MUON  :::::::::::::::::::::::::::::::*/ 
    if(CB_HasCutLevel>4){ 
      if(h1_selected_cut5_pt) h1_selected_cut5_pt->Fill(pt);
      if(h1_selected_cut5_p) h1_selected_cut5_p->Fill(p);
      if(h2_selected_cut5_etaVSphi) h2_selected_cut5_etaVSphi->Fill(phi,eta);
    }        
    if(CB_HasCutLevel>6){ 
      if(h1_selected_pt) h1_selected_pt->Fill(pt);
      if(h1_selected_p) h1_selected_p->Fill(p);
      if(h1_selected_eta) h1_selected_eta->Fill(eta);
      if(h1_selected_phi) h1_selected_phi->Fill(phi);
      if(h2_selected_etaVSphi) h2_selected_etaVSphi->Fill(phi,eta);
      IsCombined = true;
    }

    if(CB_HasCutLevel>0 && m_TrkAuthor==0  && (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag) && (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA) ){
      if(h1_associated_Deltapt ) h1_associated_Deltapt->Fill( 
							     ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->pt() -  (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->pt())/1000.  );
      if(h1_associated_Deltap) h1_associated_Deltap->Fill( 
							  ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->p() - (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->p())/1000.  );
    }
    if(CB_HasCutLevel>0 && m_TrkAuthor==0  && (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag) && (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP) ){
      if(h1_associatedIP_Deltapt ) h1_associatedIP_Deltapt->Fill( 
								 ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->pt() -  (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->pt())/1000.  );
      if(h1_associatedIP_Deltap) h1_associatedIP_Deltap->Fill( 
							      ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->p() - (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->p())/1000.  );
    }
    
    for(; trk_SAIP_it3 != trk_SAIP_end3; ++trk_SAIP_it3 ){
      if(CB_HasCutLevel>0 && m_TrkAuthor==0  && (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag) && (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA) && (*trk_SAIP_it3) ){
	if(h1_SAIP_Deltapt ) h1_SAIP_Deltapt->Fill( 
						   ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->pt() -  (*trk_SAIP_it3)->pt())/1000.  );
	if(h1_SAIP_Deltap) h1_SAIP_Deltap->Fill( 
						( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->p() - (*trk_SAIP_it3)->p())/1000.  );
      }
      if(CB_HasCutLevel>0 && m_TrkAuthor==0  && (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag) && (*trk_SAIP_it3) ){
	if(h1_SAIP2_Deltapt ) h1_SAIP2_Deltapt->Fill( 
						     ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->pt() -  (*trk_SAIP_it3)->pt())/1000.  );
	if(h1_SAIP2_Deltap) h1_SAIP2_Deltap->Fill( 
						  ( (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->p() - (*trk_SAIP_it3)->p())/1000.  );
      }
    }
// 	cout <<"--------------------------------------------------------------------------------------------"<< endl;	
//  	cout <<"COMBINED"<<' '<< "x " << (*trk_CB_it2)->x0() << "  y" << (*trk_CB_it2)->y0() << "  z " << (*trk_CB_it2)->z0() 
//  	     << "  p " << (*trk_CB_it2)->p() <<  "  pt " << (*trk_CB_it2)->pt() << endl;
// 	cout <<"ASSO TAG"<<' '<< "x " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->x0() << "  y" << (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->y0() 
//  	       << "  z " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->z0() 
//  	       << "  p " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->p() <<  "  pt " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackTag)->pt() << endl;
// 	if((*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)){
// 	  cout <<"ASSO SA"<<' '<< "x " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->x0() << "  y" << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->y0() 
//  	       << "  z " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->z0() 
//  	       << "  p " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->p() <<  "  pt " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA)->pt() << endl;}
// 	if((*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)){
// 	  cout <<"ASSO SA_IP"<<' '<< "x " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->x0() << "  y" << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->y0() 
//  	       << "  z " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->z0() 
// 	       << "  p " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->p() <<  "  pt " << (*trk_CB_it2)->getAssociatedTrack(Id_TrackSA_IP)->pt() << endl;}

  }
  
  for(; trk_ID_it2 != trk_ID_end2; ++trk_ID_it2 ){
    int ID_npixel = 0;
    int ID_nsct = 0;
    ID_npixel = number_IDhits( trk_ID_it2, 1);
    ID_nsct   = number_IDhits( trk_ID_it2, 2);
    int ID_HasCutLevel = -10;
    ID_HasCutLevel = cutflow( trk_ID_it2, dTimeMBTS, Ev_ngoodtrack, Ev_pveto, ID_npixel, ID_nsct);
    for(int i =0;i<=ID_HasCutLevel;i++){
      if(h2_cutflow) h2_cutflow->Fill(1,i);
    }
    if(ID_HasCutLevel>1 && IsCombined){ 
      // 	cout <<"ID"<<' '<< "x " << (*trk_ID_it2)->x0() << "  y " << (*trk_ID_it2)->y0() << "  z " << (*trk_ID_it2)->z0() 
      // 	     << "  p " << (*trk_ID_it2)->p() <<  "  pt " << (*trk_ID_it2)->pt() << endl;
    }
  }
  
  for(; trk_SA_it2 != trk_SA_end2; ++trk_SA_it2 ){
    int SA_npixel = 0;
    int SA_nsct = 0;
    SA_npixel = number_IDhits( trk_SA_it2, 1);
    SA_nsct   = number_IDhits( trk_SA_it2, 2);
    int SA_HasCutLevel = -10;
    SA_HasCutLevel = cutflow( trk_SA_it2, dTimeMBTS, Ev_ngoodtrack, Ev_pveto, SA_npixel, SA_nsct);
    
    for(int i =0;i<=SA_HasCutLevel;i++){
      if(h2_cutflow) h2_cutflow->Fill(2,i);
    }
    if(SA_HasCutLevel>1 && IsCombined){ 
//  	cout <<"SA"<<' '<< "x " << (*trk_SA_it2)->x0() << "  y " << (*trk_SA_it2)->y0() << "  z " << (*trk_SA_it2)->z0() 
//  	     << "  p " << (*trk_SA_it2)->p() <<  "  pt " << (*trk_SA_it2)->pt() << endl;
    }
  } 
  
  for(; trk_SAIP_it2 != trk_SAIP_end2; ++trk_SAIP_it2 ){
    int SAIP_npixel = 0;
    int SAIP_nsct = 0;
    SAIP_npixel = number_IDhits( trk_SAIP_it2, 1);
    SAIP_nsct   = number_IDhits( trk_SAIP_it2, 2);
    int SAIP_HasCutLevel = -10;
    SAIP_HasCutLevel = cutflow( trk_SAIP_it2, dTimeMBTS, Ev_ngoodtrack, Ev_pveto, SAIP_npixel, SAIP_nsct);
    for(int i =0;i<=SAIP_HasCutLevel;i++){
      if(h2_cutflow) h2_cutflow->Fill(3,i);
    }
    if(SAIP_HasCutLevel>1 && IsCombined){ 
      //  	cout <<"SAIP"<<' '<< "x " << (*trk_SAIP_it2)->x0() << "  y " << (*trk_SAIP_it2)->y0() << "  z " << (*trk_SAIP_it2)->z0() 
      //  	     << "  p " << (*trk_SAIP_it2)->p() <<  "  pt " << (*trk_SAIP_it2)->pt() << endl;
    }
  } 
    
  for(; trk_Tag_it2 != trk_Tag_end2; ++trk_Tag_it2 ) {
    int Tag_npixel = 0;
    int Tag_nsct = 0;
    Tag_npixel = number_IDhits( trk_Tag_it2, 1);
    Tag_nsct   = number_IDhits( trk_Tag_it2, 2);
    int Tag_HasCutLevel = -10;
    Tag_HasCutLevel = cutflow( trk_Tag_it2, dTimeMBTS, Ev_ngoodtrack, Ev_pveto, Tag_npixel, Tag_nsct);
    for(int i =0;i<=Tag_HasCutLevel;i++){
      if(h2_cutflow) h2_cutflow->Fill(4,i);
    }
    if(Tag_HasCutLevel>1 && IsCombined){ 
//  	cout <<"TAG"<<' '<< "x " << (*trk_Tag_it2)->x0() << "  y " << (*trk_Tag_it2)->y0() << "  z " << (*trk_Tag_it2)->z0() 
//  	     << "  p " << (*trk_Tag_it2)->p() <<  "  pt " << (*trk_Tag_it2)->pt() << endl;
    }
  }

  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/  

  int i_mdt = 0; // mdt hits for each track
  int i_rpc = 0; // rpc hits for each track
  int i_csc = 0; // csc hits for each track
  int i_tgc = 0; // tgc hits for each track

  // MODIFICATIONS INTRODUCED FOR MUON VETO ANALYSIS WITH SEGMENTS refitted-t0

  bool CollisionEvent = false;
  if(abs(dTimeMBTS)<10 && Ev_ngoodtrack>2 && Ev_pveto>0 ) CollisionEvent=true;

  // ----------- Choose datacards here according to your analysis -----
  // HERE DECIDE WHETHER THE EVENT MUST BE A COLLISION CANDIDATE OR NOT :
  bool goodEvent = true;
  goodEvent = CollisionEvent;   // IN CASE YOU WANT ALL EVENTS, just COMMENT THIS LINE

  // HERE SELECT THE TRACK TYPE
  string TrackMDTtime_selection = "SA";
  // string TrackMDTtime_selection = "SA_IP";
  // string TrackMDTtime_selection = "Tag";
  // string TrackMDTtime_selection = "CB";
  // string TrackMDTtime_selection = "NONE";
  
  // HERE SELECT THE CUT THE TRACK MUST PASS
  // int PASSEDCUT = 0;  // if goodEvent = CollisionEvent is enables 
                         // PASSEDCUT = 0,1,2,3 is equivalent to PASSEDCUT = 4
  int PASSEDCUT = 0;
  // ----------- END of datacards according to your analysis -----

  if (goodEvent) {

    histoType="trk_d0VSz0_GoodTime"; TH2F *h2d0VSz0_goodT = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
    histoType="trk_d0VSz0_BadTime"; TH2F *h2d0VSz0_badT = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
    histoType="trk_d0VSz0_noSeg"; TH2F *h2d0VSz0_noSeg = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);

    MuonCalibSelector::TrkCit trkMDTtime_it;
    MuonCalibSelector::TrkCit trkMDTtime_it_end;

    if (TrackMDTtime_selection == "SA") {
      trkMDTtime_it  = trk_SA.begin();
      trkMDTtime_it_end = trk_SA.end();
    }
    if (TrackMDTtime_selection == "SA_IP") {
      trkMDTtime_it  = trk_SA.begin();
      trkMDTtime_it_end = trk_SA.end();
    }
    if (TrackMDTtime_selection == "Tag") {
      trkMDTtime_it  = trk_Tag.begin();
      trkMDTtime_it_end = trk_Tag.end();
    }
    if (TrackMDTtime_selection == "CB") {
      trkMDTtime_it  = trk_CB.begin();
      trkMDTtime_it_end = trk_CB.end();
    }

    if (TrackMDTtime_selection == "NONE") {
      std::vector<const MuonCalibExtendedSegment*> segs;
    
      for (unsigned int k=position; k<segments.size(); k++) {    // LOOP OVER SEGMENTS 
	MuonCalibSegment *segment(segments[k]);
	// int autore = segment->author();
	// int fitOK = 0;
	// if (segment->hasFittedT0() ) fitOK = 1;
	// cout << " DEBUG DEBUG : autore, fitOK, t0, chi2 " 
	//      <<autore<<" "<<fitOK<<" "<<segment->fittedT0()<<" "<<segment->chi2() <<endl;

	const MuonCalibExtendedSegment *segext = (const MuonCalibExtendedSegment* ) segment;
	segs.push_back(segext);
      }
      //       int iallSegMDTtime = -99;
      //if (segs.size()>0) iallSegMDTtime = MDTtiming(segs);

    } else {
      for(;trkMDTtime_it!=trkMDTtime_it_end;++trkMDTtime_it){

	int iMDTtime = -99;
	int TRACK_npixel = 0;
	int TRACK_nsct = 0;
	TRACK_npixel = number_IDhits( trkMDTtime_it, 1);
	TRACK_nsct   = number_IDhits( trkMDTtime_it, 2);
	int TRACK_HasCutLevel = -10;
	TRACK_HasCutLevel = cutflow( trkMDTtime_it, dTimeMBTS, Ev_ngoodtrack, Ev_pveto, TRACK_npixel, TRACK_nsct);
	if (TRACK_HasCutLevel<PASSEDCUT) {
	  //cout << "DEBUG DEBUG DEBUG : TRACK_HasCutLevel = "<< TRACK_HasCutLevel << endl;
	  continue;
	}
	const std::vector<const MuonCalibExtendedSegment*> assoSegments = (*trkMDTtime_it)->associatedSegments();
	bool additionalCut = true;
	// additionalCut = (*trkMDTtime_it)->z0ip()>2800. && (*trkMDTtime_it)->z0ip()<3800.;
	if (assoSegments.size()>0 && additionalCut ) iMDTtime = MDTtiming(assoSegments);
	// cout << " DEBUG DEBUG iMDTtime = "<<iMDTtime<<endl;
	if (iMDTtime==1) {
	  // cout << " debug filling 1 " << endl;
	  if (h2d0VSz0_goodT) {
	    // cout << " debug filling 1 " << endl;
	    h2d0VSz0_goodT->Fill( (*trkMDTtime_it)->z0ip(), (*trkMDTtime_it)->d0());
	  }
	}
	if (iMDTtime==0) {
	  // cout << " debug filling 0 " << endl;
	  if (h2d0VSz0_badT) {
	    // cout << " debug filling 0 " << endl;
	    h2d0VSz0_badT->Fill( (*trkMDTtime_it)->z0ip(), (*trkMDTtime_it)->d0());
	  }
	}
	if (iMDTtime==-1) {
	  // cout << " debug filling -1 " << endl;
	  if (h2d0VSz0_noSeg) {
	    // cout << " debug filling -1 " << endl;
	    h2d0VSz0_noSeg->Fill( (*trkMDTtime_it)->z0ip(), (*trkMDTtime_it)->d0());
	  }
	}
      }
    }
  }  // close IF goodEvent
  //
  // END MODIFICATIONS INTRODUCED FOR MUON VETO ANALYSIS WITH SEGMENTS refitted-t0

  /*::::::::::::::::::::::::::::::::::::::::::::::*/
  /*:::::::loop on non cut tracks          :::::::*/
  /*::::::::::::::::::::::::::::::::::::::::::::::*/
  for(;track_it!=track_it_end;++track_it) {
      
    double eta = -log ( tan ( (*track_it)->theta()/2));

    i_trk_sel++;

    if (h1pt) h1pt->Fill( (*track_it)->pt()/1000. ); // in GeV
    if (h1eta) h1eta->Fill( eta );
    if (h1phi) h1phi->Fill( (*track_it)->phi() );
    if (h2etaVSphi) h2etaVSphi->Fill( eta, (*track_it)->phi());
    if (h1d0) h1d0->Fill( (*track_it)->d0());
    if (h1z0ip) h1z0ip->Fill( (*track_it)->z0ip());
    if (h2d0VSz0) h2d0VSz0->Fill( (*track_it)->z0ip(), (*track_it)->d0());
    if (h2yVSx) h2yVSx->Fill( (*track_it)->x0() , (*track_it)->y0() );
    if (h1qoverp) h1qoverp->Fill( (*track_it)->qOverP()*1000. );  // in Gev^-1
    if (h1chi2dof) h1chi2dof->Fill( (*track_it)->chi2()/(*track_it)->ndof() );
    if (h2chi2dof_VS_eta) h2chi2dof_VS_eta->Fill( eta, (*track_it)->chi2()/(*track_it)->ndof() );
    if (h1segments) h1segments->Fill( (*track_it)->associatedSegments().size() );
    
    MuonCalibTrack_E::HitVector::const_iterator hit=(*track_it)->beginHit();
    MuonCalibTrack_E::HitVector::const_iterator hit_end=(*track_it)->endHit();
    
    for(;hit!=hit_end;++hit){

      MuonFixedId identifier =  (*hit)->identify();

      if( identifier.is_mdt()  ){  // MDT hits
	if (h1pull) h1pull->Fill( (*hit)->pull());
	if (h1driftR) h1driftR->Fill( (*hit)->driftRadius()); 
	i_mdt++;
      }
      if( identifier.is_rpc()  ){  // RPC hits
	i_rpc++;
      }
      if( identifier.is_tgc()  ){  // TGC hits
	i_tgc++;
      }
      if( identifier.is_csc()  ){  // CSC hits
	i_csc++;
      }
    }
      
    if(i_mdt>0){
      if (h1mdt) h1mdt->Fill(i_mdt); 
    }
    if(i_rpc>0){
      if (h1rpc) h1rpc->Fill(i_rpc); 
      if(i_mdt>0){
	if (h2mdtVSrpc) h2mdtVSrpc->Fill(i_rpc,i_mdt);
      }
    }
    if(i_tgc>0){
      if (h1tgc) h1tgc->Fill(i_tgc); 
    }
    if(i_csc>0){
      if (h1csc) h1csc->Fill(i_csc); 
    }
    
    if (h2mdthits_VS_eta) h2mdthits_VS_eta->Fill( eta,i_mdt );
    if (h2mdthits_VS_phi) h2mdthits_VS_phi->Fill((*track_it)->phi() ,i_mdt );

    //    if(i_tgc>0 || i_rpc>0){      
    //    }
  }

  /********************************************************************************************/

  if (h1tracks) h1tracks->Fill(extra_event.nrTracks()); // including all authors    
  if (h1tracks_sel) h1tracks_sel->Fill( i_trk_sel );    // only selected author

  return;
}  //end MdtDqaTrackAnalysis::handleEvent

int MdtDqaTrackAnalysis::number_IDhits(MuonCalibSelector::TrackVec::const_iterator track, int detector){
  int npixel = 0;
  int nsct = 0;
  int ntrt = 0;
  int ntrtb = 0;
  int nhit = 0;
  MuonCalibTrack_E::HitVector::const_iterator it_hit = (*track)->beginHit();
  MuonCalibTrack_E::HitVector::const_iterator it_hit_end = (*track)->endHit();
  for (;it_hit != it_hit_end; ++it_hit) {
    if ((*it_hit)->type() != 1) continue;
    if ((*it_hit)->position().perp() < 200.) {
      npixel++;
      if(detector ==1) nhit++; 
    } else if ((*it_hit)->position().perp() < 540.) {
      nsct++;
      if(detector ==2) nhit++; 
    } else if ((*it_hit)->position().perp() < 1100.) {
      ntrt++;
      if(detector ==3) nhit++; 
      if(fabs((*it_hit)->position().z()) < 750.){
	ntrtb++;
	if(detector ==4) nhit++; 
      }
    }
  }
  
  return nhit;
}  //end MdtDqaTrackAnalysis::number_IDhits

int MdtDqaTrackAnalysis::cutflow(MuonCalibSelector::TrackVec::const_iterator track, double dTimeMBTS, int Ev_ngoodtrack, int Ev_pveto,  int npixel, int nsct) {
  int HasPassedLevel = -10;
  
  double eta = 0;
  double pt  = 0;
  double p  = 0;
  eta = -log(tan((*track)->theta()/2.));
  pt  = (*track)->pt()/1000;
  p   = (*track)->p()/1000; 

  HasPassedLevel = 0;    
  HasPassedLevel = 1; 
  if(HasPassedLevel == 1 && abs(dTimeMBTS)<10)                                     HasPassedLevel = 2; 
  if(HasPassedLevel == 2 && Ev_ngoodtrack>2)                                       HasPassedLevel = 3; 
  if(HasPassedLevel == 3 && Ev_pveto>0)                                            HasPassedLevel = 4;  
  if(HasPassedLevel == 4)                                                          HasPassedLevel = 5;     
  if(HasPassedLevel == 5 && abs(eta)<2.5 && pt>2.5)                                HasPassedLevel = 6;
  if(HasPassedLevel == 6 && p>4.)                                                  HasPassedLevel = 7;
  if(HasPassedLevel == 7 && npixel>0 && nsct>5)                                    HasPassedLevel = 8;
  
  return HasPassedLevel;
}  //end MdtDqaTrackAnalysis::cutflow

int MdtDqaTrackAnalysis::MDTtiming(const std::vector<const MuonCalibExtendedSegment*> &segments ){
  //
  // MODIFICATIONS INTRODUCED FOR MUON VETO ANALYSIS WITH SEGMENTS refitted-t0
  //
  string histoType;
  histoType="nSegPerTrack"; TH1F *h1_nSegAll = (TH1F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="nSegPerTrack_HitCut"; TH2F *h2_nSegHitCut = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);

  histoType="t0_BI"; TH2F *h2_t0BI = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_BM"; TH2F *h2_t0BM = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_BO"; TH2F *h2_t0BO = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_EI"; TH2F *h2_t0EI = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_EM"; TH2F *h2_t0EM = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_EO"; TH2F *h2_t0EO = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  
  histoType="t0_BO_BM"; TH2F *h2_t0BOBM = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_BO_BI"; TH2F *h2_t0BOBI = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_BM_BI"; TH2F *h2_t0BMBI = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_EO_EM"; TH2F *h2_t0EOEM = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_EO_EI"; TH2F *h2_t0EOEI = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);
  histoType="t0_EM_EI"; TH2F *h2_t0EMEI = (TH2F*) m_histoManager->GetHisto("DEBUG",histoType);

  //  bool debug_seg = false;

  if(h1_nSegAll) h1_nSegAll->Fill(segments.size());

  const int NSEGMAX = 20;       // Maximum of Segments in the same Layer (Inner, Middle, Outer)
  float t0acceptedSeg[NSEGMAX];
  int nAcceptedSeg=0;
  float t0refit_BI[NSEGMAX][2];
  float t0refit_BM[NSEGMAX][2];
  float t0refit_BO[NSEGMAX][2];
  float t0refit_EI[NSEGMAX][2];
  float t0refit_EM[NSEGMAX][2];
  float t0refit_EO[NSEGMAX][2];
  int nseg_BI = 0;
  int nseg_BM = 0;
  int nseg_BO = 0;
  int nseg_EI = 0;
  int nseg_EM = 0;
  int nseg_EO = 0;
  for (int i=0; i<NSEGMAX; i++) {
    t0refit_BI[i][0] = 99999.; 
    t0refit_BM[i][0] = 99999.; 
    t0refit_BO[i][0] = 99999.; 
    t0refit_EI[i][0] = 99999.; 
    t0refit_EM[i][0] = 99999.; 
    t0refit_EO[i][0] = 99999.; 
  }
  
  unsigned int MINNUMHITS = 5;
  int nseg_hitCut = 0;
  //  if(debug_seg) cout << " -------------   Start loop on Segments. Number of segments:  " << segments.size() << endl;
  std::vector<const MuonCalibExtendedSegment*>::const_iterator seg_it;
  for (seg_it = segments.begin(); seg_it!=segments.end(); seg_it++) {   // START LOOP ON SEGMENTS
//          unsigned int nMDThits = (*seg_it)->mdtHitsOnTrack();
    if ((*seg_it)->mdtHitsOnTrack() >= MINNUMHITS) {  // IF segment has >= MINNUMHITS
      nseg_hitCut++;
      if ( nseg_hitCut >=NSEGMAX ) {
	//	cout << " WARNING : Number of segments in the same layer (inner/mid/out) GREATER THAN "<<NSEGMAX 
	//	     << " segment skipped " << endl;
	continue;
      }

      // MDT hits on segment 
      std::vector<MdtCalibHitBase*>::const_iterator aHit_it = (*seg_it)->mdtHOTBegin();
      std::vector<MdtCalibHitBase*>::const_iterator aHit_it_end = (*seg_it)->mdtHOTEnd();
      //      if (debug_seg){
      //	cout << " hits on segment " << (*seg_it)->mdtHitsOnTrack() 
      //	     << " delta hits " << (*seg_it)->deltaHits() << " out of time " << (*seg_it)->outOfTimeHits() 
      //	     << " empty tubes or Holes " << (*seg_it)->emptyTubes() << " chi2/dof " << (*seg_it)->chi2() << std::endl;
      //      }
            //    GLOBAL segment_Phi Vs segment_Eta
            // 
      //    float gdirX = (*seg_it)->globalDirection().x();
      //float gdirY = (*seg_it)->globalDirection().y();
      //float gdirZ = (*seg_it)->globalDirection().z();
      //      float globalPhi = atan2(gdirY,gdirX)* 180./3.14159265;
      //      float globalTheta = acos(gdirZ);
      //      float globalEta = -log(tan(globalTheta/2.));
      
      //double dirY=(*seg_it)->direction().y();
      //double dirZ=(*seg_it)->direction().z();
      //double posY=(*seg_it)->position().y();
      //double posZ=(*seg_it)->position().z();
      //double aseg=0.;
      //double bseg=0.;
      //if (dirZ !=0. ) {
	//aseg=dirY/dirZ;
	//bseg= posY - aseg*posZ;
      //}

      //      if (debug_seg){
      //	cout << " Global Phi, Theta, Eta : "<<globalPhi<<" "<<globalTheta<<" "<<globalEta
      //	     << " ------- Segment slope and intercept (a, b ) : "<<aseg<<" "<<bseg<< endl;
      //      }

      float t0refit = (*seg_it)->fittedT0();
      //      if (debug_seg) cout << " fittedT0 : " << t0refit << endl;
                
      if (t0refit!= 0.) {
	int hitCount(0);
	string chamberName_firstHit;
	int chamberEta_firstHit(0);
	bool sameChamber = true;
	bool adjacentChamber = false;
	int sectorid(0.0);
	for (; aHit_it != aHit_it_end; ++aHit_it){   // start loop over hits
	  hitCount++;
	  MuonFixedId id = (*aHit_it)->identify();
	  MDTName chamb(id);
	  string chamberName = chamb.getOnlineName();
	  if (hitCount==1) { 
	    chamberName_firstHit = chamberName;
	    chamberEta_firstHit = chamb.getOnlineEta();
	    sectorid = chamb.getOnlineSector();
	  } else {
	    if (chamberName!=chamberName_firstHit) {
	      sameChamber = false;
	      if (!(chamberName.substr(0,3)==chamberName_firstHit.substr(0,3) 
		    && chamb.getOnlineSector()==sectorid)) adjacentChamber = false;
	      if (chamberName.substr(0,3)==chamberName_firstHit.substr(0,3) && chamb.getOnlineSector()==sectorid){
		if (chamb.getOnlineEta()==chamberEta_firstHit+1 || chamb.getOnlineEta()==chamberEta_firstHit-1) adjacentChamber = true;
	      }
              
	    }
	  }
	  //	  if (debug_seg) {
	  //	    cout << chamberName << " ML "<< id.mdtMultilayer()<< " Layer "<<id.mdtTubeLayer()
	  //		 <<" tube " << id.mdtTube() << " tube t0 "<< (*aHit_it)->tubeT0() << endl;
	  //	  }
	} // end loop over hits

	if (sameChamber || adjacentChamber)  { // IF all hits are in the same or adjacent Chamber
	  // if (sameChamber && chamberEta_firstHit==2)   // THIS IS TO STUDY WITH MC THE t0 RESOLUTION
	  t0acceptedSeg[nAcceptedSeg]=t0refit;
	  nAcceptedSeg++;
	  if (chamberName_firstHit.substr(0,2) == "BI") {
	    t0refit_BI[nseg_BI][0] = t0refit;
	    t0refit_BI[nseg_BI][1] = sectorid;
	    nseg_BI++;
	  }
	  if (chamberName_firstHit.substr(0,2) == "BM") {
	    t0refit_BM[nseg_BM][0] = t0refit;
	    t0refit_BM[nseg_BM][1] = sectorid;
	    nseg_BM++;
	  }
	  if (chamberName_firstHit.substr(0,2) == "BO") {
	    t0refit_BO[nseg_BO][0] = t0refit;
	    t0refit_BO[nseg_BO][1] = sectorid;
	    nseg_BO++;
	  }
	  if (chamberName_firstHit.substr(0,2) == "EI") {
	    t0refit_EI[nseg_EI][0] = t0refit;
	    t0refit_EI[nseg_EI][1] = sectorid;
	    nseg_EI++;
	  }
	  if (chamberName_firstHit.substr(0,2) == "EM") {
	    t0refit_EM[nseg_EM][0] = t0refit;
	    t0refit_EM[nseg_EM][1] = sectorid;
	    nseg_EM++;
	  }
	  if (chamberName_firstHit.substr(0,2) == "EO") {
	    t0refit_EO[nseg_EO][0] = t0refit;
	    t0refit_EO[nseg_EO][1] = sectorid;
	    nseg_EO++;
	  }
	} // end IF same or adjacent Chamber
      } // end IF t0refit != 0
    } // end IF segment has >= MINNUMHITS
  } // END LOOP ON SEGMENTS

  int sectorWithSegments[17];
  for (int i=0;i<17;i++) sectorWithSegments[i]=0;

  for (int n=0;n<nseg_BI;n++) {
    //    if(debug_seg) cout <<" t0refit_BI, sector " <<t0refit_BI[n][0]<<" "<<t0refit_BI[n][1]<<endl;
    h2_t0BI->Fill(t0refit_BI[n][1],t0refit_BI[n][0]);
    sectorWithSegments[(int)t0refit_BI[n][1]]++;
  }
  for (int n=0;n<nseg_BM;n++) {
    //    if(debug_seg) cout <<" t0refit_BM, sector " <<t0refit_BM[n][0]<<" "<<t0refit_BM[n][1]<<endl;
    h2_t0BM->Fill(t0refit_BM[n][1],t0refit_BM[n][0]);
    sectorWithSegments[(int)t0refit_BM[n][1]]++;
  }
  for (int n=0;n<nseg_BO;n++) {
    //    if(debug_seg) cout <<" t0refit_BO, sector " <<t0refit_BO[n][0]<<" "<<t0refit_BO[n][1]<<endl;
    h2_t0BO->Fill(t0refit_BO[n][1],t0refit_BO[n][0]);
    sectorWithSegments[(int)t0refit_BO[n][1]]++;
  }
  for (int n=0;n<nseg_EI;n++) {
    //    if(debug_seg) cout <<" t0refit_EI, sector " <<t0refit_EI[n][0]<<" "<<t0refit_EI[n][1]<<endl;
    h2_t0EI->Fill(t0refit_EI[n][1],t0refit_EI[n][0]);
    sectorWithSegments[(int)t0refit_EI[n][1]]++;
  }
  for (int n=0;n<nseg_EM;n++) {
    //    if(debug_seg) cout <<" t0refit_EM, sector " <<t0refit_EM[n][0]<<" "<<t0refit_EM[n][1]<<endl;
    h2_t0EM->Fill(t0refit_EM[n][1],t0refit_EM[n][0]);
    sectorWithSegments[(int)t0refit_EM[n][1]]++;
  }
  for (int n=0;n<nseg_EO;n++) {
    //    if(debug_seg) cout <<" t0refit_EO, sector " <<t0refit_EO[n][0]<<" "<<t0refit_EO[n][1]<<endl;
    h2_t0EO->Fill(t0refit_EO[n][1],t0refit_EO[n][0]);
    sectorWithSegments[(int)t0refit_EO[n][1]]++;
  }
  
  int topnSegInSector = 0;
  int sectorWithHighestNseg = 0;
  for (int i=1;i<17;i++) {
    if (sectorWithSegments[i]>topnSegInSector) {
      topnSegInSector=sectorWithSegments[i];
      sectorWithHighestNseg = i;
    }
  }
  //  if(debug_seg) {
  //    cout << " nseg_BI,BM,BO,EI,EM,EO : "<<nseg_BI<<" "<<nseg_BM<<" "<<nseg_BO<<" "
  //	 <<nseg_EI<<" "<<nseg_EM<<" "<<nseg_EO<<endl;
  //    cout << " Sector with Highest Number of Segments : " <<sectorWithHighestNseg
  //	 << " Nsegments : "<<topnSegInSector<< endl;
  //  }
  h2_nSegHitCut->Fill( (float)sectorWithHighestNseg, (float)topnSegInSector );
  if (topnSegInSector>1) {
    // look for combinations BO-BM
    for (int n1=0;n1<nseg_BO;n1++) {
      for (int n2=0;n2<nseg_BM;n2++) {
	if (t0refit_BO[n1][1]==(float)sectorWithHighestNseg && t0refit_BM[n2][1]==(float)sectorWithHighestNseg) {
	  float deltat0_BOBM = t0refit_BO[n1][0]-t0refit_BM[n2][0];
	  //	  if(debug_seg) cout << " Found BO-BM = " << deltat0_BOBM <<endl;
	  h2_t0BOBM->Fill((float)sectorWithHighestNseg,deltat0_BOBM);
	}
      }
    }
    // look for combinations BO-BI
    for (int n1=0;n1<nseg_BO;n1++) {
      for (int n2=0;n2<nseg_BI;n2++) {
	if (t0refit_BO[n1][1]==(float)sectorWithHighestNseg && t0refit_BI[n2][1]==(float)sectorWithHighestNseg) {
	  float deltat0_BOBI = t0refit_BO[n1][0]-t0refit_BI[n2][0];
	  //	  if(debug_seg) cout << " Found BO-BI = " << deltat0_BOBI <<endl ;
	  h2_t0BOBI->Fill((float)sectorWithHighestNseg,deltat0_BOBI);
	}
      }
    }
    // look for combinations BM-BI
    for (int n1=0;n1<nseg_BM;n1++) {
      for (int n2=0;n2<nseg_BI;n2++) {
	if (t0refit_BM[n1][1]==(float)sectorWithHighestNseg && t0refit_BI[n2][1]==(float)sectorWithHighestNseg) {
	  float deltat0_BMBI = t0refit_BM[n1][0]-t0refit_BI[n2][0];
	  //	  if(debug_seg) cout << " Found BM-BI = " << deltat0_BMBI <<endl ;
	  h2_t0BMBI->Fill((float)sectorWithHighestNseg,deltat0_BMBI);
	}
      }
    }

    // look for combinations EO-EM
    for (int n1=0;n1<nseg_EO;n1++) {
      for (int n2=0;n2<nseg_EM;n2++) {
	if (t0refit_EO[n1][1]==(float)sectorWithHighestNseg && t0refit_EM[n2][1]==(float)sectorWithHighestNseg) {
	  float deltat0_EOEM = t0refit_EO[n1][0]-t0refit_EM[n2][0];
	  //	  if(debug_seg) cout << " Found EO-EM = " << deltat0_EOEM <<endl;
	  h2_t0EOEM->Fill((float)sectorWithHighestNseg,deltat0_EOEM);
	}
      }
    }
    // look for combinations EO-EI
    for (int n1=0;n1<nseg_EO;n1++) {
      for (int n2=0;n2<nseg_EI;n2++) {
	if (t0refit_EO[n1][1]==(float)sectorWithHighestNseg && t0refit_EI[n2][1]==(float)sectorWithHighestNseg) {
	  float deltat0_EOEI = t0refit_EO[n1][0]-t0refit_EI[n2][0];
	  //	  if(debug_seg) cout << " Found EO-EI = " << deltat0_EOEI <<endl ;
	  h2_t0EOEI->Fill((float)sectorWithHighestNseg,deltat0_EOEI);
	}
      }
    }
    // look for combinations EM-EI
    for (int n1=0;n1<nseg_EM;n1++) {
      for (int n2=0;n2<nseg_EI;n2++) {
	if (t0refit_EM[n1][1]==(float)sectorWithHighestNseg && t0refit_EI[n2][1]==(float)sectorWithHighestNseg) {
	  float deltat0_EMEI = t0refit_EM[n1][0]-t0refit_EI[n2][0];
	  //	  if(debug_seg) cout << " Found EM-EI = " << deltat0_EMEI <<endl ;
	  h2_t0EMEI->Fill((float)sectorWithHighestNseg,deltat0_EMEI);
	}
      }
    }
  } // CLOSE IF topnSegInSector>1
  
  int iMDTtime = -99;
  if (nAcceptedSeg==0) iMDTtime = -1;
  int nacc=0;
  for (int ns=0; ns<nAcceptedSeg; ns++) {
    if (t0acceptedSeg[ns]>-50. && t0acceptedSeg[ns]<30.) nacc++;
  }
  if (nAcceptedSeg>0 && nacc==nAcceptedSeg) iMDTtime = 1;  // THIS CONDITION is verified if ALL segments are good !
  if (nAcceptedSeg>0 && nacc<nAcceptedSeg) iMDTtime = 0;
  
  // cout << " DEBUG DEBUG iMDTtime in the class  = "<<iMDTtime<<endl;
  
  return iMDTtime;
}  //end MdtDqaTrackAnalysis::MDTtiming

}  //namespace MuonCalib
#endif
