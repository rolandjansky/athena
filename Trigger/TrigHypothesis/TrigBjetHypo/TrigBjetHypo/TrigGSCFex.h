/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigGSCFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   John Alison
// EMAIL:    John.Alison@uchicago.edu
//
// ************************************************

#ifndef TRIGGSCFEX_H
#define TRIGGSCFEX_H

#include <vector>
#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "TrigInterfaces/FexAlgo.h"
#include "JetCalibTools/IJetCalibrationTool.h"

//#include "TrigBjetHypo/TrigBjetTagger.h"
//#include "TrigBjetHypo/TuningLikelihood.h"
//#include "TrigBjetHypo/TrigBjetDataHelper.h"

//#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"
//#include "VxSecVertex/VxSecVertexInfo.h"

class MsgStream;

namespace Analysis {
  //class IBTagTool;
  //class IBTagTrackAssociation;
  //class IBTagSecVertexing;
}

/**
 * @brief FEX class for the b-jet weight construction using offline tools
 *
 * @author John Alison <johnda @ uchicago.edu>
 *
 * This is the base feature extraction class for the HLT jet slice.
 * It computes applies the GSC calibration based on b-jet tracks
 */


class TrigGSCFex: public HLT::FexAlgo {

 public:

  /** @brief For monitoring purposes the TrigBjetTagger class has been declared as friend */
  //friend class TrigBjetTagger;

  /** @brief Constructor */
  TrigGSCFex(const std::string&, ISvcLocator*);
  /** @brief Destructor */
  ~TrigGSCFex();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  HLT::ErrorCode hltFinalize();

 private:

  // TOOLS
  /** @brief Offline BTag tool */
  ToolHandle<IJetCalibrationTool> m_jetGSCCalib_tool;
  //ToolHandle< Analysis::IBTagTool > m_bTagTool;
  /** @brief Offline TrackAssociation tool */
  //ToolHandle< Analysis::IBTagTrackAssociation > m_bTagTrackAssocTool;
  /** @brief Offline SecondaryVertexing tool */
  //ToolHandle< Analysis::IBTagSecVertexing > m_bTagSecVtxTool;

  // DATA
  /** @brief Pointer to output xAOD::BTaggingContainer */
  //xAOD::BTaggingContainer* m_trigBTaggingContainer;
  /** @brief Pointer to xAOD::BTagVertexContainer */
  //xAOD::BTagVertexContainer* m_trigBTaggingVertexContainer;
  /** @brief List of xAOD tagger basenames */
  //std::vector<std::string> m_TaggerBaseNames;

  // PROPERTIES
  /** @brief Switch for offline tools */
  bool m_setupOfflineTools;

  /** @brief to check the beam spot flag status and start track selection accordingly. */

  // 
  // /* for monitoring *//
  // 

  /** @brief Run-2 monitored variables. */
  int   m_mon_gsc_ntrk;
  float m_mon_gsc_width;
  float m_mon_gsc_ptsum;
  float m_mon_gsc_ptdiff;
  float m_mon_gsc_ptratio;

  std::string m_trackKey;
  std::string m_jetKey;
  std::string m_jetOutputKey;
  std::string m_priVtxKey;

  bool trkIsGood(const xAOD::TrackParticle *trk);

};

#endif
