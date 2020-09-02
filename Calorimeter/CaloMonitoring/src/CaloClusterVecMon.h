/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloClusterVecMon.h
//
// AUTHORS:   R. Kehoe    06/15/2006
//            S.Chekanov (TileCal),  08/04/2008 
// MANAGER:   H. Hadavand   
//            R. Dhullipudi (01/07/2008) 
//            D. Hu (Jun 2011 - May 2012)
//            Jun Guo, L. Sawyer (2013 - )
// ********************************************************************

#ifndef CALOClusterVecMon_H
#define CALOClusterVecMon_H

#include "CaloMonToolBase.h"

#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/TileID.h"

#include "TH1D.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <functional>
#include <set>

class CaloClusterVecMon : public CaloMonToolBase {
 public:

  CaloClusterVecMon(const std::string & type, const std::string& name, const IInterface* parent);
   virtual ~CaloClusterVecMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode bookHistograms();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();

 private:
 
   void initHists();

   StatusCode retrieveTools();

   bool checkTimeGran(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, Interval_t& theinterval); 
   void bookCellHists(const Interval_t theinterval);
   void bookClusterHists(const Interval_t theinterval);
   void bookClusterStatHists(const Interval_t theinterval); 
   void fillTileHistRange();
   void bookTileHists(const Interval_t theinterval);

   void initCounter();
   void fillCellHist(const xAOD::CaloCluster* clus);
   void fillClusterEMvar(const xAOD::CaloCluster* clus);
   void fillClusterStat(const xAOD::CaloCluster* clus);
   void fillClusterHist(const xAOD::CaloCluster* clus); 
   void fillClusterStatHist(const xAOD::CaloClusterContainer* clusterCont); 
   void fillTileHist(const xAOD::CaloClusterContainer* clusterCont);

   enum EthreshLvl {		// energy threshold levels for histos
      LOW_E = 0,
      LOWMED_E = 1,
      MED_E = 2,
      MEDHIGH_E = 3,
      HIGH_E = 4,
      MAX_E = 5
   };

   enum DetRegion {
     REGION1 = 0, // LAr or Tile
     REGION2 = 1,
     REGION3 = 2,
     MAX_REGION = 3
   };

   // CaloClusterContainer name 
   SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainerName{ this, "CaloClusterContainer", "CaloCalTopoClusters" };

   // Time granularity  
   std::string m_timeGran;

   //energy threshold, eta min and time width for plots 
   float m_Ethresh[MAX_E];
   float m_etaMin[MAX_REGION];
   float m_tWidth;

   // energy threshold for tile plots
   float m_Threshold;

   std::vector<float> m_binRangeE;
   std::vector<float> m_binRangeEta;
   std::vector<float> m_binRangePhi;
   std::vector<float> m_binRangeEtaPhi;

   // Services
   const CaloIdManager* m_caloMgr;
   const CaloCell_ID*   m_caloCellHelper;

   // cluster stat variables
   int m_eventsCounter; // add
   int m_cluscount;
   int m_cluscount_top;
   int m_cluscount_bot;
   int m_maxclusindex_top;
   int m_maxclusindex_bot;
   float m_maxclusene_top;
   float m_maxclusene_bot;

   // cluster EM variables
   double m_EMenergy;
   double m_EMenergy_abs;
   double m_EMet;
   double m_EMeta;
   double m_EMphi;

   // cell hists
   TH1I*       m_nCells;
   TH1F*       m_maxEcellToEclusterRatio;
   TH2F*       m_dominantCellOccupancy_etaphi;
   TProfile2D* m_dominantCellAverageEnergy_etaphi;
   TProfile2D* m_nCellInCluster_etaphi;
   TH2F*       m_clusterTimeVsEnergy;
   TH1F*       m_clusterTime;
   TH1F*       m_cellTime;
   TH2F*       m_cellvsclust_time;

   // mult-threshold cluster hists
   TH2F*       m_clus_etaphi_Et_thresh[MAX_E];
   TProfile2D* m_etaphi_thresh_avgEt[MAX_E];
//   TH2F*       m_EMclus_etaphi_Et_thresh[MAX_E];
//   TH2F*       m_EMclus_etaVsPhi[MAX_E];
   TH1F*       m_clus_eta[MAX_E];
   TH1F*       m_clus_phi[MAX_E][3];
   TH1F*       m_clus_eta_Et[MAX_E]; // km add
   TH1F*       m_clus_phi_Et[MAX_E][3]; // km add
   TH2F*       m_etaVsPhi[MAX_E];
   TProfile2D* m_etaphi_thresh_avgenergy[MAX_E];
   TH2F*       m_etaphi_thresh_Totalenergy[MAX_E];

   // negative energy cluster hists
   TH2F*       m_etaVsPhiNegEn;
   TProfile2D* m_averageNegativeEnergy_etaphi;

   // no threshold cluster hists
   TProfile*   m_averageEnergy_phi;
   TProfile*   m_averageEnergy_eta;

   TH1F*       m_clusterEnergyVsEta_barrel;
   TH1F*       m_clusterEnergyVsEta_endcap;
   TH1F*       m_clusterEnergyVsEta_hecfcal;
   TH1F*       m_clusterEtVsEta_barrel; // only for non-cosmics 
   TH1F*       m_clusterEtVsEta_endcap; // only for non-cosmics
   TH1F*       m_clusterEtVsEta_hecfcal; // only for non-cosmics

   // energy > 500GeV cluster hists
   TProfile2D* m_averageEtOver500_etaphi; // only for non-cosmics

   // cluster stat hists

   TH1I*       m_nClusters;
   TH2I*       m_nClustersBottomVsTop;
   TProfile2D* m_averageEnergy_etaphi_maxEclusters; 
   TH2F*       m_dEtaVsdPhi_maxEclustersTopVsBottom; // only for cosmics

   // tile hists 
   TProfile*   m_clustersCellsRatioEta;
   TProfile*   m_clustersCellsRatioPhi;
   TProfile*   m_clustersCellsRatioE;

   TH1F*       m_clustersE;
   TProfile*   m_clustersEta;
   TProfile*   m_clustersPhi;
   TH2F*       m_clustersEtaPhi;

   TProfile*   m_clustersCellsE;
   TProfile*   m_clustersCellsEta;
   TProfile*   m_clustersCellsPhi;

};

#endif // CaloClusterVecMon_H
