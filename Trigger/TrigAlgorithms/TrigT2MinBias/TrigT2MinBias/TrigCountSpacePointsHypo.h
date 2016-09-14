/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGSPACEPOINTCOUNTERHYPO_H 
#define TRIGMINBIAS_TRIGSPACEPOINTCOUNTERHYPO_H 

#include "GaudiKernel/IToolSvc.h"
#include "TrigInterfaces/HypoAlgo.h"

#include <string>

class TrigCountSpacePointsHypo: public HLT::HypoAlgo {
  
 public:
  TrigCountSpacePointsHypo(const std::string & name, ISvcLocator* pSvcLocator);
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();  
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  
 private:

  HLT::ErrorCode checkDetectorMask();

  Bool_t m_hltExecuteInitialisationRun; //!< Flag to run extra initialisation on the first event when xAOD::EventInfo is present
  
  /** A data member to retain a connection to the MsgStream
      service. */
  MsgStream m_log;
  
  /** Name of the TrigSpacePointCounts object */
  std::string m_trigSpacePointCountsKey;

  /** If enabled this inverts the final decision of the hypo, turning it into a veto. */
  bool m_veto;

  /** If enabled the event is passed, ignoring the trigger decision. */
  bool m_acceptAll;

  /** Cut made on time overthreshold before any multiplicities are calculated. */
  float m_timeOverThreshold_cut;

  /** If set to false the trigger combination of Pixel and SCT is an OR. */
  bool m_triggerTypeAND;

  /** If set to true rejects events with SP larger than m_totalPixelClus_cut or m_totalSctSp_cut  (depending on previous switch)*/
  bool m_vetoLargeSP;

  // Threshold settings for multiplicity requirements.
  // A trigger threshold can be disabled by setting it to be < 0

  /** Total number of pixel clusters required (becomes max number of cluters if  m_vetoLargeSP is true) */
  float m_totalPixelClus_cut;

  /** Min number of pixel clusters required  (used only if m_vetoLargeSP is true)*/
  float m_totalPixelClus_cut_min;

  /** Pixel cluster ratio A required */
  float m_pixelClusRatioA_cut;

  /** Pixel cluster ratio B required */
  float m_pixelClusRatioB_cut;

  /** Total number of pixel clusters required in endcap C */
  float m_pixelClusEndcapC_cut;

  /** Total number of pixel clusters required in the barrel */
  float m_pixelClusBarrel_cut;

  /** Total number of pixel clusters required in endcap A */
  float m_pixelClusEndcapA_cut;

  /** Maximum number of pixel clusters allowed in endcap C */
  float m_pixelClusEndcapC_max;

  /** Maximum number of pixel clusters allowed in the barrel */
  float m_pixelClusBarrel_max;

  /** Maximum number of pixel clusters allowed in endcap A */
  float m_pixelClusEndcapA_max;

  /** Minumum number of pixel clusters required in one of the endcaps */
  float m_pixelClusEndcaps_cut;


  /** Total number of sct space points required  (becomes max number of cluters if  m_vetoLargeSP is true) */
  float m_totalSctSp_cut;
  
   /** Min number of scp space points required  (used only if m_vetoLargeSP is true)*/
  float m_totalSctSp_cut_min;


  /** Total number of sct space points required in endcap C */
  float m_sctSpEndcapC_cut;

  /** Total number of sct space points required in the barrel */
  float m_sctSpBarrel_cut;

  /** Total number of sct space points required in endcap A */
  float m_sctSpEndcapA_cut;

  /** Maximum number of sct space points allowed in endcap C */
  float m_sctSpEndcapC_max;
  
  /** Maximum number of sct space points allowed in the barrel */
  float m_sctSpBarrel_max;

  /** Maximum number of sct space points allowed in endcap A */
  float m_sctSpEndcapA_max;

  /** Minumum number of sct space points required in one of the endcaps */
  float m_sctSpEndcaps_cut;

  /** detector mask flags */
  bool m_sct_barrel_a_side;
  bool m_sct_barrel_c_side;
  bool m_sct_endcap_a_side;
  bool m_sct_endcap_c_side;
  bool m_pixel_barrel, m_pixel_b_layer, m_pixel_disk;
 
  float m_totNumPixSP;
  float m_totNumSctSP;
  float m_totSelNumPixSP;
  float m_totSelNumSctSP;
  float m_maxNumPixClusEC;
  float m_maxNumSctSpEC;
 
  /** Variables used for multiplicity checks */
  enum MbSpCounterType {
    SCT_SP_EC = 0,
    SCT_SP_EA = 1,
    SCT_SP_BL = 2,
    PIXEL_CLUS_EC = 3,
    PIXEL_CLUS_EA = 4,
    PIXEL_CLUS_BL = 5,
    PIXEL_CLUS_SZ_1 = 6,
    PIXEL_CLUS_SZ_2 = 7,
    PIXEL_CLUS_SZ_3 = 8
  };

  /**  Multiplicities normally calculated once per event. */
  double m_multiplicity[9]; // Matching MbSpCounterType enum

  // trigger times
  TrigTimer* m_formFeaturesTimer;


};
#endif
