/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleSlimming/TrackSlimmingHdlr.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <vector>
#include "Particle/TrackParticle.h"


TrackSlimmingHdlr::TrackSlimmingHdlr(Rec::TrackParticle * trkp, INamedInterface* requester): 
		Athena::ISlimmingHdlr(),  m_trkp(trkp), m_firsthit{},
		m_lasthit{},
		m_requester(requester), 
		m_thisName("TrackSlimmingHdlr")
	{ }

void TrackSlimmingHdlr::commit()
{
  typedef std::vector<const Trk::ParametersBase<5,Trk::Charged>*> TPBs;
  TPBs * parameters = const_cast<TPBs*> (&(m_trkp->trackParameters()));
  std::vector<const Trk::ParametersBase<5,Trk::Charged>*>::iterator pit=parameters->begin();
  //Cases: size of parameters is 1 --> we keep it
  //       size of parameters is 3 --> we keep only the last one
  if(parameters->size() == 1){
    // don't remove anything in this case
  } else if(parameters->size() == 3){
    unsigned int index = 0;
    for(;pit!=parameters->end(); ++pit){
      index++;
      if(index == 1){//swap first hit
	Trk::ParametersBase<5,Trk::Charged>* rpit = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*pit));
	// check that we don't remove the perigee
	if(dynamic_cast<Trk::Perigee*>(rpit) == NULL && rpit != NULL){
	  std::swap(rpit, m_firsthit);
	  (*pit) = 0;
	}else{
	  Athena::MsgStreamMember log(Athena::Options::Eager, m_thisName);
	  log << MSG::ERROR << "We found Perigee in commit in Trk::ParametersBase vector at index " << index << " , which should not be the case!" << std::endl;
	}
      }
      
      if(index == 2){//swap last hit
	Trk::ParametersBase<5,Trk::Charged>* rpit = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*pit));
	// check that we don't remove the perigee
	if(dynamic_cast<Trk::Perigee*>(rpit) == NULL && rpit != NULL){
	  std::swap(rpit, m_lasthit);
	  (*pit) = 0;
	}else{
	  Athena::MsgStreamMember log(Athena::Options::Eager, m_thisName);
	  log << MSG::ERROR << "We found Perigee in commit in Trk::ParametersBase vector at index " << index << " , which should not be the case!" << std::endl;
	}
      }
    }
  }

  
}

void TrackSlimmingHdlr::rollback()
{
  typedef std::vector<const Trk::ParametersBase<5,Trk::Charged>*> TPBs;
  TPBs* parameters = const_cast<TPBs*> (&(m_trkp->trackParameters()));
  std::vector<const Trk::ParametersBase<5,Trk::Charged>*>::iterator pit=parameters->begin();
  //Cases: size of parameters is 1 --> don't do anything, it's the perigee 
  //       size of parameters is 3 --> we put back the 2 non-perigee parameters
  if(parameters->size() == 1){
    // don't remove anything in this case
  } else if(parameters->size() == 3){
    unsigned int index = 0;
    for(;pit!=parameters->end(); ++pit){
      index++;
      if(index == 1){//swap back first hit
	Trk::ParametersBase<5,Trk::Charged>* rpit = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*pit));
	// check that we don't remove the perigee
	if(dynamic_cast<Trk::Perigee*>(rpit) == NULL && rpit != NULL){
	  // ok, now swap this element back
	  std::swap(m_firsthit, rpit);
	}else{
	  Athena::MsgStreamMember log(Athena::Options::Eager, m_thisName);
	  log << MSG::ERROR << "We found Perigee in rollback in Trk::ParametersBase vector at index " << index << " , which should not be the case!" << std::endl;
	}
      }
      
      if(index == 2){//swap last hit
	Trk::ParametersBase<5,Trk::Charged>* rpit = const_cast<Trk::ParametersBase<5,Trk::Charged>*>((*pit));
	// check that we don't remove the perigee
	if(dynamic_cast<Trk::Perigee*>(rpit) == NULL && rpit != NULL){
	  // ok, now swap this element back
	  std::swap(m_lasthit, rpit);
	}else{
	  Athena::MsgStreamMember log(Athena::Options::Eager, m_thisName);
	  log << MSG::ERROR << "We found Perigee in rollback in Trk::ParametersBase vector at index " << index << " , which should not be the case!" << std::endl;
	}
      }
    }
  }
}

/** @brief returns a pointer to the object being slimmed
 */
void * TrackSlimmingHdlr::object()
{
  return m_trkp;  
}


/** @brief returns the component who requested the registration of that
 *         slimming handler
 *         (mostly for debugging purposes)
 */
const ::INamedInterface* TrackSlimmingHdlr::requester()
{
  return m_requester;
}


/** @brief returns the type-id of the object being slimmed
 *         (mostly for debugging purposes)
 */
std::type_info& TrackSlimmingHdlr::type_id()
{
  const std::type_info & i(typeid(Rec::TrackParticle));
  return const_cast<std::type_info&>(i);
}
