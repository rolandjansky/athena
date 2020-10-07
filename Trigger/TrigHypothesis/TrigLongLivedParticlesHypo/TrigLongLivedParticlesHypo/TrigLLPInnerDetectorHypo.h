/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     TrigLLPInnerDetectorHypo
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// AUTHORS:  Cristiano Alpigiani, Andrea Coccaro
//
// ************************************************************

#ifndef TRIGLLPINNERDETECTORHYPO_H
#define TRIGLLPINNERDETECTORHYPO_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/HypoAlgo.h"

#include "IRegionSelector/IRegSelTool.h"


class StoreGateSvc;
class SCT_ID;
class PixelID;



class TrigLLPInnerDetectorHypo: public HLT::HypoAlgo {
 public:
  
  TrigLLPInnerDetectorHypo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor
  virtual ~TrigLLPInnerDetectorHypo();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  StatusCode fillCollections( bool, bool, std::vector<int>&, std::vector<int>& );

 private:

  HLT::ErrorCode checkDetectorMask();
  
  Bool_t                        m_hltExecuteInitialisationRun; //!< Flag to run extra initialisation on the first event when xAOD::EventInfo is present

  ToolHandle<IRegSelTool>  m_regionSelector_pix { this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel" };      //!< region selector tool
  ToolHandle<IRegSelTool>  m_regionSelector_sct { this, "RegSelTool_SCT",   "RegSelTool/RegSelTool_SCT"   };      //!< region selector tool
  
  const PixelID* m_pixHelper;   
  const SCT_ID* m_sctHelper;
  std::string m_sctSpName;
  std::string m_pixelSpName;
  
  /** detector mask flags */
  bool m_sct_barrel_a_side;
  bool m_sct_barrel_c_side;
  bool m_sct_endcap_a_side;
  bool m_sct_endcap_c_side;
  bool m_pixel_barrel, m_pixel_b_layer, m_pixel_disk;
  
  // ----------------------------------------------------------------------------------------------------------


  float m_nPixSP;
  float m_totNumPixSP;
  float m_pixclToT; 
  float m_totNumPixCL_1; 
  float m_totNumPixCL_2; 
  float m_totNumPixCLmin3; 
  float m_pixelClusToTCut;
  float m_ratioA;
  float m_ratioB;

  //std::vector<int> m_listOfPixIds;
  //std::vector<int> m_listOfSctIds;
  std::vector<IdentifierHash> m_listOfSctIds; 
  std::vector<IdentifierHash> m_listOfPixIds;      
  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_sctDataErrors;
  int m_pixListSize;
  int m_sctListSize;
  
  unsigned int m_pixModuleThreshold;
  unsigned int m_sctModuleThreshold;
  unsigned int m_maxnid;

  std::vector<unsigned int> m_pixSpPerModuleBarr;
  std::vector<unsigned int> m_pixSpPerModuleECA;
  std::vector<unsigned int> m_pixSpPerModuleECC;
  std::vector<unsigned int> m_pixSpPerModule;
  
  // endcap/barrel separation for pixel
  unsigned int m_SPpixBarr; 
  unsigned int m_SPpixECA;  
  unsigned int m_SPpixECC;
  
  unsigned int m_pixClBarrel;
  unsigned int m_pixClEndcapA;
  unsigned int m_pixClEndcapC;
  
  // --- SCT
  unsigned int m_nSctSP;
  unsigned int m_totNumSctSP;
  std::vector<unsigned int> m_sctSpPerModuleBarr;
  std::vector<unsigned int> m_sctSpPerModuleECA;
  std::vector<unsigned int> m_sctSpPerModuleECC;
  std::vector<unsigned int> m_sctSpPerModule;

  // endcap/barrel separation for sct
  unsigned int m_SPsctBarr;
  unsigned int m_SPsctECA;
  unsigned int m_SPsctECC;
  
    /** The total number of SCT space points for endcap C. */
  unsigned int m_sctSpEndcapC;

  /** The total number of SCT space points for the barrel. */
  unsigned int m_sctSpBarrel;

  /** The total number of SCT space points for endcap A. */
  unsigned int m_sctSpEndcapA;

  /** TrigHisto input parameters */
  unsigned int m_hPixelClusTotBins;
  float m_hPixelClusTotMin;
  float m_hPixelClusTotMax;
  unsigned int m_hPixelClusSizeBins;
  float m_hPixelClusSizeMin;
  float m_hPixelClusSizeMax;
  
};


#endif



