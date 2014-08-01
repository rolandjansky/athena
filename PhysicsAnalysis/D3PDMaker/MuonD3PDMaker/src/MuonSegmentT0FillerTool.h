/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonSegmentT0FillerTool.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jun 2010
 * @brief t0 fit info for muon segments, detail level 2
 */

#ifndef D3PDMAKER_MUONSEGMENTT0FILLERTOOL_H
#define D3PDMAKER_MUONSEGMENTT0FILLERTOOL_H 1

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
//#include "MuonRecHelperTools/MuonIdHelperTool.h"
//#include "MuonCalibITools/IIdToFixedIdTool.h"
#include <string>

namespace Trk { 
  class Segment;
}


namespace D3PD {


class MuonSegmentT0FillerTool
  : public BlockFillerTool<Trk::Segment>
{
public:
  MuonSegmentT0FillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  /// Standard Gaudi initialize method.
  StatusCode initialize();

  virtual StatusCode book();

  virtual StatusCode fill (const Trk::Segment& p);

private:
  // t0 from either t0segment or chamber-t0
  float *m_t0;
  float *m_t0err;

  // muonboy specific CSC timing code
  bool m_doMuonBoyCSCTiming;
  void MuonboyT0CSCSegment(const Muon::MuonSegment* pMuonSegment, float& t0, float& t0Error);
  
  std::string m_muonSegmentT0ContainerKey;
  // tools
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  //  ToolHandle<Muon::MuonIdHelperTool>  m_idHelperTool;
  //  ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;

};

} // namespace D3PD

#endif
