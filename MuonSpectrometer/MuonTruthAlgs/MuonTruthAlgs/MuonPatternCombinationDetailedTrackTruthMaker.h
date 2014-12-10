/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPATTERNCOMBINATIONDETAILEDTRACKTRUTHMAKER_H
#define MUONPATTERNCOMBINATIONDETAILEDTRACKTRUTHMAKER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRecToolInterfaces/IDetailedMuonPatternTruthBuilder.h"

/**
 * This algorithm produces track truth data using Muon PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 * 
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

class MuonPatternCombinationDetailedTrackTruthMaker : public AthAlgorithm  {
  
public:
  MuonPatternCombinationDetailedTrackTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  // PRD truth maps in any order
  std::vector<std::string> m_PRD_TruthNames;

  // Track input
  std::string m_trackCollectionName;

  // Muon pattern combination collection input
  std::string m_collection;

  // DetailedTrackTruthCollection output
  std::string m_detailedTrackTruthName;

  // Tool Handle for truth tool
  ToolHandle<Trk::IDetailedMuonPatternTruthBuilder> m_truthTool;

};

#endif/*MUONPATTERNCOMBINATIONDETAILEDTRACKTRUTHMAKER_H*/
