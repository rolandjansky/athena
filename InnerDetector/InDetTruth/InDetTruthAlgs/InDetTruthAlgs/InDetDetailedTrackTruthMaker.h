/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETDETAILEDTRACKTRUTHMAKER_H
#define INDETDETAILEDTRACKTRUTHMAKER_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

namespace InDet {

/**
 * This algorithm produces track truth data using InDet PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov>
 */

class InDetDetailedTrackTruthMaker : public AthReentrantAlgorithm  {

public:
  InDetDetailedTrackTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute_r(const EventContext &ctx) const;
  virtual StatusCode finalize();

private:
  // PRD inputs
  SG::ReadHandleKey<PRD_MultiTruthCollection> m_PRDTruthNamePixel;
  SG::ReadHandleKey<PRD_MultiTruthCollection> m_PRDTruthNameSCT;
  SG::ReadHandleKey<PRD_MultiTruthCollection> m_PRDTruthNameTRT;
  // Track input
  SG::ReadHandleKey<TrackCollection>          m_trackCollectionName;

  // DetailedTrackTruthCollection output
  SG::WriteHandleKey<DetailedTrackTruthCollection> m_detailedTrackTruthName;

  // Tool Handle for truth tool
  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool;

};


} // namespace InDet

#endif/*INDETDETAILEDTRACKTRUTHMAKER_H*/
