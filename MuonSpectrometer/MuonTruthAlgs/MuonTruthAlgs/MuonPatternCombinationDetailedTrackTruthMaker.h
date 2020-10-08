/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPATTERNCOMBINATIONDETAILEDTRACKTRUTHMAKER_H
#define MUONPATTERNCOMBINATIONDETAILEDTRACKTRUTHMAKER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IDetailedMuonPatternTruthBuilder.h"

#include <string>
#include <vector>

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
  
private:
  Gaudi::Property<std::vector<std::string>> m_PRD_TruthNames{this,"PRD_TruthNames",{"CSC_TruthMap","RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"},"PRD truth maps in any order"};

  Gaudi::Property<std::string> m_trackCollectionName{this,"TrackCollectionName","MooreTracks","Track input"};

  Gaudi::Property<std::string> m_collection{this,"MuonPatternCombinationCollection","MuonHoughPatternCombinations","Muon pattern combination collection input"};

  Gaudi::Property<std::string> m_detailedTrackTruthName{this,"DetailedTrackTruthNames","","DetailedTrackTruthCollection output"};

  ToolHandle<Trk::IDetailedMuonPatternTruthBuilder> m_truthTool{this,"TruthTool","Trk::DetailedMuonPatternTruthBuilder/DetailedMuonPatternTruthBuilder"};
};

#endif/*MUONPATTERNCOMBINATIONDETAILEDTRACKTRUTHMAKER_H*/
