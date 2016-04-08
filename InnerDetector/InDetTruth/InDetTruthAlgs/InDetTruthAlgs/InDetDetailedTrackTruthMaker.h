/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETDETAILEDTRACKTRUTHMAKER_H
#define INDETDETAILEDTRACKTRUTHMAKER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"

namespace InDet {

/**
 * This algorithm produces track truth data using InDet PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 * 
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

class InDetDetailedTrackTruthMaker : public AthAlgorithm  {
  
public:
  InDetDetailedTrackTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  // PRD inputs
  std::string m_PRDTruthNamePixel;
  std::string m_PRDTruthNameSCT;
  std::string m_PRDTruthNameTRT;
  // Track input
  std::string m_trackCollectionName;

  // DetailedTrackTruthCollection output
  std::string m_detailedTrackTruthName;
  
  // Tool Handle for truth tool
  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool;

};


} // namespace InDet

#endif/*INDETDETAILEDTRACKTRUTHMAKER_H*/
