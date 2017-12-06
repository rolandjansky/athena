/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_xAODTrackParticle     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandle_xAODTrackParticle.h"
#include "VP1TrackSystems/TrackHandle_xAODTrackParticle.h"

//#include "xAODTrackParticle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "VP1Base/IVP1System.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include <QStringList>

//____________________________________________________________________
QStringList TrackCollHandle_xAODTrackParticle::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<xAOD::TrackParticleContainer>();
}

//____________________________________________________________________
class TrackCollHandle_xAODTrackParticle::Imp {
public:
  TrackCollHandle_xAODTrackParticle * theclass;
  int updateGUICounter;
  void possiblyUpdateGUI() {//Fixme: to TrackCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};


//____________________________________________________________________
TrackCollHandle_xAODTrackParticle::TrackCollHandle_xAODTrackParticle(TrackSysCommonData * cd,
							     const QString& name)
  : TrackCollHandleBase(cd,name,TrackType::xAOD), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
}

//____________________________________________________________________
TrackCollHandle_xAODTrackParticle::~TrackCollHandle_xAODTrackParticle()
{
  delete m_d;
}

//____________________________________________________________________
bool TrackCollHandle_xAODTrackParticle::load()
{
  //Get collection:
  const xAOD::TrackParticleContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve track particle collection with key="+name());
    return false;
  }

  // // Retrieve the xAOD particles:
  //  const xAOD::TrackParticleContainer* xaod = evtStore()->retrieve<const xAOD::TrackParticleContainer>( m_xAODTrackParticleCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
     message("No auxiliary store got associated to the xAODTrackParticle container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::TrackParticleContainer* >( coll )->setStore(
             ( SG::IAuxStore* ) coll->getConstStore() );

  //Make appropriate trk::track handles:
  hintNumberOfTracksInEvent(coll->size());
  xAOD::TrackParticleContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    m_d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null xAODTrackParticle pointer.");
      continue;
    }
    if ((*it)->charge()==0.0) {
      messageDebug("WARNING: Ignoring xAODTrackParticle which claims to be neutral (charge()==0.0).");
      continue;
    }
    addTrackHandle(new TrackHandle_xAODTrackParticle(this,*it));
  }

  return true;
}

//____________________________________________________________________
bool TrackCollHandle_xAODTrackParticle::cut(TrackHandleBase* handle)
{
  if (!TrackCollHandleBase::cut(handle))
    return false;

  //Fixme: more?

  return true;
}
