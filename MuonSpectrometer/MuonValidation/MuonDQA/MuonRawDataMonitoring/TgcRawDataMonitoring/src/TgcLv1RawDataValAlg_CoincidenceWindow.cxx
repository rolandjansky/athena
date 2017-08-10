/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// GeoModel
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TMath.h>
#include <inttypes.h> 

#include <sstream>

using namespace std;


///////////////////////////////////////////////////////////////////////////
// bookCoincidenceWindowHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsCoincidenceWindow(){

  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";

  MonGroup tgclv1_cw_a( this, generic_path_tgclv1+"/TGCEA/CW", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_cw_c( this, generic_path_tgclv1+"/TGCEC/CW", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_cw_ac[2] = { &tgclv1_cw_a, &tgclv1_cw_c};

  std::stringstream ss;
  std::string side[2]={"A","C"};
  std::string muid[1]={"Muid"};

  
  ///////////////////////////////////////////////////////////////////////////
  // Turn on curves for pT1-6
  for(int ac=0;ac<2;ac++){// side
    for(int mod=0;mod<9;mod++){// I have no idea what mod is, it appears to just be a way of indexing CW histograms
    // Set RoI string
      std::string roi="5";
      if((mod==2)||(mod==5)||(mod==8)) roi="57";
      for(int pt=0;pt<6;pt++){
        // Coincidence Window for SL triggers for current pT threshold
        ss.str(""); ss << "CW_Mod"<< mod << "_RoI" << roi << "_PT"<< pt+1 << "_" << side[ac] ;
        m_tgclv1cw[ac][mod][pt] = new TH2F(ss.str().c_str(), (ss.str() + ";d#phi;dR").c_str(), 15,  -7, 8, 31, -15, 16);
        ATH_CHECK( tgclv1_cw_ac[ac]->regHist(m_tgclv1cw[ac][mod][pt]) );
        // Coincidence Window for SL triggers compared with Offline Muons
        for(int m=0;m<m_nMuonAlgorithms;m++){
          // CW for SL triggers matched with offline muons above current pT threshold
          ss.str(""); ss << "CW_Mod"<< mod << "_RoI" << roi << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] ;
          m_tgclv1cwoffline[ac][mod][pt][m] = new TH2F(ss.str().c_str(), (ss.str() + ";d#phi;dR").c_str(), 15,  -7, 8, 31, -15, 16);
          ATH_CHECK( tgclv1_cw_ac[ac]->regHist(m_tgclv1cwoffline[ac][mod][pt][m]) );
          
          // CW for SL triggers matched with offline muons below current pT threshold
          ss.str(""); ss << "!CW_Mod"<< mod << "_RoI" << roi << "_PT"<< pt+1 << "_" << muid[m] <<  "_" << side[ac] ;
          m_tgclv1cwrejectedoffline[ac][mod][pt][m] = new TH2F(ss.str().c_str(), (ss.str() + ";d#phi;dR").c_str(), 15,  -7, 8, 31, -15, 16);
          ATH_CHECK( tgclv1_cw_ac[ac]->regHist(m_tgclv1cwrejectedoffline[ac][mod][pt][m]) );
        }// muon algorithm
      }// pt
    }// mod
  }// ac

  return StatusCode::SUCCESS;
}// EOF


///////////////////////////////////////////////////////////////////////////
// fillCoincidenceWindow
///////////////////////////////////////////////////////////////////////////
void
TgcLv1RawDataValAlg::fillCoincidenceWindow(int ms,
                                           vector<float>* mu_pt, vector<float>* mu_eta,
                                           vector<float>* mu_phi,vector<float>* mu_q){
  // Set delta cut used to associate Offline muon with SL trigger
  const float deltarcut = 0.25;
  // Get number of Offline Muons 
  int osize = mu_pt->size();
  
  // Check if any Offline Muons were found close to each other
  bool smalltrackdr=false;
  for(int o1=0;o1<osize;o1++){
    for(int o2=o1+1;o2<osize;o2++){
      float eta1 = mu_eta->at(o1);
      float phi1 = mu_phi->at(o1);
      float eta2 = mu_eta->at(o2);
      float phi2 = mu_phi->at(o2);
      float trackdr = deltaR(eta1, phi1, eta2, phi2);

      if(trackdr < 0.8 ) smalltrackdr=true;
    }
  }

  // Loop over HpT Wires
  for(int ihptw=0;ihptw<(int)m_hpt_delta[0][1].size();ihptw++){

    // Get HpT Wire Variables
    int deltaw      = m_hpt_delta[0][1].at(ihptw);
    int tidw        = m_hpt_tid[0][1].at(ihptw);
    int subw        = m_hpt_sub[0][1].at(ihptw);
    int phi48w      = m_hpt_phi48[0][1].at(ihptw);
    bool isForwardw = m_hpt_isForward[0][1].at(ihptw);
    bool isAsidew   = m_hpt_isAside[0][1].at(ihptw);

    // Loop over HpT Strips
    for(int ihpts=0;ihpts<(int)m_hpt_delta[1][1].size();ihpts++){

      // Get HpT Strip Variables
      int deltas      = m_hpt_delta[1][1].at(ihpts);
      int tids        = m_hpt_tid[1][1].at(ihpts);
      int subs        = m_hpt_sub[1][1].at(ihpts);
      int phi48s      = m_hpt_phi48[1][1].at(ihpts);
      bool isForwards = m_hpt_isForward[1][1].at(ihpts);
      bool isAsides   = m_hpt_isAside[1][1].at(ihpts);

      // Check if Wire and Strip are in same sector
      if( phi48w!=phi48s ||
          isAsidew != isAsides ||
          isForwardw != isForwards ) continue;
      
      // Get RoI
      int roihpt = trackletIdSub2RoI(tidw,subw,tids,subs,isForwardw,phi48w);
        
      // Loop over SL
      for(int sl=0;sl<(int)m_sl_pt[1].size();sl++){

        // Get SL Variables
        int pt         = m_sl_pt[1].at(sl);
        int roi        = m_sl_roi[1].at(sl);
        bool isAside   = m_sl_isAside[1].at(sl);
        bool isForward = m_sl_isForward[1].at(sl);
        float sleta    = m_sl_eta[1].at(sl);
        float slphi    = m_sl_phi[1].at(sl);
        int sltidw     = m_sl_tidw[1].at(sl);
        int sltids     = m_sl_tids[1].at(sl);
        int slphi48    = m_sl_phi48[1].at(sl);

        // Check if SL is in same sector, with same tracklet id and RoI as Wire&Strip
        if( slphi48   != phi48w ||
            isAside   != isAsidew ||
            isForward != isForwardw ||
            sltidw    != tidw ||
            sltids    != tids ||
            roi       != roihpt ) continue;

        // Currently only monitoring RoI=5  for Endcap sectors
        //           Also monitoring RoI=57 for Forward sectors
        //           phi indexing starts at A01 phi0
        int mod=0;// I have no idea what mod is, it appears to just be a way of indexing CW histograms
        if(isForward){
          // Only monitor RoI=57 for Forward
          if( roi!=57 ) continue;
          int phimod = slphi48+1;
          if(phimod>23) phimod-=24;//[0:23]
          mod = 3*(phimod%3) + 2; //A01 phi0=2, phi2=5, A02 phi0=8, phi2=2,...
        }
        else{
          // Only monitor RoI=5 for Endcap
          if( roi!=5 ) continue;
          int phimod = slphi48+2;
          if(phimod>47) phimod-=48;//[0:47]
          mod = 1.5*(phimod%6); //A01 phi0=0, phi1=1, phi2=3, phi3=4, A02 phi0=6, phi1=7, phi2=0....
        }

        // If ms (function argument)==0 fill general CW histogram
        if(ms==0){
          m_tgclv1cw[isAside==false][mod][pt-1]->Fill(deltas, deltaw);
        }

        // If no tracks were found close together loop over offline muons
        if(!smalltrackdr){
          for(int o=0;o<osize;o++){
            // Get Offline Muon Variables
            float opt  = mu_pt->at(o)/CLHEP::GeV * mu_q->at(o);
            float oeta = mu_eta->at(o);
            float ophi = mu_phi->at(o);
            
            // Check if Offline Muon is on the same side as preceding Coincidences
            int ac = (oeta<0);
            bool isAside = (oeta>0);
            if( isAside != isAsidew ) continue;
            
            // Check if Offline Muon is close enough to SL Coincidence
            float dr = deltaR(oeta, ophi, sleta, slphi);
            if(dr>deltarcut) continue;
            
            // Find pT Trigger Level the Offline Muon should trigger
            int optthres = 0;
            if     (opt>20.) optthres = 6;
            else if(opt>15.) optthres = 5;
            else if(opt>10.) optthres = 3;
            else if(opt> 6.) optthres = 2;
            else if(opt> 4.) optthres = 1;
            
            // Loop over Trigger Levels
            for(int ipt=0;ipt<6;ipt++){// ipt
              // Fill histogram based on whether the Offline Trigger Level is greater
              // than currently looping trigger level
              if(optthres>ipt)
                m_tgclv1cwoffline[ac][mod][ipt][ms]->Fill(deltas, deltaw);
              else
                m_tgclv1cwrejectedoffline[ac][mod][ipt][ms]->Fill(deltas, deltaw);
            }// ipt
          }// offline muons
  
        }// if no offline muons close together
      }// SL
    }// HpT strip
  }// HpT wire
}// EOF

