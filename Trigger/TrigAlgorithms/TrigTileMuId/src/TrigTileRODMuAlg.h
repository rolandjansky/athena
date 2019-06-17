/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TrigTileRODMuAlg.h
//  Author   : Aranzazu Ruiz Martinez
//  Created  : December 2006
//
//  DESCRIPTION:
//     LVL2 trigger algorithm to extract the low pT muons tagged at the TileCal ROD DSP level.
//     The algorithm is processed taking as input the energy previously reconstructed at the 
//     DSP with Optimal Filtering. Only possible to use with events which contain the frag 
//     types 0x10, 0x11, 0x12 or 0x13 (inside TileCal ROD fragments).
//
//  HISTORY:
//
//  BUGS:
//    
//    
//*****************************************************************************

#ifndef  TRIGTILEMUID_TRIGTILERODMUALG_H
#define  TRIGTILEMUID_TRIGTILERODMUALG_H 

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/Algorithm.h" 
#include "TileByteStream/TileROD_Decoder.h"
#include "TileEvent/TileContainer.h"

#include <string>

namespace HLT {
  class TriggerElement;
}
class IRegSelSvc;
class IROBDataProviderSvc;
class Identifier;
class TileID;

class TrigTileRODMuAlg : public HLT::AllTEAlgo
{
 public:

  TrigTileRODMuAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~TrigTileRODMuAlg();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*input*/, unsigned int output);

 private:

  IRegSelSvc* m_pRegionSelector;  

  // Timing measurements
  TrigTimer *m_timerTotal, *m_timerAccess, *m_timerAddRoi;

  IROBDataProviderSvc * m_robDataProvider;

  const TileID * m_tileID;

  TileROD_Decoder * m_tiledecoder;
  TileL2Container * m_container; 

  // Properties

  std::string m_tileTagContainer;  

  bool m_commissioning;

  std::vector<int> m_ROBList;

  double m_etaMin;
  double m_etaMax;
  double m_phiMin;
  double m_phiMax;

  bool m_LooseSelection;
  bool m_ReadRoIsFromL1;

  // Histograms

  int m_allNMuon;
  std::vector<float> m_allEta;
  std::vector<float> m_allPhi;
  std::vector<float> m_allEne;
  std::vector<float> m_allEta_LS;
  std::vector<float> m_allPhi_LS;
  std::vector<float> m_allEne_LS;

  int m_NROBFrags;
  float m_EffEvtswRoI; 
  //float m_EffEvtswRoI_LimitedEta;
  std::vector<float> m_EffEvtswRoI_LimitedEta;
  std::vector<float> m_EtaTileMu_wRoI;
  std::vector<float> m_EtaRoI_wTileMu;
  std::vector<float> m_PhiTileMu_wRoI;
  std::vector<float> m_PhiRoI_wTileMu;
  std::vector<float> m_EtaRoI_NoTileMu;
  std::vector<float> m_PhiRoI_NoTileMu;
 
  std::vector<uint32_t> m_vrobid;
  std::vector<unsigned int> m_vid;
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *> m_vrob;
};

#endif // TRIGTILEMUID_TRIGTILERODMUALG_H
