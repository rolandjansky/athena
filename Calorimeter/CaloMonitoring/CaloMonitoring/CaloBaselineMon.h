/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloBaselineMon.h
//
// AUTHORS:   Benjamin Trocme (LPSC Grenoble)    01/2018
//
// ********************************************************************

#ifndef CALOBaselineMon_H
#define CALOBaselineMon_H

#include "CaloMonitoring/CaloMonToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"

class TProfile_LW;
class TProfile2D_LW;
class TH1I_LW;

#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <functional>
#include <set>

class CaloBaselineMon : public CaloMonToolBase {
 public:

  CaloBaselineMon(const std::string & type, const std::string& name, const IInterface* parent);
   virtual ~CaloBaselineMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistograms();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();

 private:
 
   StatusCode retrieveTools();
   
   // CaloCellContainer name 
   std::string m_cellContainerName;

   // LHC BCID caracteristics. Do not change it
   const float m_BCID0_min = 0.;
   const float m_BCID0_max = 3563.;
   const int m_BCID0_nbins = 3563;
   // Minimal BCID distance away from a bunch train to compute pedestal baseline
   // If equal to negative, do not compute pedestal baseline in inter train
   int m_pedestalMon_BCIDmin;
   // Maximal BCID distance away from the start of  bunch train to monitor CaloLumiBCIDTool
   // If equal to negative, do not monitor CaloLumiBCIDTool
   int m_bcidtoolMon_BCIDmax; 
   // Definition of eta range as a function of partitions
   std::vector<float> m_etaMax,m_etaMin;
   std::vector<uint> m_nbOfEtaBins;
   std::vector<float> m_etaBinWidth;
   std::vector<float> m_inv_etaBinWidth;
   
   // Boolean to switch off/on monitoring - Derived from the two above properties
   bool m_bool_pedestalMon;
   bool m_bool_bcidtoolMon;

   // Services
   //const CaloIdManager* m_caloIdMgr;
   const CaloCell_ID*   m_calo_id;

   // Tools
   ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;

   // Control plots of BCID filled for each type
   TH1I_LW* h1_BCID_bcidtoolMon;
   TH1I_LW* h1_BCID_pedestalMon;

   std::vector<std::vector<float>> m_sum_partition_eta;

   struct partitionHistos
   {      
     TProfile_LW* hProf_bcidtoolMon_vs_Eta=nullptr;
     TProfile_LW* hProf_bcidtoolMon_vs_LB=nullptr;
     std::vector<TProfile_LW*> hProf_bcidtoolMon_vs_EtaBCID;
     //     TProfile2D_LW* hProf2d_bcidtoolMon_vs_EtaBCID=nullptr;

     TProfile_LW* hProf_pedestalMon_vs_Eta=nullptr;
     TProfile_LW* hProf_pedestalMon_vs_LB=nullptr;
     std::vector<TProfile_LW*> hProf_pedestalMon_vs_EtaBCID;
     //TProfile2D_LW* hProf2d_pedestalMon_vs_EtaBCID=nullptr;
   };

   // List of partitions given in job options;
   // Can be : EM, HEC, FCal, HEC+FCal
   std::vector<std::string> m_partNames;
   // Map between the 4 partition and the m_partNames
   // Filled at initialize
   std::vector<int> m_partMap;

   std::vector<partitionHistos> m_partHistos;  

   void bookPartitionHistos(partitionHistos& partition, uint partNumber, MonGroup& group);

};

#endif // CaloBaselineMon_H
