/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECPARTICLE_TRACKSLIMMINGHDLR_H
#define RECPARTICLE_TRACKSLIMMINGHDLR_H

// STL includes
#include <string>
#include <typeinfo>

//#include <iostream>
// Track includes
//#include "Particle/TrackParticleContainer.h"
//#include "Particle/TrackParticle.h"
//#include "TrkTrackSummary/TrackSummary.h"
//#include "TrkTrack/TrackCollection.h"
//#include "TrkEventPrimitives/FitQuality.h"
//#include "TrkEventPrimitives/ParamDefs.h"

#include "TrkParametersBase/ParametersBase.h"
#include "TrkTrack/Track.h" //to get Trk::Charged



// Athena/Gaudi includes
#include "AthenaKernel/ISlimmingHdlr.h"
//#include "AthenaKernel/MsgStreamMember.h"

namespace Rec{
	class TrackParticle;
}

/**
 * @class Athena::TrackSlimmingHdlr
 * @brief This class is to slim track objects
 */

class TrackSlimmingHdlr: public ::Athena::ISlimmingHdlr
{

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  ///////////////////////////////////////////////////////////////////   
 public:

  // Copy constructor: 
  TrackSlimmingHdlr( const TrackSlimmingHdlr& rhs ); //no implementation?
  
  // Delete assignment
  TrackSlimmingHdlr & operator=(const TrackSlimmingHdlr& ) = delete;
  

  /// Constructor with parameters: 
  TrackSlimmingHdlr(Rec::TrackParticle * trkp,INamedInterface* requester);
  
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

  /** pointer to the component requesting slimming of the above object
   */
  const INamedInterface *m_requester;
  
  // this is needed for message service
  std::string m_thisName;

};



#endif // RECPARTICLE_TRACKSLIMMINGHDLR_H
