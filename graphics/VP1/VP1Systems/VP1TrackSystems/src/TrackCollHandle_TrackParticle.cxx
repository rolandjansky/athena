/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_TrackParticle     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandle_TrackParticle.h"
#include "VP1TrackSystems/TrackHandle_TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "VP1Base/IVP1System.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include <QStringList>

//____________________________________________________________________
QStringList TrackCollHandle_TrackParticle::availableCollections( IVP1System*sys )
{
  return VP1SGContentsHelper(sys).getKeys<Rec::TrackParticleContainer>();
}

//____________________________________________________________________
class TrackCollHandle_TrackParticle::Imp {
public:
  TrackCollHandle_TrackParticle * theclass;
  int updateGUICounter;
  void possiblyUpdateGUI() {//Fixme: to TrackCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};


//____________________________________________________________________
TrackCollHandle_TrackParticle::TrackCollHandle_TrackParticle(TrackSysCommonData * cd,
							     const QString& name)
  : TrackCollHandleBase(cd,name,TrackType::TrackParticle), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
}

//____________________________________________________________________
TrackCollHandle_TrackParticle::~TrackCollHandle_TrackParticle()
{
  delete m_d;
}

//____________________________________________________________________
bool TrackCollHandle_TrackParticle::load()
{
  //Get collection:
  const Rec::TrackParticleContainer * coll(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
    message("Error: Could not retrieve track particle collection with key="+name());
    return false;
  }

  //Make appropriate trk::track handles:
  hintNumberOfTracksInEvent(coll->size());
  Rec::TrackParticleContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    m_d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null TrackParticle pointer.");
      continue;
    }
    if ((*it)->charge()==0.0) {
      messageDebug("WARNING: Ignoring TrackParticle which claims to be neutral (charge()==0.0).");
      continue;
    }
    addTrackHandle(new TrackHandle_TrackParticle(this,*it));
  }

  return true;
}

//____________________________________________________________________
bool TrackCollHandle_TrackParticle::cut(TrackHandleBase* handle)
{
  if (!TrackCollHandleBase::cut(handle))
    return false;

  //Fixme: more?

  return true;
}
