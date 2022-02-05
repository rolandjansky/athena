// -*- c++ -*-
#ifndef RPCRAWDATAMONITORING_RPCDQUtils_H
#define RPCRAWDATAMONITORING_RPCDQUtils_H

// ROOT
#include "TH1.h"
#include "TH1F.h"

// Athena
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "xAODMuon/MuonContainer.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkExInterfaces/IExtrapolator.h"

//================================================================================================
struct RpcPanel
{
  RpcPanel()= default;
  RpcPanel(Identifier id, const RpcIdHelper &rpcIdHelper);
  RpcPanel( const Muon::IMuonIdHelperSvc    &idHelperSvc,
            const MuonGM::RpcReadoutElement *_readoutEl, 
            const int                        _doubletZ, 
            const int                        _doubletPhi,
            const int                        _gasgap,
            const int                        _measPhi,
            int                        &_panelIn);

  const MuonGM::RpcReadoutElement *readoutEl = nullptr;
  
  Identifier      panelId     {0};
  bool            panel_valid {false};
  std::string     panel_str;
  std::string     panel_name;
  std::string     stationNameStr;

  void FillRpcId(Identifier id, const RpcIdHelper &rpcIdHelper);

  int        stationName  {0};
  int        stationEta   {0};
  int        stationPhi   {0};
  int        doubletR     {0};
  int        doubletZ     {0};
  int        doubletPhi   {0};
  int        gasGap       {0};
  int        measPhi      {-1};
  int        panel_index  {0};
  unsigned   NHit_perEvt  {0};


  std::string         getOnlineConvention() const;
  std::pair<int, int> getSectorLayer() const;

  bool operator ==(const RpcPanel &rhs) const;

  bool operator <(const RpcPanel &rhs) const;

  bool operator >(const RpcPanel &rhs) const;

};

//================================================================================================
struct TagDef{
  TString eventTrig;
  TString tagTrig;
};


//================================================================================================
struct MyMuon{
  const xAOD::Muon* muon;
  TLorentzVector fourvec;

  bool tagged {false};
  bool isolated {false};
  bool tagProbeOK {false};
  bool tagProbeAndZmumuOK {false};
  bool isZmumu {false};

  std::set<int> matchedL1ThrExclusive;
  std::set<int> matchedL1ThrInclusive;
};

//================================================================================================
struct ExResult
{
  ExResult(const Identifier _gasgap_id, const Trk::PropDirection _direction);
  
  const Identifier         gasgap_id;
  const Trk::PropDirection direction;
  
  int               localTrackPosValid          {-1};
  bool              localTrackPosInBounds       {false};
  bool              localTrackPosInBoundsTight  {false};
  float             localTrackPosY  {-1.0e6};
  float             localTrackPosZ  {-1.0e6};
  float             globalTrackPosX {-1.0e6};
  float             globalTrackPosY {-1.0e6};
  float             globalTrackPosZ {-1.0e6};

  float             minTrackReadoutDR   {10.0};
  float             minTrackGasGapDEta  {10.0};
  float             minTrackGasGapDPhi  {10.0};
  float             minTrackGasGapDR    {10.0};
};

//================================================================================================
struct GasGapData
{
  GasGapData(const Muon::IMuonIdHelperSvc    &idHelperSvc,
			       const MuonGM::RpcReadoutElement *_readoutEl, 
			       const int                        _doubletZ, 
			       const int                        _doubletPhi,
			       const unsigned                   _gasgap);
  
  void computeTrackDistanceToGasGap(ExResult &result, const xAOD::TrackParticle &track) const;

  const MuonGM::RpcReadoutElement *readoutEl;
  
  Identifier   gapid       {0};
  bool         gapid_valid {false};
  std::string  gapid_str;
  
  int           stationName;
  int           stationEta;
  int           stationPhi;
  int           doubletR;
  int           doubletZ;
  int           doubletPhi;
  unsigned      gasgap;      
  
  unsigned      nstrip_eta      {0};
  unsigned      nstrip_phi      {0};
  double        minStripEta     { 10.0};
  double        maxStripEta     {-10.0};
  double        minStripPhi     { 10.0};
  double        maxStripPhi     {-10.0};
  double        localTrackPosY  {0.0};
  double        localTrackPosZ  {0.0};

  std::pair<std::shared_ptr<RpcPanel>, std::shared_ptr<RpcPanel>> RpcPanel_eta_phi;

};

//================================================================================================
struct ExSummary
{
  ExSummary ()= default;

  unsigned nGasGapInBounds{0};
  unsigned nGasGapInBoundsTight{0};
};

#endif