/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcLv1RawDataValAlg
// Authors:  A.Ishikawa(Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

//offline 
#include "muonEvent/MuonContainer.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include <inttypes.h> 
#include <TProfile.h>

#include <sstream>

///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg Constructor
///////////////////////////////////////////////////////////////////////////
TgcLv1RawDataValAlg::TgcLv1RawDataValAlg(const std::string &type, const std::string &name, const IInterface* parent)
  :ManagedMonitorToolBase(type, name, parent)
{
  declareProperty("UseExpressStream",m_useExpressStream=false);
  // initialize the histos
  for(int ac=0;ac<2;ac++){
	  m_tgclv1lpttiming[ac] = 0;
	  m_tgclv1lpttimingptcut[ac] = 0;
	  m_tgclv1lpttimingmap[ac] = 0;
	  m_tgclv1lptcurrentfractionmap[ac] = 0;
	  m_tgclv1lpttimingmapptcut[ac] = 0;
	  m_tgclv1lptcurrentfractionmapptcut[ac] = 0;
	  m_tgclv1slvslpttiming[ac] = 0;
	  m_tgclv1slvslpttimingptcut[ac] = 0;
	  m_tgclv1_SL_trigger_timing_ES[ac] = 0;
	  m_tgclv1_SL_trigger_timing_num_ES[ac] = 0;
	  m_tgclv1_SL_trigger_timing_denom_ES[ac] = 0;
	  m_tgclv1_LPT_trigger_timing_ES[ac] = 0;
	  m_tgclv1_LPT_trigger_timing_num_ES[ac] = 0;
	  m_tgclv1_LPT_trigger_timing_denom_ES[ac] = 0;
	  m_tgclv1_SL_trigger_timing_ES_GM[ac] = 0;
	  m_tgclv1_LPT_trigger_timing_ES_GM[ac] = 0;
	  m_tgclv1_plateau_eff_counting_ES[ac] = 0;
	  m_tgclv1_plateau_eff_counting_num_ES[ac] = 0;
	  m_tgclv1_plateau_eff_counting_denom_ES[ac] = 0;
	  m_tgclv1_plateau_eff_counting_ES_GM[ac] = 0;
	  m_tgclv1summaryofsltiming[ac] = 0;
	  m_tgclv1summaryoflpttiming[ac] = 0;
	  
	  m_tgclv1slchamberlowstat[ac] = 0;
	  m_tgclv1sltimingptcutlowstat[ac] = 0;

	  m_tgclv1sltiming[ac] = 0;
	  m_tgclv1sltimingptcut[ac] = 0;
	  m_tgclv1sltimingmap[ac] = 0;
	  m_tgclv1slcurrentfractionmap[ac] = 0;
	  m_tgclv1sltimingmapptcut[ac] = 0;
	  m_tgclv1slcurrentfractionmapptcut[ac] = 0;

	  for(int isec=0; isec<12; isec++){
	    m_tgclv1sltimingsector[ac][isec] = 0;
	    m_tgclv1sltimingptcutsector[ac][isec] = 0;
	    m_tgclv1lpttimingsector[ac][isec] = 0;
	    m_tgclv1lpttimingptcutsector[ac][isec] = 0;
	    m_tgclv1slvslpttimingsector[ac][isec] = 0;
	    m_tgclv1slvslpttimingptcutsector[ac][isec] = 0;
	    for(int itrig=0; itrig<4; itrig++){
		m_tgclv1sltimingsectortrg[ac][isec][itrig] = 0;
		m_tgclv1sltimingptcutsectortrg[ac][isec][itrig] = 0;
		m_tgclv1lpttimingsectortrg[ac][isec][itrig] = 0;
		m_tgclv1lpttimingptcutsectortrg[ac][isec][itrig] = 0;
		m_tgclv1slvslpttimingsectortrg[ac][isec][itrig] = 0;
		m_tgclv1slvslpttimingptcutsectortrg[ac][isec][itrig] = 0;
	    }
	  }
	  for(int itrig=0; itrig<4; itrig++){
	    m_tgclv1sltimingtrg[ac][itrig] = 0;
	    m_tgclv1sltimingptcuttrg[ac][itrig] = 0;
	    m_tgclv1lpttimingtrg[ac][itrig] = 0;
	    m_tgclv1lpttimingptcuttrg[ac][itrig] = 0;
	  }
	  for(int im=0; im<2; im++){
	    m_tgclv1sltimingtrack[ac][im] = 0;
	    m_tgclv1sltimingptcuttrack[ac][im] = 0;
	    m_tgclv1lpttimingtrack[ac][im] = 0;
	    m_tgclv1lpttimingptcuttrack[ac][im] = 0;
	  }

	  for(int ipt=0; ipt<6; ipt++){
	    m_tgclv1turnon_ES[ac][ipt] = 0;
	    m_tgclv1turnontg_ES[ac][ipt] = 0;
	    m_tgclv1turnonnum_ES[ac][ipt] = 0;
	    m_tgclv1turnondenom_ES[ac][ipt] = 0;

	    m_tgclv1sltimingpt[ac][ipt] = 0;
	    m_tgclv1lpttimingpt[ac][ipt] = 0;
	    for(int itrig=0; itrig<4; itrig++){
		m_tgclv1sltimingpttrg[ac][ipt][itrig] = 0;
		m_tgclv1lpttimingpttrg[ac][ipt][itrig] = 0;
	    }
	    for(int im=0; im<2; im++)
		m_tgclv1deltar[ac][ipt][im] = 0;
	    for(int icharge=0; icharge<3; icharge++){
	       for(int imuid=0; imuid<2; imuid++){
		  m_tgclv1turnon[ac][ipt][icharge][imuid] = 0;
		  m_tgclv1turnontg[ac][ipt][icharge][imuid] = 0;
		  m_tgclv1turnonnum[ac][ipt][icharge][imuid] = 0;
		  m_tgclv1turnondenom[ac][ipt][icharge][imuid] = 0;
		  m_tgclv1effetavsphidenom[ac][ipt][icharge][imuid] = 0;
	          for(int ipcn=0; ipcn<3; ipcn++){
		     m_tgclv1effetavsphi[ac][ipt][icharge][imuid][ipcn] = 0;
		     m_tgclv1effetavsphinum[ac][ipt][icharge][imuid][ipcn] = 0;
		  }
	       }
	    }
	  }
          for(int mod=0;mod<9;mod++)
            for(int pt=0;pt<6;pt++){
              m_tgclv1cw[ac][mod][pt] = 0;
              for(int m=0;m<2;m++){
                m_tgclv1cwoffline[ac][mod][pt][m] = 0;
                m_tgclv1cwrejectedoffline[ac][mod][pt][m] = 0;
              }
            }

	
  }
  for(int ich=0; ich<6; ich++){
	  m_tgclv1summaryofsltimingperchambertype[ich] = 0;
	  m_tgclv1summaryoflpttimingperchambertype[ich] = 0;
  }

} 

///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg initialize
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_MSG_INFO( "in TgcLv1RawDataValAlg initialize"  );

  // Set number of Muon Algorithms to use, 1:muid, 2:muid&staco
  m_nMuonAlgorithms=1;
  if(m_environment==AthenaMonManager::online) m_nMuonAlgorithms=1;
  ATH_CHECK(m_coinCollectionLocation.initialize());
  ATH_CHECK(m_coinCollectionLocationPrevious.initialize());
  ATH_CHECK(m_coinCollectionLocationNext.initialize());
  ATH_CHECK(m_muonKey.initialize());
  ATH_CHECK(m_L1muonRoIName.initialize());
  ATH_CHECK(m_L1emtauRoIName.initialize());
  ATH_CHECK(m_L1jetRoIName.initialize());
  ATH_CHECK(m_L1esumRoIName.initialize());
  ATH_CHECK(m_eventInfo.initialize());
  ATH_CHECK(m_trigOpInfo.initialize(m_useExpressStream));
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg bookHistograms
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsRecurrent(){
  ATH_MSG_DEBUG( "TGCLV1 RawData Monitoring Histograms being booked"  );
  
  // Book histograms per 10LBs 
  if( newRunFlag() || newLowStatIntervalFlag() ){
    // not monitor these profiles at GM
    if( m_environment != AthenaMonManager::online ){
      ATH_CHECK(  bookHistogramsLowStat() );
    }
    else{
      for(int ac=0;ac<2;ac++){
        m_tgclv1slchamberlowstat[ac] = 0;
        m_tgclv1sltimingptcutlowstat[ac] = 0;
      }
    }
  }// newLowStatInterval
  
  // Book histograms per ... (not used yet, but commenting them out causes "unused" warnings)
  if(newLumiBlockFlag()){}
  
  
  // Book histograms per Run
  if(newRunFlag()){
    // Book Number Of Trigger And Profile histograms
    ATH_CHECK( bookHistogramsNumberOfTriggersAndProfile() );

    // Book Number Of Trigger Rate histograms
    ATH_CHECK( bookHistogramsTriggerRate() );

    // Book Timing histograms
    ATH_CHECK( bookHistogramsTiming() );

    // Book Turn-on curves and efficiency maps
    ATH_CHECK( bookHistogramsEfficiency() );

    // not monitor these profiles at GM
    if( m_environment != AthenaMonManager::online ){
      // Book Coincidence Windows
      ATH_CHECK( bookHistogramsCoincidenceWindow() );
    }
    else
      for(int ac=0;ac<2;ac++)
        for(int mod=0;mod<9;mod++)
          for(int pt=0;pt<6;pt++){
            m_tgclv1cw[ac][mod][pt] = 0;
            for(int m=0;m<m_nMuonAlgorithms;m++){
              m_tgclv1cwoffline[ac][mod][pt][m] = 0;
              m_tgclv1cwrejectedoffline[ac][mod][pt][m] = 0;
            }
          }

    // Book Summary histograms
    ATH_CHECK( bookHistogramsSummary() );
  }// newRun

  ATH_MSG_DEBUG( "TGCLV1 RawData Monitoring : histo booked "  );

  return StatusCode::SUCCESS;
}// EOF


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg fillHistograms
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::fillHistograms(){
  ATH_MSG_DEBUG( "TgcLv1RawDataValAlg::TGCLV1 RawData Monitoring Histograms being filled"  );
  
  ///////////////////////////////////////////////////////////////////////////
  // Get Event Data
  /////////////////////////////////////
  // Read Event Info
  if( readEventInfo().isFailure() ){
    ATH_MSG_WARNING( "no event info container"  );
  }
  ATH_MSG_DEBUG("Read EventInfo");
  // Read L1 Trigger Data
  ATH_CHECK(  readL1TriggerType() );
  ATH_MSG_DEBUG("readL1TriggerType");
  
  // Get Data from TGC Containers
  clearVectorsArrays();
  ATH_MSG_DEBUG("cleared vectors and arrays");

  /////////////////////////////////////
  // Get TGC Coin Containers
  // Previous
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgc_previous_coin_container(m_coinCollectionLocationPrevious);
  if(tgc_previous_coin_container.isValid() && tgc_previous_coin_container.isPresent()){
    ATH_MSG_DEBUG( "****** tgc previous coin container size() : " << tgc_previous_coin_container->size()  );
    // fill vectors and arrays from TgcCoinData
    readTgcCoinDataContainer(tgc_previous_coin_container.cptr(), PREV);
  }

  // Current
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgc_current_coin_container(m_coinCollectionLocation);
  if(tgc_current_coin_container.isValid() && tgc_current_coin_container.isPresent()){
    ATH_MSG_DEBUG( "****** tgc current coin container size() : " << tgc_current_coin_container->size()  );
    // fill vectors and arrays from TgcCoinData
    readTgcCoinDataContainer( tgc_current_coin_container.cptr(), CURR);
  }

  // Next
  SG::ReadHandle<Muon::TgcCoinDataContainer> tgc_next_coin_container(m_coinCollectionLocationNext);
  if(tgc_next_coin_container.isValid() && tgc_next_coin_container.isPresent()){
    ATH_MSG_DEBUG( "****** tgc next coin container size() : " << tgc_next_coin_container->size()  );
    // fill vectors and arrays from TgcCoinData
    readTgcCoinDataContainer(    tgc_next_coin_container.cptr(), NEXT);
    readTgcCoinDataContainer(    tgc_next_coin_container.cptr(), TOTA);
  }

  ///////////////////////////////////////////////////////////////////////////
  // ReadContainer
  /////////////////////////////////////
  // Read Offline Muon Containers
  ATH_CHECK( readOfflineMuonContainer( &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q) );
  ATH_MSG_DEBUG("read offline muon container");

  ///////////////////////////////////////////////////////////////////////////
  // Fill Histograms
  
  // NumberOfTrigger
  fillNumberOfTrigger();
  ATH_MSG_DEBUG("filled # of triggers");
  
  // TriggerTiming
  int ptcut=1;
  fillTriggerTiming(ptcut);
  ATH_MSG_DEBUG("filled trigger timing");
  
  int ms;
  ms=0; //Muid
  fillTriggerTimingAssociatedWithTrack( ms, &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q );
  ATH_MSG_DEBUG("Associated with track");
  
  // Efficiency
  ms=0; //Muid
  fillEfficiency( ms, &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q );
  ATH_MSG_DEBUG("filled efficiency");
  
  
  // Coincidence Window
  // not monitor these profiles at GM
  if( m_environment != AthenaMonManager::online ){
    fillCoincidenceWindow( ms, &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q );
  }
  ATH_MSG_DEBUG("filled coincidence window");
  
  /*
    int numberOfSL=m_nSL[0]+m_nSL[1]+m_nSL[2];
    // check hit Timing if single RoI
    if (m_debuglevel) m_log << MSG::DEBUG << "number of SL " << numberOfSL << endmsg;
    if(numberOfSL!=1) return StatusCode::SUCCESS;//not single RoI
  */
  
  return StatusCode::SUCCESS;  // statuscode check  
}// EOF


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg procHistograms
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::procHistograms(){
  ATH_MSG_DEBUG( "TgcLv1RawDataValAlg finalize()"  );
  
  // Process histograms per LumiBlock
  if(endOfLumiBlockFlag()){

    // Efficiency for Global Monitoring
    if( m_environment == AthenaMonManager::online ) {
      // express stream only
      for(int ac=0;ac<2;ac++){
        for(int pt=0;pt<6;pt++) {
          m_tgclv1turnon_ES[ac][pt]  ->Divide( m_tgclv1turnonnum_ES[ac][pt], m_tgclv1turnondenom_ES[ac][pt], 1, 1, "B" );
         if(m_tgclv1turnondenom_ES[ac][pt]->Integral()>0.) m_tgclv1turnontg_ES[ac][pt]->BayesDivide( m_tgclv1turnonnum_ES[ac][pt], m_tgclv1turnondenom_ES[ac][pt] );
        }//pt

        m_tgclv1_plateau_eff_counting_ES[ac]   ->Divide( m_tgclv1_plateau_eff_counting_num_ES[ac], m_tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );
        m_tgclv1_plateau_eff_counting_ES_GM[ac]->Divide( m_tgclv1_plateau_eff_counting_num_ES[ac], m_tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );

        m_tgclv1_SL_trigger_timing_ES[ac] ->Divide( m_tgclv1_SL_trigger_timing_num_ES[ac],  m_tgclv1_SL_trigger_timing_denom_ES[ac]  );
        m_tgclv1_LPT_trigger_timing_ES[ac]->Divide( m_tgclv1_LPT_trigger_timing_num_ES[ac], m_tgclv1_LPT_trigger_timing_denom_ES[ac] );

        m_tgclv1_SL_trigger_timing_ES_GM[ac] ->Divide( m_tgclv1_SL_trigger_timing_num_ES[ac],  m_tgclv1_SL_trigger_timing_denom_ES[ac]  );
        m_tgclv1_LPT_trigger_timing_ES_GM[ac]->Divide( m_tgclv1_LPT_trigger_timing_num_ES[ac], m_tgclv1_LPT_trigger_timing_denom_ES[ac] );

      }//ac
    }
  }
  // Process histograms per Run
  if(endOfRunFlag()){
    ///////////////////////////////////////////////////////////////////////////
    // TriggerTiming
    // SL/LPT timing
    for(int ac=0;ac<2;ac++){// side
      for(int ieta=0;ieta<6;ieta++){// stationEta
        // Get eta bin indexes for different timing
        int etabinp = ieta;
        int etabinc = ieta + 6;
        int etabinn = ieta + 12;

        for(int phi48=1;phi48<49;phi48++){// stationPhi
          float   p,   c,   n, tot;
          float  fp,  fc,  fn;
          float efp, efc, efn;

          /////////////////////////////////////
          // SL Timing Map
          // Get Total number of triggers for this chamber
          p = m_tgclv1sltimingmap[ac]->GetBinContent(etabinp +1, phi48);
          c = m_tgclv1sltimingmap[ac]->GetBinContent(etabinc +1, phi48);
          n = m_tgclv1sltimingmap[ac]->GetBinContent(etabinn +1, phi48);
          tot = p+c+n;

          // Get Fractions and Errors
          fp=fc=fn=efp=efc=efn=0;
          if( tot != 0 ){
            fp= p/tot; efp = std::sqrt( fp*(1.-fp)/tot );
            fc= c/tot; efc = std::sqrt( fc*(1.-fc)/tot );
            fn= n/tot; efn = std::sqrt( fn*(1.-fn)/tot );
          }
          // Fill Fractions into map
          m_tgclv1slcurrentfractionmap[ac]->SetBinContent(etabinp +1,  phi48, fp);
          m_tgclv1slcurrentfractionmap[ac]->SetBinContent(etabinc +1,  phi48, fc);
          m_tgclv1slcurrentfractionmap[ac]->SetBinContent(etabinn +1,  phi48, fn);
          m_tgclv1slcurrentfractionmap[ac]->SetBinError(etabinp +1,  phi48, efp);
          m_tgclv1slcurrentfractionmap[ac]->SetBinError(etabinc +1,  phi48, efc);
          m_tgclv1slcurrentfractionmap[ac]->SetBinError(etabinn +1,  phi48, efn);
          
          /////////////////////////////////////
          // SL Timing Map
          // Get Total number of triggers for this chamber
          p = m_tgclv1lpttimingmap[ac]->GetBinContent(etabinp +1, phi48);
          c = m_tgclv1lpttimingmap[ac]->GetBinContent(etabinc +1, phi48);
          n = m_tgclv1lpttimingmap[ac]->GetBinContent(etabinn +1, phi48);
          tot = p+c+n;

          // Get Fractions and Errors
          fp=fc=fn=efp=efc=efn=0;
          if( tot != 0 ){
            fp= p/tot; efp = std::sqrt( fp*(1.-fp)/tot );
            fc= c/tot; efc = std::sqrt( fc*(1.-fc)/tot );
            fn= n/tot; efn = std::sqrt( fn*(1.-fn)/tot );
          }
          // Fill Fractions into map
          m_tgclv1lptcurrentfractionmap[ac]->SetBinContent(etabinp +1,  phi48, fp);
          m_tgclv1lptcurrentfractionmap[ac]->SetBinContent(etabinc+1,  phi48, fc);
          m_tgclv1lptcurrentfractionmap[ac]->SetBinContent(etabinn+1,  phi48, fn);
          m_tgclv1lptcurrentfractionmap[ac]->SetBinError(etabinp +1,  phi48, efp);
          m_tgclv1lptcurrentfractionmap[ac]->SetBinError(etabinc+1,  phi48, efc);
          m_tgclv1lptcurrentfractionmap[ac]->SetBinError(etabinn+1,  phi48, efn);

        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // NumberOfTrigger
    for(int ac=0;ac<2;ac++){
      m_tgclv1slpereventlb[ac]->Sumw2();
      m_tgclv1slpereventlb[ac]->Divide( m_tgclv1slinlb[ac], m_tgclv1eventsinlb );

      m_tgclv1slpereventbcid[ac]->Sumw2();
      m_tgclv1slpereventbcid[ac]->Divide( m_tgclv1slinbcid[ac], m_tgclv1eventsinbcid );

      for(int pt=0;pt<6;pt++){
        m_tgclv1slpereventlbpt[ac][pt]->Sumw2();
        m_tgclv1slpereventlbpt[ac][pt]->Divide( m_tgclv1slinlbpt[ac][pt], m_tgclv1eventsinlb );

        m_tgclv1slpereventbcidpt[ac][pt]->Sumw2();
        m_tgclv1slpereventbcidpt[ac][pt]->Divide( m_tgclv1slinbcidpt[ac][pt], m_tgclv1eventsinbcid );
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Efficiency
    for(int ac=0;ac<2;ac++){// side
      for(int ipt=0;ipt<6;ipt++){// pt
        for(int mu=0;mu<m_nMuonAlgorithms;mu++){// muon algorithm
          for(int pna=0;pna<3;pna++){// charge
            // Fill histograms for Both charges
            if(pna==2){
              for(int pcn=0;pcn<3;pcn++){// pcn
                m_tgclv1effetavsphinum[ac][ipt][pna][mu][pcn]->Add(m_tgclv1effetavsphinum[ac][ipt][0][mu][pcn], m_tgclv1effetavsphinum[ac][ipt][1][mu][pcn]);
              }
              m_tgclv1effetavsphidenom[ac][ipt][pna][mu]->Add(m_tgclv1effetavsphidenom[ac][ipt][0][mu], m_tgclv1effetavsphidenom[ac][ipt][1][mu]);
              m_tgclv1turnonnum[ac][ipt][pna][mu]       ->Add(m_tgclv1turnonnum[ac][ipt][0][mu],        m_tgclv1turnonnum[ac][ipt][1][mu]);
              m_tgclv1turnondenom[ac][ipt][pna][mu]     ->Add(m_tgclv1turnondenom[ac][ipt][0][mu],      m_tgclv1turnondenom[ac][ipt][1][mu]);
            }

            // Calculate Efficiencies
            for(int pcn=0;pcn<3;pcn++){// pcn
              //m_tgclv1turnon[ac][ipt][mu]->Sumw2();
              m_tgclv1turnon[ac][ipt][pna][mu]->Divide(m_tgclv1turnonnum[ac][ipt][pna][mu], m_tgclv1turnondenom[ac][ipt][pna][mu], 1, 1, "B");
              if(m_tgclv1turnondenom[ac][ipt][pna][mu]->Integral()>0.)m_tgclv1turnontg[ac][ipt][pna][mu]->BayesDivide(m_tgclv1turnonnum[ac][ipt][pna][mu], m_tgclv1turnondenom[ac][ipt][pna][mu]);

              //m_tgclv1effetavsphi[ac][ipt][mu]->Sumw2();
              m_tgclv1effetavsphi[ac][ipt][pna][mu][pcn]->Divide(m_tgclv1effetavsphinum[ac][ipt][pna][mu][pcn], m_tgclv1effetavsphidenom[ac][ipt][pna][mu], 1, 1, "B");
            }// pcn
          }// charge
        }// mu
      }// pt

      /////////////////////////////////////
      // express stream only
      for(int ipt=0;ipt<6;ipt++){// pt
        //m_tgclv1turnon_ES[ac][pt][mu]->Sumw2();
        m_tgclv1turnon_ES[ac][ipt]->Divide( m_tgclv1turnonnum_ES[ac][ipt], m_tgclv1turnondenom_ES[ac][ipt], 1, 1, "B" );
        if((m_tgclv1turnondenom_ES[ac][ipt]->Integral())>0.)m_tgclv1turnontg_ES[ac][ipt]->BayesDivide( m_tgclv1turnonnum_ES[ac][ipt], m_tgclv1turnondenom_ES[ac][ipt] );
      }// pt

      m_tgclv1_plateau_eff_counting_ES[ac]->Divide( m_tgclv1_plateau_eff_counting_num_ES[ac], m_tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );
      m_tgclv1_plateau_eff_counting_ES_GM[ac]->Divide( m_tgclv1_plateau_eff_counting_num_ES[ac], m_tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );

      m_tgclv1_SL_trigger_timing_ES[ac] -> Divide( m_tgclv1_SL_trigger_timing_num_ES[ac], m_tgclv1_SL_trigger_timing_denom_ES[ac] );
      m_tgclv1_LPT_trigger_timing_ES[ac] -> Divide( m_tgclv1_LPT_trigger_timing_num_ES[ac], m_tgclv1_LPT_trigger_timing_denom_ES[ac] );
      m_tgclv1_SL_trigger_timing_ES_GM[ac] -> Divide( m_tgclv1_SL_trigger_timing_num_ES[ac], m_tgclv1_SL_trigger_timing_denom_ES[ac] );
      m_tgclv1_LPT_trigger_timing_ES_GM[ac] -> Divide( m_tgclv1_LPT_trigger_timing_num_ES[ac], m_tgclv1_LPT_trigger_timing_denom_ES[ac] );
    }// side

  } //endOfRun

  return StatusCode::SUCCESS;
}



