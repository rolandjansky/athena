/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonPatternCombinationMissedHitFillerTool.h
 * @author Daniel Ventura <ventura@cern.ch>
 * @date November 2012
 * @brief Block filler tool for hits not contained in MuonPatternCombinations, detail level 1
 */

#ifndef D3PDMAKER_MUONPATTERNCOMBINATIONMISSEDHITFILLERTOOL_H
#define D3PDMAKER_MUONPATTERNCOMBINATIONMISSEDHITFILLERTOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonRecToolInterfaces/IDetailedMuonPatternTruthBuilder.h"
#include "TrkTruthData/SubDetHitStatistics.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "EventInfo/EventInfo.h"
#include <vector>
using namespace std;

namespace D3PD {


class MuonPatternCombinationMissedHitFillerTool
  : public BlockFillerTool< EventInfo >
{
public:
  MuonPatternCombinationMissedHitFillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode book();
  virtual StatusCode initialize();

  virtual StatusCode fill(const EventInfo& p);
  virtual SubDetHitStatistics::SubDetType findSubDetType(Identifier id);

private:
  vector<vector<int> >* m_truth_barcode;
  vector<int>* m_truth_nMDT;
  vector<int>* m_truth_nRPC;
  vector<int>* m_truth_nTGC;
  vector<int>* m_truth_nCSC;
  vector<int>* m_noise_nMDT;
  vector<int>* m_noise_nRPC;
  vector<int>* m_noise_nTGC;
  vector<int>* m_noise_nCSC;
  vector<int>* m_common_nMDT;
  vector<int>* m_common_nRPC;
  vector<int>* m_common_nTGC;
  vector<int>* m_common_nCSC;

  const AtlasDetectorID *m_idHelper;
  ToolHandle<Trk::IDetailedMuonPatternTruthBuilder> m_truthTool;
};

} // namespace D3PD

#endif
