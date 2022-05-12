/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* Algorihtm to fill the background word that is stored in the EventInfo 
Jamie Boyd 7/2/2010

Updated Mark Tibbetts 6/3/2012
*/

#ifndef RECBACKGROUNDALGS_CHBACKGROUNDWORDFILLER
#define RECBACKGROUNDALGS_CHBACKGROUNDWORDFILLER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"

#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "BCM_CollisionTime/BcmCollisionTime.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TileEvent/TileContainer.h"
#include "LArRecEvent/LArCollisionTime.h"

#include <array>

class BackgroundWordFiller : public AthAlgorithm
{

 public:

  BackgroundWordFiller(const std::string& name, ISvcLocator* pSvcLocator);
  ~BackgroundWordFiller();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  /** ReadHandleKey for EventInfo object */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "eventInfoKey", "EventInfo", "Key for EventInfo object"};

  /** ReadHandleKey for BeamBackgroundData */
  SG::ReadHandleKey<BeamBackgroundData> m_beamBackgroundDataKey{this,"BeamBackgroundDataKey","BeamBackgroundData","Key for BeamBackgroundData object"};

  /** ReadHandleKey for LUCID_RawDataContainer */
  SG::ReadHandleKey<LUCID_RawDataContainer> m_LUCID_rawDataContainerKey{this,"LUCID_rawDataContainerKey","Lucid_RawData","Key for LUCID_RawDataContainer object"};

  /** ReadHandleKey for BcmCollisionTime */
  SG::ReadHandleKey<BcmCollisionTime> m_bcmCollisionTimeKey{this,"BcmCollisionTimeKey","BcmCollisionTime","Key for BcmCollisionTime"};

  /** ReadHandleKey for SpacePointContainers */
  SG::ReadHandleKey<SpacePointContainer> m_sctSpacePointKey{this,"SCT_SpacePointKey","SCT_SpacePoints",""};
  SG::ReadHandleKey<SpacePointContainer> m_pixSpacePointKey{this,"Pixel_SpacePointKey","PixelSpacePoints",""};

  /** ReadHandleKey for TileCellContainer */
  SG::ReadHandleKey<TileCellContainer> m_tileCellContainerKey{this,"TileCellContainerKey","MBTSContainer","Key for TileCellContainer"};

  /** ReadHandleKey for LArCollisionTime */
  SG::ReadHandleKey<LArCollisionTime> m_lArCollisionTimeKey{this,"LArCollisionTimeKey","LArCollisionTime","Key for LArCollisionTime"};

  /** WriteDecorHandleKey: needed for scheduling downstream clients */
  SG::WriteDecorHandleKey<xAOD::EventInfo> m_eventInfoDecorKey{this,"eventInfoDecorKey",
                                                               "EventInfo.backgroundWord",
                                                               "Decoration key for downstream clients"};

  Gaudi::Property<bool> m_isMC{this, "IsMC", false, "Sets whether we should expect MC objects"};
  
  int m_HaloNumSegment_Cut;
  int m_HaloNumClusterShape_Cut;
  int m_HaloNumOneSidedLoose_Cut;
  int m_HaloNumTwoSided_Cut;
  int m_PixMultiplicityHuge_Cut;
  int m_PixSPNonEmpty_Cut;
  int m_SCTMultiplicityHuge_Cut;
  int m_SCTSPNonEmpty_Cut;
  int m_LUCIDBeamVeto_Cut;
  float m_BCMTimeDiffCol_Cut;
  float m_BCMTimeDiffHalo_CutLo;
  int  m_BCMHiGainCut;
  int  m_BCMLowGainCut;
  int m_MBTSBeamVeto_MultiplicityCut;
  int m_MBTSBeamVeto_TimeCut;
  float m_MBTSBeamVeto_ThresholdCut;
  const uint8_t m_MBTSmask, m_MBTSpattern;

  int m_LArEC_SideCut;
  float m_LArECTimeDiffCol_Cut;
  float m_LArECTimeDiffHalo_CutLo,m_LArECTimeDiffHalo_CutHi;

  unsigned int m_totalcnt;
  std::array<unsigned int,xAOD::EventInfo::NBackgroundWords> m_bitcntvec;
  std::array<const char*,xAOD::EventInfo::NBackgroundWords> m_bitnamevec;
 
};

#endif
