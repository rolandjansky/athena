/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBtagFexMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@ge.infn.it
//
// ************************************************

#ifndef TRIGBTAGFEX_MT_H
#define TRIGBTAGFEX_MT_H

#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigInterfaces/FexAlgo.h"

#include "TrigBjetHypo/TrigBjetTagger.h"
#include "TrigBjetHypo/TuningLikelihood.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"

#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"
#include "VxSecVertex/VxSecVertexInfo.h"



namespace Analysis {
  class IBTagTool;
  class IBTagTrackAssociation;
  class IBTagSecVertexing;
}

/**
 * @brief FEX class for the b-jet weight construction using offline tools
 *
 * @author Jeff Hetherly <jhetherly @ mail.smu.edu>
 *
 * This is the base feature extraction class for the HLT b-jet slice.
 * It computes jet light/charm/beauty probabilities using offline b-tagging tools.
 */


class TrigBtagFexMT: public AthAlgorithm {
 public:
  /** @brief For monitoring purposes the TrigBjetTagger class has been declared as friend */
  friend class TrigBjetTagger;

  /** @brief Constructor */
  TrigBtagFexMT(const std::string&, ISvcLocator*);
  /** @brief Destructor */
  ~TrigBtagFexMT();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  const xAOD::Vertex* getPrimaryVertex(const xAOD::VertexContainer* vertexContainer);

 private:
//  // TOOLS
//  /** @brief Offline BTag tool */
//  PublicToolHandle< Analysis::IBTagTool > m_bTagTool {this,"BTagTool","Analysis/IBTagTool","Offline BTag tool"};
//  /** @brief Offline TrackAssociation tool */
//  PublicToolHandle< Analysis::IBTagTrackAssociation > m_bTagTrackAssocTool {this,"BTagTrackAssocTool","Analysis/BTagTrackAssociation","Offline TrackAssociation tool"};
//  /** @brief Offline SecondaryVertexing tool */
//  PublicToolHandle< Analysis::IBTagSecVertexing > m_bTagSecVtxTool {this,"BTagSecVertexing","Analysis/BTagSecVertexing","Offline SecondaryVertexing tool"};

  // DATA
  /** @brief List of xAOD tagger basenames */
  Gaudi::Property< std::vector< std::string > > m_TaggerBaseNames {this,"TaggerBaseNames",std::vector< std::string >(),"List of xAOD tagger basenames"};

  // PROPERTIES
  /** @brief Switch for offline tools */
  Gaudi::Property< bool > m_setupOfflineTools {this,"setupOfflineTools",false,"Switch for offline tools"};
  /** @brief to check the beam spot flag status and start track selection accordingly. */
  Gaudi::Property< bool > m_useBeamSpotFlag {this,"UseBeamSpotFlag",false,"check the beam spot flag status and start track selection accordingly"};
  /** @brief Use backup algorithm for retrieving Prim Vtx. */
  Gaudi::Property< bool > m_usePriVtxKeyBackup {this,"UsePriVtxKeyBackup",true,"Use backup algorithm for retrieving Prim Vtx"};


  // //////////////////////
  // //* for monitoring *//
  // //////////////////////

 private:
  SG::ReadHandleKey< xAOD::JetContainer > m_JetContainerKey {this,"JetKey","SplitJet","Key for input jets"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_VertexContainerKey {this,"PriVtxKey","xPrimVx","Key of primary vertexes"};
  //  SG::ReadHandleKey< xAOD::VertexContainer > m_BackUpVertexContainerKey {this,"PriVtxKeyBackup","EFHistoPrmVtx","Back-Up Key of primary vertexes"};
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_trkContainerKey {this,"TracksKey","tracks__COLL","Key for track particle container"};

  SG::WriteHandleKey< xAOD::BTaggingContainer > m_outputBTaggingContainerKey {this,"OutputBTagging","Undefined","Output Btagging Container Key"};
  //  SG::WriteHandleKey< xAOD::VertexContainer > m_outputVertexContainerKey {this,"OutputVtxKey","HLT_BjetVertexFex","Output Vertex Container Key"};
  //  SG::WriteHandleKey< xAOD::BTagVertexContainer > m_outputBtagVertexContainerKey {this,"OutputBtagVtxKey","HLT_BjetSecondaryVertexFex","Output Btag Vertex Container Key"};
};

#endif

