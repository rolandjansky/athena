/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************
//
// NAME:     TaggerHelper.h
// PACKAGE:  Trigger/TrigMonitoring/TrigBjetMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// *****************************************************

#ifndef TRIGBJETMONITORING_TAGGERHELPER
#define TRIGBJETMONITORING_TAGGERHELPER



/**
 * @brief Private class for common functions used by different classes in TrigBjetHypo package.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is a private class used for common functionalities useful to different classes in TrigBjetHypo package.
 */


#include "TMath.h"

#include "GaudiKernel/MsgStream.h"



class TrigInDetTrackCollection;
class TrigInDetTrack;
class TrigVertexCollection;
class VxContainer;

namespace Rec {
  class TrackParticleContainer;
  class TrackParticle;
}


class TaggerHelper
{
 public:

  TaggerHelper(MsgStream*);
  ~TaggerHelper();

  /** @brief To print track parameters and track parameter errors for reconstructed tracks at LVL2 */
  void showParam(const TrigInDetTrack*&, unsigned int);
  /** @brief To print track parameters and track parameter errors for reconstructed tracks at EF */
  void showParam(const Rec::TrackParticle*&, unsigned int);

  /** @brief To get the number of tracks in a track collection at LVL2. */
  unsigned int getTrackNumber(const TrigInDetTrackCollection*);
  /** @brief To get the number of tracks in a track collection at EF. */
  unsigned int getTrackNumber(const Rec::TrackParticleContainer*);
  /** @brief To get the number of vertices in a vertex collection at LVL2. */
  unsigned int getVerticesNumber(const TrigVertexCollection*&);
  /** @brief To get the number of vertices in a vertex collection at EF. */
  unsigned int getVerticesNumber(const VxContainer*&);

  float signedD0(float&, float, float);
  float signedZ0(float&, float, float);
  float phiCorr(float);
  void IPCorr(float, float, float&, float&, float, float, float, float, float);

 private:

  /** @brief Pointer to message service. */
  MsgStream* m_log;

};


#endif
