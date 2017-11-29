/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtx.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTX_H
#define TRIGT2HISTOPRMVTX_TRIGT2HISTOPRMVTX_H

#include "TrigInterfaces/FexAlgo.h"


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
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the base class for the primary vertex reconstruction at LVL2.
 */


class TrigT2HistoPrmVtx : public HLT::FexAlgo {

 public:

  TrigT2HistoPrmVtx(const std::string&, ISvcLocator*);
  ~TrigT2HistoPrmVtx();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  HLT::ErrorCode hltFinalize(); 

 private:

  /** @brief Pointer to TrigT2HistoPrmVtxBase class (to not duplicate code between the Fex and the AllTE versions). */ 
  TrigT2HistoPrmVtxBase* m_c;

  /** @brief Const pointer to TrigT2HistoPrmVtxBase class (for monitoring). */ 
  const TrigT2HistoPrmVtxBase* m_constTrigT2HistoPrmVtxBase;

  /** @brief To get the number of tracks in a track collection at EF. */
  unsigned int getTrackNumbers(const xAOD::TrackParticleContainer*);

  /** @brief Pointer to TrigVertex collection. */
  xAOD::VertexContainer* m_trigVertexColl;

};
 
#endif 
