/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloBaselineMonAlg.h
//
// Based on code from   Benjamin Trocme (LPSC Grenoble)    01/2018
//
// ********************************************************************

#ifndef CALOBASELINEMONALG_H
#define CALOBASELINEMONALG_H

#include "CaloMonAlgBase.h"

#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LumiBlockData/BunchCrossingCondData.h"

class CaloBaselineMonAlg : public CaloMonAlgBase {
 public:

  CaloBaselineMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~CaloBaselineMonAlg(){};

   virtual StatusCode initialize() override final;
   virtual StatusCode fillHistograms(const EventContext& ctx) const override final;

 private:
 
   StatusCode retrieveTools();
   
   // CaloCellContainer name 
   SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey{this,"CellContainer","AllCalo","Input CaloCellContainer key"};

   SG::ReadCondHandleKey<BunchCrossingCondData> m_bcDataKey {this, "BunchCrossingCondDataKey", "BunchCrossingData" ,"SG Key of BunchCrossing CDO"};

   Gaudi::Property<std::string> m_MonGroupName  {this, "MonGroupName", "CaloBaselineGroup"};
   // LHC BCID caracteristics. Do not change it
   const float m_BCID0_min = 0.;
   const float m_BCID0_max = 3563.;
   const int m_BCID0_nbins = 3563;
   // Minimal BCID distance away from a bunch train to compute pedestal baseline
   // If equal to negative, do not compute pedestal baseline in inter train
   Gaudi::Property<int> m_pedestalMon_BCIDmin{this, "pedestalMon_BCIDmin", 25};
   // Maximal BCID distance away from the start of  bunch train to monitor CaloLumiBCIDTool
   // If equal to negative, do not monitor CaloLumiBCIDTool
   Gaudi::Property<int> m_bcidtoolMon_BCIDmax{this, "bcidtoolMon_BCIDmax", 144}; 
   // Definition of eta range as a function of partitions
   Gaudi::Property<std::vector<float> > m_etaMax{this, "maximumEta", {}};
   Gaudi::Property<std::vector<float> > m_etaMin{this, "minimumEta", {}};
   Gaudi::Property<std::vector<uint> > m_nbOfEtaBins{this, "nbOfEtaBins", {}};
   // List of partitions given in job options;
   // Can be : EM, HEC, FCal, HEC+FCal
   Gaudi::Property<std::vector<std::string> > m_partNames{this, "partionList", {}};

   std::vector<float> m_etaBinWidth;
   std::vector<float> m_inv_etaBinWidth;
   
   // Boolean to switch off/on monitoring - Derived from the two above properties
   bool m_bool_pedestalMon;
   bool m_bool_bcidtoolMon;

   
   const CaloCell_ID*   m_calo_id;

   /* Histogram grouping (part)(ieta) */
   std::vector<std::vector<int> >m_histoGroups;

   // Control plots of BCID filled for each type
   //TH1I_LW* m_h1_BCID_bcidtoolMon;
   //TH1I_LW* m_h1_BCID_pedestalMon;

   /*
   struct partitionHistos
   {      
     TProfile_LW* hProf_bcidtoolMon_vs_Eta=nullptr;
     TProfile_LW* hProf_bcidtoolMon_vs_LB=nullptr;
     std::vector<TProfile_LW*> hProf_bcidtoolMon_vs_EtaBCID;

     TProfile_LW* hProf_pedestalMon_vs_Eta=nullptr;
     TProfile_LW* hProf_pedestalMon_vs_LB=nullptr;
     std::vector<TProfile_LW*> hProf_pedestalMon_vs_EtaBCID;
   };
   */

   // Map between the 4 partition and the m_partNames
   // Filled at initialize
   std::vector<int> m_partMap;

   // std::vector<partitionHistos> m_partHistos;  

};

#endif // CaloBaselineMonAlg_H
