// C/C++
#include <math.h>
#include <fstream>

// Athena
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "FourMomUtils/xAODP4Helpers.h"
// Local

#include "RpcRawDataMonitoring/RPCDQUtils.h"

//========================================================================================================
// struct RpcPanel
//========================================================================================================
RpcPanel::RpcPanel(Identifier id, const RpcIdHelper &rpcIdHelper)
{
  FillRpcId(id, rpcIdHelper);
}

//========================================================================================================
void RpcPanel::FillRpcId(Identifier id, const RpcIdHelper &rpcIdHelper)
{
  //
  // Read RPC data
  //
  stationName = rpcIdHelper.stationName(id);
  stationEta  = rpcIdHelper.stationEta (id);
  stationPhi  = rpcIdHelper.stationPhi (id);
  doubletR    = rpcIdHelper.doubletR   (id);
  doubletZ    = rpcIdHelper.doubletZ   (id);
  doubletPhi  = rpcIdHelper.doubletPhi (id);
  gasGap      = rpcIdHelper.gasGap     (id);
  measPhi     = rpcIdHelper.measuresPhi(id);
  
  panelId     = rpcIdHelper.panelID(stationName,
			                              stationEta,
			                              stationPhi,
			                              doubletR,
			                              doubletZ,
			                              doubletPhi,
			                              gasGap,
                                    measPhi,
			                              true,
			                              &panel_valid);

  // panel_index = stationName*6144 + stationPhi*768 + (stationEta+9)*48 + doubletR*24 + doubletPhi*12 + doubletZ*4 + gasGap*2 + measPhi;
}

//========================================================================================================
RpcPanel::RpcPanel( const Muon::IMuonIdHelperSvc    &idHelperSvc,
                    const MuonGM::RpcReadoutElement *_readoutEl, 
                    const int                        _doubletZ, 
                    const int                        _doubletPhi,
                    const int                        _gasgap,
                    const int                        _measPhi,
                    int                        &_panelIn):
  readoutEl  (_readoutEl),
  doubletR   (_readoutEl->getDoubletR()),
  doubletZ   (_doubletZ),
  doubletPhi (_doubletPhi),
  gasGap     (_gasgap),
  measPhi    (_measPhi)
{
  const RpcIdHelper& rpcIdHelper = idHelperSvc.rpcIdHelper();
  const Identifier   readEl_id   = readoutEl->identify();

  stationName    = rpcIdHelper.stationName(readEl_id);
  stationNameStr = rpcIdHelper.stationNameString(stationName);

  stationEta  = rpcIdHelper.stationEta (readEl_id);
  stationPhi  = rpcIdHelper.stationPhi (readEl_id);
  // panel_index = stationName*6144 + stationPhi*768 + (stationEta+9)*48 + doubletR*24 + doubletPhi*12 + doubletZ*4 + gasGap*2 + measPhi;

  //
  // Get Identifier for this panel
  //
  panelId = rpcIdHelper.panelID(stationName,
			                          stationEta,
			                          stationPhi,
			                          doubletR,
			                          doubletZ,
			                          doubletPhi,
			                          gasGap,
                                measPhi,
			                          true,
			                          &panel_valid);
  if(panel_valid) {
    panel_str = idHelperSvc.toString(panelId);
    // Only _doubletPhi maybe different with rpcIdHelper.doubletPhi(readEl_id)
    // cout<<"Test-rpcIdHelper.gasGap(readEl_id)      = "<<rpcIdHelper.gasGap(readEl_id)<< std::endl;
    panel_index = _panelIn;
    _panelIn++;
    panel_name = getOnlineConvention();
  }
  else {
    panel_str = idHelperSvc.toString(panelId) + " - INVALID ID";
  }

}

//========================================================================================================
bool RpcPanel::operator ==(const RpcPanel &rhs) const
{
  if (this->stationName == rhs.stationName&&
      this->stationPhi  == rhs.stationPhi &&
      this->stationEta  == rhs.stationEta &&
      this->doubletR    == rhs.doubletR   &&
      this->doubletPhi  == rhs.doubletPhi &&
      this->doubletZ    == rhs.doubletZ   &&
      this->gasGap      == rhs.gasGap     &&
      this->measPhi     == rhs.measPhi    ){
    return true;
  }
  else { return false; }
}

//========================================================================================================
bool RpcPanel::operator <(const RpcPanel &rhs) const
{
  return this->panel_index < rhs.panel_index;
}

//========================================================================================================
bool RpcPanel::operator >(const RpcPanel &rhs) const
{
  return this->panel_index > rhs.panel_index;
}

//========================================================================================================
std::string RpcPanel::getOnlineConvention() const
{

  const std::string mystring_name = stationNameStr;
  const std::string mystring_eta  =        std::to_string(std::abs(stationEta));
  const std::string mystring_dPhi = "DP" + std::to_string(         doubletPhi) + ".";
  const std::string mystring_gap  = "Ly" + std::to_string(         gasGap - 1) + ".";
  const std::string mystring_dZ   = "DZ" + std::to_string(         doubletZ);

  std::string mystring_phi;
  std::string mystring_side;
  std::string mystring_PICO; //pivot or confirm
  std::string mystring_ETAPHI;

  int myphi_part = (2 * stationPhi) - 1;

  if     (stationEta > 0) { mystring_side = "A";               }
  else if(stationEta < 0) { mystring_side = "C";               }
  else                    { mystring_side = "N"; }  //N: NULL, "ETA NOT DEFINED"
     
  if(stationName == 3 || stationName == 5 || stationName == 8 || stationName == 9 || stationName == 10) {
    myphi_part += 1;
  }
  
  if(myphi_part < 10) { mystring_phi = "0" + std::to_string(myphi_part); }
  else                { mystring_phi =       std::to_string(myphi_part); }

  if ((stationNameStr == "BMS" && doubletR == 1) ||
      (stationNameStr == "BML" && doubletR == 1) ||
      (stationNameStr == "BMF" && doubletR == 1)) {
    mystring_PICO = ".CO.";
  }
  else if ((stationNameStr == "BMS" && doubletR == 2) ||
	         (stationNameStr == "BML" && doubletR == 2) ||
	         (stationNameStr == "BMF" && doubletR == 2)) {
    mystring_PICO = ".PI.";
  }
  else if ((stationNameStr == "BOS" && doubletR == 1) ||
	         (stationNameStr == "BOL" && doubletR == 1) || 
	         (stationNameStr == "BOF" && doubletR == 1) ||
	         (stationNameStr == "BOG" && doubletR == 1)) {
    mystring_PICO = ".CO.";
  }
  else {
    // mystring_PICO = ".PIVOT_CONFIRMED_NOT_DEFINED.";
      mystring_PICO = ".NU"+std::to_string(doubletR)+"."; // NU: NULL, Not defined
  }
  
  if(measPhi == 0) { mystring_ETAPHI = "ETA."; }
  else             { mystring_ETAPHI = "PHI."; }
    
  return mystring_name + mystring_eta + mystring_side + mystring_phi + mystring_PICO + mystring_dPhi + mystring_gap + mystring_dZ + mystring_ETAPHI;
}

//========================================================================================================
std::pair<int, int> RpcPanel::getSectorLayer() const
{

  // {'2':'BML', '3':'BMS', '4':'BOL', '5':'BOS', '8':'BMF' , '9':'BOF', '10':'BOG', '53':'BME'}
  int sector = 0;
  int layer  = 0;

  if      (stationName == 2 || stationName ==53 ){
    sector   = stationPhi*2-1;
    layer = (doubletR-1)*2 + gasGap;
  }
  else if ( stationName == 4 ){
    sector   = stationPhi*2-1;
    layer = 4 + (doubletR-1)*2 + gasGap;
  }
  else if (stationName == 3 || stationName == 8){
    sector = stationPhi*2;
    layer = (doubletR-1)*2 + gasGap;
  }
  else if (stationName == 5 || stationName == 9 || stationName == 10){
    sector = stationPhi*2;
    layer = 4+(doubletR-1)*2 + gasGap;
  }
  if (stationEta < 0) sector = -1*sector;

  std::pair<int, int> sec_layer(sector, layer);

  return sec_layer;
}

//========================================================================================================
// strcuct ExResult
//========================================================================================================
ExResult::ExResult(const Identifier _gasgap_id, const Trk::PropDirection _direction):
  gasgap_id                  (_gasgap_id),
  direction                  (_direction)
{}

//========================================================================================================
// GasGapData
//========================================================================================================
GasGapData::GasGapData(const Muon::IMuonIdHelperSvc    &idHelperSvc,
			                 const MuonGM::RpcReadoutElement *_readoutEl, 
			                 const int                        _doubletZ, 
			                 const int                        _doubletPhi,
			                 const unsigned                   _gasgap):
  readoutEl  (_readoutEl),
  doubletR   (_readoutEl->getDoubletR()),
  doubletZ   (_doubletZ),
  doubletPhi (_doubletPhi),
  gasgap     (_gasgap)
{
  const RpcIdHelper& rpcIdHelper = idHelperSvc.rpcIdHelper();
  const Identifier   readEl_id   = readoutEl->identify();

  stationName = rpcIdHelper.stationName(readEl_id);
  stationEta  = rpcIdHelper.stationEta (readEl_id);
  stationPhi  = rpcIdHelper.stationPhi (readEl_id);

  //
  // Get Identifier for this gas gap
  //
  gapid = rpcIdHelper.gapID(stationName,
			                      stationEta,
			                      stationPhi,
			                      doubletR,
			                      doubletZ,
			                      doubletPhi,
			                      gasgap,
			                      true,
			                      &gapid_valid);

  if(gapid_valid) {
    gapid_str = idHelperSvc.toString(gapid);
  }
  else {
    gapid_str = idHelperSvc.toString(gapid) + " - INVALID ID";
  }

  //
  // Determine gas gap span in eta
  //
  for(int etaStrip = 1; etaStrip <= readoutEl->NetaStrips(); ++etaStrip) {
    bool valid = false;
    const Identifier stripId = rpcIdHelper.channelID( stationName,
						                                          stationEta,
						                                          stationPhi,
						                                          doubletR,
						                                          doubletZ,
						                                          doubletPhi,
						                                          gasgap,
						                                          false,
						                                          etaStrip,
						                                          true,
						                                          &valid);
    
    if(!(valid && readoutEl->containsId(stripId))) {
      continue;
    }

    nstrip_eta++;

    const Amg::Vector3D glbStripPos = readoutEl->stripPos(doubletR, doubletZ, doubletPhi, gasgap, false, etaStrip);

    if(nstrip_eta == 1) {
      minStripEta = glbStripPos.eta();
      maxStripEta = glbStripPos.eta();      
    }
    else {
      minStripEta = std::min<double>(minStripEta, glbStripPos.eta());
      maxStripEta = std::max<double>(maxStripEta, glbStripPos.eta());      
    }
  }
  
  //
  // Determine gas gap span in phi
  //
  for(int phiStrip = 1; phiStrip <= readoutEl->NphiStrips(); ++phiStrip) {
    bool valid = false;
    const Identifier stripId = rpcIdHelper.channelID( stationName,
						                                          stationEta,
						                                          stationPhi,
						                                          doubletR,
						                                          doubletZ,
						                                          doubletPhi,
						                                          gasgap,
						                                          true,
						                                          phiStrip,
						                                          true,
						                                          &valid);
    
    if(!(valid && readoutEl->containsId(stripId))) {
      continue;
    }

    nstrip_phi++;
    
    const Amg::Vector3D glbStripPos = readoutEl->stripPos(doubletR, doubletZ, doubletPhi, gasgap, true, phiStrip);
    
    if(nstrip_phi == 1) {
      minStripPhi = glbStripPos.phi();
      maxStripPhi = glbStripPos.phi();      
    }
    else {
      minStripPhi = std::min<double>(minStripPhi, glbStripPos.phi());
      maxStripPhi = std::max<double>(maxStripPhi, glbStripPos.phi());      
    }
  }

}

//========================================================================================================
void GasGapData::computeTrackDistanceToGasGap(ExResult &result, const xAOD::TrackParticle &track) const
{
  /*
    This function:  
    - computes minum DR distance between track and RpcReadoutElement
    - do this before expensive call to extrapolator
   */

  const double deta1 = std::fabs(minStripEta - track.eta());
  const double deta2 = std::fabs(maxStripEta - track.eta());

  // const double dphi1 = std::fabs(TVector2::Phi_mpi_pi(minStripPhi - track.phi()));
  // const double dphi2 = std::fabs(TVector2::Phi_mpi_pi(maxStripPhi - track.phi()));
  const double dphi1 = std::fabs(xAOD::P4Helpers::deltaPhi(minStripPhi, track.phi()));
  const double dphi2 = std::fabs(xAOD::P4Helpers::deltaPhi(maxStripPhi, track.phi()));

  result.minTrackGasGapDEta = std::min<double>(deta1, deta2);
  result.minTrackGasGapDPhi = std::min<double>(dphi1, dphi2);

  //
  // Now check if track position is between min and max - if true, set distance to zero
  //
  if(minStripEta <= track.eta() && track.eta() <= maxStripEta) { result.minTrackGasGapDEta = 0.0; }
  if(minStripPhi <= track.phi() && track.phi() <= maxStripPhi) { result.minTrackGasGapDPhi = 0.0; }

  //
  // Compute min DR to this gas gap
  //
  result.minTrackGasGapDR = std::sqrt(result.minTrackGasGapDEta*result.minTrackGasGapDEta + 
				                              result.minTrackGasGapDPhi*result.minTrackGasGapDPhi);

  //
  // Compute min DR to the center of ReadoutElement to which this gas gap belongs
  //
  const double dphir = TVector2::Phi_mpi_pi(readoutEl->REcenter().phi() - track.phi());
  const double detar =                      readoutEl->REcenter().eta() - track.eta();

  result.minTrackReadoutDR = std::sqrt(detar*detar + dphir*dphir);
}
