/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TaggerHelper.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETHYPO_TAGGERHELPER
#define TRIGBJETHYPO_TAGGERHELPER

/**
 * @brief Private class for common functions used by different classes in TrigBjetHypo package.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is a private class used for common functionalities useful to different classes in TrigBjetHypo package.
 */


#include "TMath.h"

#ifndef VALIDATION_TOOL
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"
#include "VxVertex/VxContainer.h"
 
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

class TrigInDetTrackCollection;
class TrigInDetTrack;
class TrigVertexCollection;
//class VxContainer;

/* namespace Rec { */
/*   class TrackParticleContainer; */
/*   class TrackParticle; */
/* } */

namespace Trk { 
  class ErrorMatrix;
  class RecVertex;
}

#else

#include "CLHEP/Vector/ThreeVector.h"

#include <vector>

#include "Root/MsgStream.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"

#endif

class TaggerHelper
{
 public:

#ifndef VALIDATION_TOOL

  TaggerHelper(MsgStream&, unsigned int);

#else

  TaggerHelper(MsgStream&, unsigned int, bool);

#endif

  ~TaggerHelper();

#ifndef VALIDATION_TOOL

  /** @brief To print track parameters and track parameter errors for reconstructed tracks at LVL2 */
  void showParam(const TrigInDetTrack*&, unsigned int);
  /** @brief To print track parameters and track parameter errors for reconstructed tracks at EF */
  void showParam(const xAOD::TrackParticle*&, unsigned int);

  /** @brief To get the number of tracks in a track collection at LVL2. */
  unsigned int getTrackNumber(const TrigInDetTrackCollection*);
  /** @brief To get the number of tracks in a track collection at EF. */
  unsigned int getTrackNumber(const xAOD::TrackParticleContainer*);
  /** @brief To get the number of vertices in a vertex collection at LVL2. */
  unsigned int getVerticesNumber(const TrigVertexCollection*&);
  /** @brief To get the number of vertices in a vertex collection at EF. */
  unsigned int getVerticesNumber(const VxContainer*&);

#else

  unsigned int getTrackNumber(const std::vector<TrigBjetTrackInfo> &);

#endif

  float signedD0(float, float, float);
  float signedZ0(float, float, float);
  float phiCorr(float);
  void  IPCorr(float, float, float&, float&, float, float, float, float, float);

#ifndef VALIDATION_TOOL
  //double VrtVrtDist(const Trk::RecVertex&, const Amg::Vector3D&, const std::vector<double>, float&);
  double VrtVrtDist(const xAOD::Vertex&, const Amg::Vector3D&, const std::vector<double>, float&);
#else
#endif

 private:

  /** @brief Pointer to message service. */
  MsgStream m_log;
  unsigned int m_logLvl;  

#ifdef VALIDATION_TOOL
  float m_sign; 
#endif

};


#endif
