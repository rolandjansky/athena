/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtxAllTE.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Viviana Cavaliere
// EMAIL:    viviana.cavaliere@cern.ch
// 
// ************************************************

#ifndef TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTXALLTE_H
#define TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTXALLTE_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

class TrigInDetTrackCollection;
class TrigInDetTrack;
class TrigVertexCollection;

namespace Rec {
  class TrackParticleContainer;
  class TrackParticle;
}

class TriggerElement;
class HistoVertexHelper;
class TrigT2HistoPrmVtxBase;



/**
 * @brief Class for a fast primary vertex reconstruction in the HLT framework.
 * This is the base class for the primary vertex reconstruction at LVL2.
 */


class TrigT2HistoPrmVtxAllTE : public HLT::AllTEAlgo {

 public:

  TrigT2HistoPrmVtxAllTE(const std::string&, ISvcLocator*);
  ~TrigT2HistoPrmVtxAllTE();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&, unsigned int output);
  HLT::ErrorCode hltFinalize(); 

 private:

  /** @brief Pointer to TrigT2HistoPrmVtxBase class (to not duplicate code between Fex and AllTE versions). */ 
  TrigT2HistoPrmVtxBase* c;

  /** @brief Const pointer to TrigT2HistoPrmVtxBase class (for monitoring). */ 
  const TrigT2HistoPrmVtxBase* m_constTrigT2HistoPrmVtxBase;

  /** @brief To retrieve tracks collection reconstructed at LVL2 and stored in TrigInDetTrackCollection. */
  HLT::ErrorCode getCollection(const TrigInDetTrackCollection*&, const HLT::TriggerElement*);

  /** @brief To get the number of tracks in a track collection at LVL2. */
  unsigned int getTrackNumbers(const TrigInDetTrackCollection*);
  /** @brief To get the number of tracks in a track collection at EF. */
  unsigned int getTrackNumbers(const xAOD::TrackParticleContainer*);

  /** @brief Pointer to TrigVertex collection. */
  //TrigVertexCollection* m_trigVertexColl;
  xAOD::VertexContainer* m_trigVertexColl;

};
 
#endif 
