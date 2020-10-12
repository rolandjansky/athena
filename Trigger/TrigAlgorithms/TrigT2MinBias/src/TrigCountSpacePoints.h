/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGCOUNTSPACEPOINTS_H
#define TRIGMINBIAS_TRIGCOUNTSPACEPOINTS_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/AllTEAlgo.h"

#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "xAODTrigMinBias/TrigHisto2D.h"
#include "IRegionSelector/IRegSelTool.h"

class SCT_ID;
class PixelID;
class TrigSpacePointCounts;


/** @class TrigCountSpacePoints

@author Regina Kwee <Regina.Kwee@cern.ch>

A feature extraction algorithm to extract the time over threshold
distributions for Pixel clusters and the number of SCT space points.
*/
class TrigCountSpacePoints: public HLT::AllTEAlgo {
 public:
  
  TrigCountSpacePoints(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor
  virtual ~TrigCountSpacePoints();
  
  HLT::ErrorCode hltInitialize();  //!< hlt initialize
  HLT::ErrorCode hltFinalize();    //!< hlt finalize
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
  StatusCode fillCollections( bool, bool, std::vector<int>&, std::vector<int>& );

  /**
   * @brief This method overwrites the default one (doing nothing) in the algo.h class
   *
   * This is used to reset the internal caching mechanism of this algorithm
   */
  HLT::ErrorCode hltEndEvent();

 private:

  HLT::ErrorCode checkDetectorMask();
  
  Bool_t                        m_hltExecuteInitialisationRun; //!< Flag to run extra initialisation on the first event when xAOD::EventInfo is present
  ToolHandle<IRegSelTool>     m_regionSelector_pix{ this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel" };      //!< region selector service
  ToolHandle<IRegSelTool>     m_regionSelector_sct{ this, "RegSelTool_SCT",   "RegSelTool/RegSelTool_SCT"   };      //!< region selector service
  
  bool m_doPixelSp; //!< Flag to switch on or off Pixel space point retrieval 
  bool m_doSctSp; //!< Flag to switch on or off SCT space point retrieval 
  bool m_doOnlyBLayer; //!< Flag to only count innermost layer of PIX (does not affect SCT)

  const PixelID* m_pixHelper;   
  const SCT_ID* m_sctHelper;
  std::string m_sctSpName;
  std::string m_pixelSpName;

  /** A histogram of pixel cluster time over threshold against pixel
      cluster size for endcap C. */
  xAOD::TrigHisto2D* m_pixelClusEndcapC;
  
  /** A histogram of pixel cluster time over threshold against pixel
      cluster size for the barrel. */
  xAOD::TrigHisto2D* m_pixelClusBarrel;
  
  /** A histogram of pixel cluster time over threshold against pixel
      cluster size for endcap A. */
  xAOD::TrigHisto2D* m_pixelClusEndcapA;
    
  bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode.
  
  xAOD::TrigSpacePointCounts* m_spacePointCounts; //!< internal caching: SpacePoint feature from the first execution.
  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution.

  /** detector mask flags */
  bool m_sct_barrel_a_side;
  bool m_sct_barrel_c_side;
  bool m_sct_endcap_a_side;
  bool m_sct_endcap_c_side;
  bool m_pixel_barrel, m_pixel_b_layer, m_pixel_disk;
  
  // ----------------------------------------------------------------------------------------------------------


  float m_nPixSP;
  float m_totNumPixSP;
  float m_pixClSize;
  float m_nPixCL_1; 
  float m_nPixCL_2; 
  float m_nPixCLmin3; // pixel clusters with size > 3 
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
  
  // module monitoring
  std::vector<int> m_pixECA_clust_occ_disk;
  std::vector<int> m_pixECC_clust_occ_disk;
  std::vector<int> m_pixECA_clust_occ_phi;
  std::vector<int> m_pixECC_clust_occ_phi;
  
  std::vector<int> m_pixBarrL0_clust_occ_eta;
  std::vector<int> m_pixBarrL1_clust_occ_eta;
  std::vector<int> m_pixBarrL2_clust_occ_eta;
  std::vector<int> m_pixBarrL0_clust_occ_phi;
  std::vector<int> m_pixBarrL1_clust_occ_phi;
  std::vector<int> m_pixBarrL2_clust_occ_phi;
  
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
  
  // module monitoring
  // 4 barrel layers
  std::vector<int> m_sctBarrL1_sp_occ_eta;
  std::vector<int> m_sctBarrL2_sp_occ_eta;
  std::vector<int> m_sctBarrL3_sp_occ_eta;
  std::vector<int> m_sctBarrL4_sp_occ_eta;
  
  std::vector<int> m_sctBarrL1_sp_occ_phi;
  std::vector<int> m_sctBarrL2_sp_occ_phi;
  std::vector<int> m_sctBarrL3_sp_occ_phi;
  std::vector<int> m_sctBarrL4_sp_occ_phi;
  
  // eca
  std::vector<int> m_sctECA_sp_occ_disk;
  std::vector<int> m_sctECA_sp_occ_phi;
  
  // ecc
  std::vector<int> m_sctECC_sp_occ_disk;
  std::vector<int> m_sctECC_sp_occ_phi;


  // trigger times

  TrigTimer* m_pixSPCTimer;
  TrigTimer* m_sctSPCTimer;
  TrigTimer* m_pixGetCollTimer;
  TrigTimer* m_sctGetCollTimer;
  TrigTimer* m_attachFTimer;

  /** TrigHisto input parameters */
  unsigned int m_hPixelClusTotBins;
  float m_hPixelClusTotMin;
  float m_hPixelClusTotMax;
  unsigned int m_hPixelClusSizeBins;
  float m_hPixelClusSizeMin;
  float m_hPixelClusSizeMax;
  
  /** Variables used to monitor data stored in TrigSpacePointCounts */ 
  std::vector<float> m_pixelClusEndcapC_Tot; 
  std::vector<float> m_pixelClusEndcapC_Size; 
  std::vector<float> m_pixelClusBarrel_Tot; 
  std::vector<float> m_pixelClusBarrel_Size; 
  std::vector<float> m_pixelClusEndcapA_Tot; 
  std::vector<float> m_pixelClusEndcapA_Size;

};


#endif



