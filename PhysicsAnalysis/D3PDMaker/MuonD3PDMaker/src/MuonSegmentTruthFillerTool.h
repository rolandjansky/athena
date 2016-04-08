/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonSegmentTruthFillerTool.h
 * @author Daniel Blackburn <ventura@cern.ch>
 * @date February 2013
 * @brief Block filler tool for MuonSegmentTruth
 */

#ifndef D3PDMAKER_MuonSegmentTruthFillerTool_H
#define D3PDMAKER_MuonSegmentTruthFillerTool_H 

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "TrkTruthData/SubDetHitStatistics.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRecToolInterfaces/IDetailedMuonPatternTruthBuilder.h"

#include <vector>


namespace D3PD {


class MuonSegmentTruthFillerTool
  : public BlockFillerTool< Trk::Segment >
{
public:
  MuonSegmentTruthFillerTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent);

  virtual StatusCode book();
  virtual StatusCode initialize();

  virtual StatusCode fill(const Trk::Segment& segment);
  virtual double deltaR(double eta1, double eta2, double phi1, double phi2);
//  virtual SubDetHitStatistics::SubDetType findSubDetType(Identifier id);

private:
  std::vector<int>* m_truth_barcode;
  float* m_res_x;
  float* m_res_y;
  float* m_dAngleYZ;
  float* m_dAngleXZ;
  int* m_truth_nSTGC;
  int* m_truth_nMM;
  int* m_truth_nMDT;
  int* m_truth_nRPC;
  int* m_truth_nTGC;
  int* m_truth_nCSC;
  int* m_noise_nSTGC;
  int* m_noise_nMM;
  int* m_noise_nMDT;
  int* m_noise_nRPC;
  int* m_noise_nTGC;
  int* m_noise_nCSC;
  int* m_common_nSTGC;
  int* m_common_nMM;
  int* m_common_nMDT;
  int* m_common_nRPC;
  int* m_common_nTGC;
  int* m_common_nCSC;
  
  //int m_matchedTruth;
  std::string m_truthSGkey;

  const AtlasDetectorID *m_idHelper;
  ToolHandle<Trk::IDetailedMuonPatternTruthBuilder> m_truthTool;
};

} // namespace D3PD

#endif
