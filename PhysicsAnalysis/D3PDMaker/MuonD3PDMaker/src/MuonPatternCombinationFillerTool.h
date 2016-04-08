/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonPatternCombinationFillerTool.h
 * @author Daniel Ventura <ventura@cern.ch>
 * @date July 2012
 * @brief Block filler tool for MuonPatternCombinations, detail level 1
 */

#ifndef D3PDMAKER_MUONPATTERNCOMBINATIONFILLERTOOL_H
#define D3PDMAKER_MUONPATTERNCOMBINATIONFILLERTOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonRecToolInterfaces/IDetailedMuonPatternTruthBuilder.h"
#include "TrkTruthData/SubDetHitStatistics.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include <vector>

namespace D3PD {


class MuonPatternCombinationFillerTool
  : public BlockFillerTool< Muon::MuonPatternCombination >
{
public:
  MuonPatternCombinationFillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode book();
  virtual StatusCode initialize();

  virtual StatusCode fill(const Muon::MuonPatternCombination& pattern);
  virtual double deltaR(double eta1, double eta2, double phi1, double phi2);
  virtual SubDetHitStatistics::SubDetType findSubDetType(Identifier id);

private:
  float* m_pattern_gpos_eta;
  float* m_pattern_gpos_phi;
  float* m_pattern_gpos_r;
  float* m_pattern_gpos_z;
  float* m_pattern_gdir_x;
  float* m_pattern_gdir_y;
  float* m_pattern_gdir_z;
  int*   m_pattern_seedType;
  int* m_pattern_nMDT;
  int* m_pattern_nRPC;
  int* m_pattern_nTGC;
  int* m_pattern_nCSC;
  std::vector<std::vector<int> >* m_truth_barcode;
  std::vector<int>* m_truth_nMDT;
  std::vector<int>* m_truth_nRPC;
  std::vector<int>* m_truth_nTGC;
  std::vector<int>* m_truth_nCSC;
  std::vector<int>* m_noise_nMDT;
  std::vector<int>* m_noise_nRPC;
  std::vector<int>* m_noise_nTGC;
  std::vector<int>* m_noise_nCSC;
  std::vector<int>* m_common_nMDT;
  std::vector<int>* m_common_nRPC;
  std::vector<int>* m_common_nTGC;
  std::vector<int>* m_common_nCSC;
  
  //int m_matchedTruth;
  std::string m_truthSGkey;
  std::string m_patternKey;

  const AtlasDetectorID *m_idHelper;
  ToolHandle<Trk::IDetailedMuonPatternTruthBuilder> m_truthTool;
};

} // namespace D3PD

#endif
