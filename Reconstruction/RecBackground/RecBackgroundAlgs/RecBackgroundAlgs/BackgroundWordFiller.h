/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Algorihtm to fill the background word that is stored in the EventInfo 
Jamie Boyd 7/2/2010

Updated Mark Tibbetts 6/3/2012
*/

#ifndef RECBACKGROUNDALGS_CHBACKGROUNDWORDFILLER
#define RECBACKGROUNDALGS_CHBACKGROUNDWORDFILLER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
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

  int m_HaloNumSegment_Cut;
  int m_HaloNumClusterShape_Cut;
  int m_HaloNumOneSidedLoose_Cut;
  int m_HaloNumTwoSided_Cut;
  int m_PixMultiplicityHuge_Cut;
  int m_PixSPNonEmpty_Cut;
  int m_SCTMultiplicityHuge_Cut;
  int m_SCTSPNonEmpty_Cut;
  int m_LUCIDBeamVeto_Cut;
  int m_MBTS_SideCut;
  float m_BCMTimeDiffCol_Cut;
  float m_BCMTimeDiffHalo_CutLo;
  int  m_BCMHiGainCut;
  int  m_BCMLowGainCut;
  int m_MBTSBeamVeto_MultiplicityCut;
  int m_MBTSBeamVeto_TimeCut;
  float m_MBTSBeamVeto_ThresholdCut;
  const uint8_t m_MBTSmask, m_MBTSpattern;
  std::string m_mbtsContainerName;

  int m_LArEC_SideCut;
  float m_LArECTimeDiffCol_Cut;
  float m_LArECTimeDiffHalo_CutLo,m_LArECTimeDiffHalo_CutHi;

  unsigned int m_totalcnt;
  std::array<unsigned int,xAOD::EventInfo::NBackgroundWords> m_bitcntvec;
  std::array<const char*,xAOD::EventInfo::NBackgroundWords> m_bitnamevec;
 
};

#endif
