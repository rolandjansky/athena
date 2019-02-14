/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TheAnalysis.h
//

#ifndef CALOTESTS_ANALYSIS_H
#define CALOTESTS_ANALYSIS_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AIDA/IHistogram2D.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArElecCalib/ILArfSampl.h"
#include "GeneratorObjects/McEventCollection.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TTree.h"

class LArDetDescrManager ;
class ITriggerTime;

namespace MyAnalysis {

  class Analysis : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    Analysis(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~Analysis();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    
    bool CheckLArIdentifier(int,int,int,int);

    bool CheckDMIdentifier(int,int,int,int,int);
 
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------

  ITHistSvc* m_thistSvc;
  TH1D* m_hist_etraw_emb_s0;
  TH1D* m_hist_etraw_emb_s1;
  TH1D* m_hist_etraw_emb_s2;
  TH1D* m_hist_etraw_emb_s3;
  TH1D* m_hist_etraw_emec_s0;
  TH1D* m_hist_etraw_emec_s1;
  TH1D* m_hist_etraw_emec_s2;
  TH1D* m_hist_etraw_emec_s3;
  TH1D* m_hist_clusnoise;
  TH1D* m_hist_clusnoise1;
  TH1D* m_hist_clusnoise2;
  TH1D* m_hist_clusnoise3;
  TH1D* m_hist_ot;
  TH1D* m_hist_ot_em;
  TH1D* m_hist_ot_hec;
  TH1D* m_hist_ot_fcal;
  TH1D* m_hist_it;
  TH1D* m_hist_it_em;
  TH1D* m_hist_it_hec;
  TH1D* m_hist_it_fcal;
  TH1D* m_hist_hittime;
  TH1D* m_hist_hitener;
  TH1D* m_hist_nhitlar;
  TH1D* m_hist_cal0;
  TH1D* m_hist_cal1;
  TH1D* m_hist_cal2;
  TH1D* m_hist_cal3;

  //---------------------------------------------------
  // Parameters of this algorithm
  //---------------------------------------------------
  bool m_clusternoise;
  bool m_check;
  bool m_raw;
  bool m_cluster;
  bool m_cell;
  bool m_hit;
  bool m_calhit;
  bool m_useTriggerTime;
  bool m_doTruth;

  SG::ReadHandleKey<McEventCollection> m_mcCollName { this, "MCColl", "GEN_AOD" };
  SG::ReadHandleKeyArray<LArHitContainer> m_hitContainerNames { this, "HitContainers",
                                                                {"LArHitEMB",
                                                                 "LArHitEMEC",
                                                                 "LArHitHEC",
                                                                 "LArHitFCAL"} };
  SG::ReadHandleKey<CaloCellContainer> m_caloCellName { this, "CaloCells", "AllCalo" };
  SG::ReadHandleKey<LArRawChannelContainer> m_rawChannelName { this, "RawChannels", "LArRawChannels" };
  SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_calibHitContainerNames
    { this, "CalibHitContainers",
      {"LArCalibrationHitActive",
       "LArCalibrationHitDeadMaterial",
       "LArCalibrationHitInactive",
       "TileCalibHitActiveCell",
       "TileCalibHitInactiveCell",
       "TileCalibHitDeadMaterial"} };
  SG::ReadHandleKey<CaloClusterContainer> m_cluster55Name { this, "Cluster55", "LArClusterEM" };
  SG::ReadHandleKey<CaloClusterContainer> m_cluster55gamName { this, "Cluster55gam", "LArClusterEMgam" };
  SG::ReadHandleKey<CaloClusterContainer> m_cluster35Name { this, "Cluster35", "LArClusterEM35" };
  SG::ReadHandleKey<CaloClusterContainer> m_cluster35gamName { this, "Cluster35gam", "LArClusterEMgam35" };
  SG::ReadHandleKey<CaloClusterContainer> m_cluster37Name { this, "Cluster37", "LArClusterEM37" };
  SG::ReadHandleKey<CaloClusterContainer> m_cluster37gamName { this, "Cluster37gam", "LArClusterEMgam37" };
    
  int m_nevt;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  StringProperty m_triggerTimeToolName;
  ITriggerTime* m_triggerTimeTool;

  Identifier m_id;
  const LArEM_ID*        m_larem_id;
  const CaloDM_ID*       m_calodm_id;

  // list of cell energies
  struct CellInfo {
       int bec;
       int sampling;
       double eta;
       double phi;
       double Ehit;
       double Thit;
       double Eraw;
       double Ecell;
       Identifier identifier;
   };
   std::vector<CellInfo> m_CellListEM; 

   double m_nt_e;
   double m_nt_eta;
   double m_nt_phi;
   double m_nt_e0;
   double m_nt_e1;
   double m_nt_e2;
   double m_nt_e3;
   double m_nt_c0;
   double m_nt_c1;
   double m_nt_c2;
   double m_nt_c3;
   double m_nt_hb0;
   double m_nt_hb1;
   double m_nt_hb2;
   double m_nt_hb3;
   double m_nt_he0;
   double m_nt_he1;
   double m_nt_he2;
   double m_nt_he3;
   double m_nt_tb0;
   double m_nt_tb1;
   double m_nt_tb2;
   double m_nt_tb3;
   double m_nt_eclus;
   double m_nt_eclus0;
   double m_nt_eclus1;
   double m_nt_eclus2;
   double m_nt_eclus3;
   double m_nt_etaclus;
   double m_nt_phiclus;
   double m_nt_etaclus551;
   double m_nt_etaclus552;
   double m_nt_eclus35;
   double m_nt_eclus37;
   double m_nt_eclus350;
   double m_nt_eclus351;
   double m_nt_eclus352;
   double m_nt_eclus353;
   double m_nt_etaclus35;
   double m_nt_etaclus351;
   double m_nt_etaclus352;
   double m_nt_etaclus371;
   double m_nt_etaclus372;
   double m_nt_eclusg;
   double m_nt_etaclusg551;
   double m_nt_etaclusg552;
   double m_nt_eclusg35;
   double m_nt_etaclusg351;
   double m_nt_etaclusg352;
   double m_nt_eclusg37;
   double m_nt_etaclusg371;
   double m_nt_etaclusg372;
   double m_nt_eActEM;
   double m_nt_eActNonEM;
   double m_nt_eActEscaped;
   double m_nt_eActInvisible;
   double m_nt_eInactEM;
   double m_nt_eInactNonEM;
   double m_nt_eInactEscaped;
   double m_nt_eInactInvisible;
   double m_nt_eDeadEM;
   double m_nt_eDeadNonEM;
   double m_nt_eDeadEscaped;
   double m_nt_eDeadInvisible;
   double m_nt_eTile;
   double m_nt_edead_1_0;
   double m_nt_edead_1_1_0;
   double m_nt_edead_1_1_1;
   double m_nt_edead_1_1_2;
   double m_nt_edead_1_1_3;
   double m_nt_edead_1_1_4;
   double m_nt_edead_1_1_5;
   double m_nt_edead_1_1_6;
   double m_nt_edead_1_1_7;
   double m_nt_edead_1_2_0;
   double m_nt_edead_1_2_1;
   double m_nt_edead_1_2_2;
   double m_nt_edead_1_2_3;
   double m_nt_edead_1_2_4;
   double m_nt_edead_1_2_5;
   double m_nt_edead_1_3;
   double m_nt_eleak;
   double m_xconv;
   double m_yconv;
   double m_zconv;
   double m_xvert;
   double m_yvert;
   double m_zvert;
   TTree* m_ntuple;
  };
} // end of namespace bracket
#endif
