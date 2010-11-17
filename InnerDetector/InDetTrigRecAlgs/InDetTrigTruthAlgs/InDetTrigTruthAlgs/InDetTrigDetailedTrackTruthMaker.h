/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * This algorithm produces track truth data using InDet PRD truth collections.
 * Calls a DetailedTrackTruthBuilder tool that does the actual job.
 * 
 * @author Andrei Gaponenko <agaponenko@lbl.gov> 
 */

// filename: InDetTrigDetailedTrackTruthMaker.cxx
//
// author: Adapted for the Event Filter: Iwona Grabowska-Bold, June 2006
//         Iwona.Grabowska@cern.ch
//
// Description:  Trigger version of the InDetDetailedTrackTruthMaker
//               (see original package documentation).
//
// -------------------------------
// ATLAS Collaboration
//***************************************************************************
#ifndef INDETTRIGDETAILEDTRACKTRUTHMAKER_H
#define INDETTRIGDETAILEDTRACKTRUTHMAKER_H

#include <string>
#include <vector>
#include "GaudiKernel/ToolHandle.h"

#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

namespace Trk { class IDetailedTrackTruthBuilder;}

namespace InDet {

class InDetTrigDetailedTrackTruthMaker : public HLT::FexAlgo  {
  
public:
  InDetTrigDetailedTrackTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
  virtual HLT::ErrorCode hltFinalize();
  
  
private:

  // Truth tool
  ToolHandle<Trk::IDetailedTrackTruthBuilder> m_truthTool;

  // Subdetector weights
  std::vector<double> m_subDetWeights;
  
  bool m_doTruth; //!< flag which governs whether the truth association is done or not

  void fillOutput(TrackTruthCollection *out, const DetailedTrackTruthCollection *in);
  double getProbability(const DetailedTrackTruth& dt) const;

  //monitoring
  std::vector<int> m_numPRDClusters;
  int m_numTruthTracks;
  int m_numPRDs;
    

};

} //end of namespace
#endif/*INDETTRIGDETAILEDTRACKTRUTHMAKER_H*/
