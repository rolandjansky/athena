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


 private:

  /** @brief Pointer to message service. */
  MsgStream* m_log;

};


#endif
