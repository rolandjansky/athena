/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *  @file HLTIDpvtxTool.h
 *
 *  Contact:
 *  @author Ben.Smart (AT cern DOT ch)
 *  
 *  Todo: Make plots aware of warm period, through DCS info (?)
 *
 */

#ifndef HLTIDPVTXTOOL_H
#define HLTIDPVTXTOOL_H

#include <string>
#include <vector>
#include <map>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "tauEvent/TauJetContainer.h" //TJN
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/Feature.h"
#include "TLorentzVector.h"

// Forward declarations
class IInterface;
class StoreGateSvc;
class TH1I;
class TH1F;
class StatusCode;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class HLTIDpvtxTool : public IHLTMonTool
{

 public:

  //+++ Standard Athena algorithm methods
  HLTIDpvtxTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~HLTIDpvtxTool();
  
  StatusCode init();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); 
#endif

  StatusCode fill();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
#endif
  
 private:

  void calculate(int nTracks);
    
  //+++ Trigger decision tool related
  ToolHandle<Trig::TrigDecisionTool> m_TDT;

  std::string m_ChainName;
  std::string m_pvtxContainer;
  float m_effRangeX;
  float m_effRangeY;
  float m_effRangeZ;

  float m_offline_pvtx_x;
  float m_offline_pvtx_y;
  float m_offline_pvtx_z;

  float m_BeamSpot_x;
  float m_BeamSpot_y;
  float m_BeamSpot_z;

  float m_BeamSpot_sigmaX;
  float m_BeamSpot_sigmaY;
  float m_BeamSpot_sigmaZ;

  float m_online_pvtx_x;
  float m_online_pvtx_y;
  float m_online_pvtx_z;

  int m_nTracksTotal;

  bool m_haveOfflinePV;
  bool m_haveOnlinePV;
  
  std::vector< std::vector< int > > m_online_efficiency; // number of events within range, number of events out of range, nTracks
  std::vector< std::vector< int > > m_online_efficiency_xyz; // 0=x, 1=y, 2=z, number of events within range, number of events out of range

  std::vector< std::vector< double > > m_online_x_resolution; // k, A, Q, nTracks
  std::vector< std::vector< double > > m_online_y_resolution;
  std::vector< std::vector< double > > m_online_z_resolution;
  std::vector< std::vector< double > > m_online_r_resolution;

};

#endif
