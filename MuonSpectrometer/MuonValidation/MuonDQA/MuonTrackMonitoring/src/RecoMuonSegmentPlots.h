/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONSEGMENTPLOTS_H
#define MUONTRACKMONITORING_RECOMUONSEGMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonHistUtils/MuonSegmentPlots.h"

#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTracking/TrackParticleContainer.h"


namespace Muon {
  class IMuonSegmentTrackBuilder;
}

class RecoMuonSegmentPlots:public PlotBase {
 public:
      RecoMuonSegmentPlots(PlotBase* pParent, std::string sDir);

      //fill methods
      void fill(const xAOD::MuonSegment& muonSeg);
      void fill(std::vector<xAOD::MuonSegment*> selectedSegments, const xAOD::TrackParticleContainer& MSTracks);

      //generic plots from MuonHistUtils
      Muon::MuonSegmentPlots m_oMuonSegmentPlots;

      //additional monitoring plots
      TH1* DQ_delta_phi_segtrk;
      TH1* DQ_delta_eta_segtrk;
      TH1* DQ_delta_R_segtrk;
      TH1* DQ_nGoodSegments;
      TH1* DQ_close_Segments;
      TH1* DQ_close_dir_Segments;
      TH1* DQ_close_pos_Segments;
      std::vector<TH2*> DQ_nSegment_sector_etaIndex;
      std::vector<TH2*> DQ_nGoodSegment_sector_etaIndex;
      std::vector<TH2*> DQ_nBadSegment_sector_etaIndex;
      TH1* DQ_resdiual_Pos_Segments;

 private:
      void initializePlots();
      
      bool goodSegmentQuality(const xAOD::MuonSegment* muonSeg);//check if it is a good segment
      bool matchPairSegment_dir( const Amg::Vector3D& globalDir1, const Amg::Vector3D& globalDir2);// see if two segments are close by in directions
      bool matchPairSegment_pos( const Amg::Vector3D& globalPos1, const Amg::Vector3D& globalPos2);// see if two segments are close by in positions
      void removeOverlapSegments(std::vector<xAOD::MuonSegment*> &muonSegments);

      //useful constants
      const float myPi = 3.14159;
      const char *StationName[8] = {"BI", "BM", "BO", "BE", "EI", "EM", "EO", "EE"};//For filling in monitoring plots

      float DeltaR(float eta1, float eta2, float phi1, float phi2) {  return sqrt(pow(eta1 - eta2, 2) + pow(phi1 - phi2, 2)); }

      /** MDT segment specific cuts for barrel*/
      const double m_bMdtchi2max = 12.0;
      const int    m_bMdtnhitsmin = 5;
      /** MDT segment specific cuts for endcap*/
      const double m_eMdtchi2max = 12.0;
      const int    m_eMdtnhitsmin = 5;
      /** CSC segment specific cuts for endcap*/
      const double m_eCscchi2max = 12.0;//used to be 20
      const int    m_eCscnhitsmin = 4;

      /** Requirements on segment pairs */
      const double m_mdtdelthetamax = 0.05;//eta direction in rads
      const double m_mdtdelphimax = 0.10; //phi direction in rads
      const double m_mdtdelzmax = 100.0;//position diff in z
      const double m_mdtdelrmax = 100.0;//position diff in r

};

#endif
