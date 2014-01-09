/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrackParticleSlimming/TrackParticleSlimmingLastHitHdlr.h"

TrackParticleSlimmingLastHitHdlr::TrackParticleSlimmingLastHitHdlr(Rec::TrackParticle * trkp, INamedInterface* requester):
  Athena::ISlimmingHdlr(),
  m_trkp(trkp),
  m_firsthit(0),
  m_lasthit(0),
  m_perigee(0),
  m_requester(requester),
  m_thisName("TrackParticleSlimmingLastHitHdlr")
{ }

void TrackParticleSlimmingLastHitHdlr::commit()
{
  typedef std::vector<const Trk::ParametersBase<5,Trk::Charged>*> TPBs;
  TPBs * parameters = const_cast<TPBs*> (&(m_trkp->trackParameters()));
  //Cases: size of parameters is 1 --> we keep it
  //       size of parameters is 3 --> we keep only the last one
  if(parameters->size() == 1)
    {
      // don't remove anything in this case
    }
  else if(parameters->size() == 3)
    {
      Trk::ParametersBase<5,Trk::Charged>* ptr0 = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*parameters)[0]);
      m_firsthit = ptr0;
      Trk::ParametersBase<5,Trk::Charged>* ptr1 = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*parameters)[1]);
      m_lasthit  = ptr1;
      Trk::ParametersBase<5,Trk::Charged>* ptr2 = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*parameters)[2]);
      m_perigee  = dynamic_cast<Trk::Perigee*>(ptr2);
      
      // remove last hit, which should be in second in vector
      // check that we identified perigee correctly (third in vector)
      if( bool(m_lasthit) && bool(m_perigee) )
	{
	  parameters->erase( parameters->begin()+1 );
	  // we didn't erase first hit and perigee, set them back to zero
	  m_firsthit = NULL;
	  m_perigee  = NULL;
	}
      else
	{
	  Athena::MsgStreamMember log(Athena::Options::Eager, m_thisName);
	  log << MSG::ERROR << "Order in Trk::ParametersBase vector different that expected ! Not doing anything !" << endreq;
	}
    }
}

void TrackParticleSlimmingLastHitHdlr::rollback()
{
  typedef std::vector<const Trk::ParametersBase<5,Trk::Charged>*> TPBs;
  TPBs* parameters = const_cast<TPBs*> (&(m_trkp->trackParameters()));
  // put back last hit, if we removed it
  if( bool(m_lasthit) && parameters->size() == 2 )
    {
      parameters->insert( parameters->begin()+1, (Trk::ParametersBase<5,Trk::Charged>*)m_lasthit );
      m_lasthit=0;
    }
}

/** @brief returns a pointer to the object being slimmed
 */
void * TrackParticleSlimmingLastHitHdlr::object()
{
  return m_trkp;  
}


/** @brief returns the component who requested the registration of that
 *         slimming handler
 *         (mostly for debugging purposes)
 */
const ::INamedInterface* TrackParticleSlimmingLastHitHdlr::requester()
{
  return m_requester;
}


/** @brief returns the type-id of the object being slimmed
 *         (mostly for debugging purposes)
 */
std::type_info& TrackParticleSlimmingLastHitHdlr::type_id()
{
  const std::type_info & i(typeid(Rec::TrackParticle));
  return const_cast<std::type_info&>(i);
}

TrackParticleSlimmingLastHitHdlr::~TrackParticleSlimmingLastHitHdlr()
{
  delete m_firsthit;
  delete m_perigee;
  delete m_lasthit;
}
