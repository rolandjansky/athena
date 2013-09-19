/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTruthCreatorTools/CommonTruthMatchWeights.h"
#include "TrkTruthData/SubDetHitStatistics.h"

namespace Trk {

//================================================================
CommonTruthMatchWeights::CommonTruthMatchWeights(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  :  AthAlgTool(type,name,parent)
  , m_subDetWeights(SubDetHitStatistics::NUM_SUBDETECTORS)
{
  declareProperty("WeightPixel",  m_subDetWeights[SubDetHitStatistics::Pixel] = 1.);
  declareProperty("WeightSCT",    m_subDetWeights[SubDetHitStatistics::SCT] = 1.);
  declareProperty("WeightTRT",    m_subDetWeights[SubDetHitStatistics::TRT] = 1.);
  declareProperty("WeightMDT",    m_subDetWeights[SubDetHitStatistics::MDT] = 1.);
  declareProperty("WeightRPC",    m_subDetWeights[SubDetHitStatistics::RPC] = 1.);
  declareProperty("WeightTGC",    m_subDetWeights[SubDetHitStatistics::TGC] = 1.);
  declareProperty("WeightCSC",    m_subDetWeights[SubDetHitStatistics::CSC] = 1.);
}

//================================================================
StatusCode CommonTruthMatchWeights::initialize() {
  MsgStream& log = msg(MSG::INFO);
  log<<"Weights = (";
  for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
    log<<m_subDetWeights[i]<<" ";
  }
  log<<")"<<endreq;

  return StatusCode::SUCCESS;
}

//================================================================

} // namespace Trk
