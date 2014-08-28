/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDETAILEDTRACKTRUTHMAKER_H
#define MUONDETAILEDTRACKTRUTHMAKER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"

/**
 * This algorithm produces track truth data using Muon PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 * 
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

class MuonDetailedTrackTruthMaker : public AthAlgorithm  {
  
public:
  MuonDetailedTrackTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  // PRD truth maps in any order
  std::vector<std::string> m_PRD_TruthNames;

  // Track input
  std::vector<std::string> m_trackCollectionNames;

  // DetailedTrackTruthCollection output
  std::vector<std::string> m_detailedTrackTruthNames;

  // Tool Handle for truth tool
  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool;

};

#endif/*MUONDETAILEDTRACKTRUTHMAKER_H*/
