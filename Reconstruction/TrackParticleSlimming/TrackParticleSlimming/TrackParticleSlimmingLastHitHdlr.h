/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECPARTICLE_TRACKPARTICLESLIMMINGLASTHITHDLR_H
#define RECPARTICLE_TRACKPARTICLESLIMMINGLASTHITHDLR_H

// STL includes
#include <vector>
#include <typeinfo>

#include <iostream>

// Track includes
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkParametersBase/ParametersBase.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"


// Athena/Gaudi includes
#include "AthenaKernel/ISlimmingHdlr.h"
#include "AthenaKernel/MsgStreamMember.h"

/**
 * @class Athena::TrackParticleSlimmingLastHitHdlr
 * @brief This class is to slim track objects
 */

class TrackParticleSlimmingLastHitHdlr: public ::Athena::ISlimmingHdlr
{

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  ///////////////////////////////////////////////////////////////////   
 public:

  // Copy constructor: 
  //  TrackParticleSlimmingLastHitHdlr( const TrackParticleSlimmingLastHitHdlr& rhs );

  /// Constructor with parameters: 
  TrackParticleSlimmingLastHitHdlr(Rec::TrackParticle * trkp,INamedInterface* requester);
  
  ~TrackParticleSlimmingLastHitHdlr();
  
  /** @brief apply the slimming: remove parts of the object
   */
  void commit();
  /** @brief restore object's state as before slimming was applied
   */
  void rollback();
  /** @brief returns a pointer to the object being slimmed
   */ 
  void * object();

  /** @brief returns the type-id of the object being slimmed
   *         (mostly for debugging purposes)
   */
  virtual std::type_info& type_id();

  /** @brief returns the component who requested the registration of that
   *         slimming handler
   *         (mostly for debugging purposes)
   */
  const ::INamedInterface* requester();


  /** pointer to object being slimmed
   */  
  Rec::TrackParticle * m_trkp;
  Trk::ParametersBase<5,Trk::Charged> * m_firsthit;
  Trk::ParametersBase<5,Trk::Charged> * m_lasthit;
  Trk::Perigee *                m_perigee;

  /** pointer to the component requesting slimming of the above object
   */
  const INamedInterface *m_requester;
  
  // this is needed for message service
  std::string m_thisName;

};



#endif // RECPARTICLE_TRACKPARTICLESLIMMINGLASTHITHDLR_H
