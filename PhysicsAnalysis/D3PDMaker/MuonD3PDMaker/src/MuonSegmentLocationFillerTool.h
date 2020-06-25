/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonSegmentLocationFillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Feb 2010
 * @brief Position/direction/chamber-location for muon segments, detail level 1
 */

#ifndef D3PDMAKER_MUONSEGMENTLOCATIONFILLERTOOL_H
#define D3PDMAKER_MUONSEGMENTLOCATIONFILLERTOOL_H 1

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

namespace Trk { 
  class Segment;
}

namespace D3PD {

class MuonSegmentLocationFillerTool
  : public BlockFillerTool<Trk::Segment>
{
public:
  MuonSegmentLocationFillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  /// Standard Gaudi initialize method.
  StatusCode initialize();

  virtual StatusCode book();

  virtual StatusCode fill (const Trk::Segment& p);

private:
  // global position
  float *m_x;
  float *m_y;
  float *m_z;

  // global direction
  float *m_phi;
  float *m_theta;

  // local position
  float *m_locX;
  float *m_locY;

  // local direction - conventions for Moore and Muonboy are different!
  float *m_thetaXZ; // local x-z angle
  float *m_thetaYZ; // local y-z angle
  float *m_thetaXZ_IP; // local x-z angle pointing to the IP
  float *m_thetaYZ_IP; // local y-z angle pointing to the IP

  // chamber summary
  int *m_sector;  // phi sector - 1 to 16
  int *m_stationEta; // station eta
  bool *m_isEndcap; // 1 for endcap, 0 for barrel
  int *m_stationName; // Station name in MuonFixedId scheme

  // hit counts
  int* m_nphiHits;
  int* m_netaHits;
  int* m_netaTrigHits;
  int* m_npadHits;
  int* m_npseudoHits;

  std::vector<int>*   m_id; // fixed id
  std::vector<int>*   m_type;  // 1000*channelType (0:pad,1:eta,2:phi) + 0:MDT,1:RPC,2:TGC,3:CSC,4:STGC,5::MM,6::Pseudo)
  std::vector<float>* m_error; // error
  std::vector<float>* m_residual; // residual
  std::vector<float>* m_biasedPull; // biased pull
  std::vector<float>* m_unbiasedPull; // unbiased pull

  // tools
  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" };
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;
  ToolHandle<Trk::IPropagator>  m_slPropagator;
  ToolHandle<Trk::IResidualPullCalculator>  m_pullCalculator;
  Trk::MagneticFieldProperties* m_magFieldProperties;
};

} // namespace D3PD

#endif
