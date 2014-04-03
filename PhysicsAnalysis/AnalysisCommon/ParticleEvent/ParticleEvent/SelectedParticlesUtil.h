// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_SELECTEDPARTICLESUTIL_H
#define PARTICLEEVENT_SELECTEDPARTICLESUTIL_H 1

/*************************************************************************
SelectedParticlesUtil.h:  template functions for conversions SelectedParticles<->ParticleLinks
                 
          S. Protopopescu 15-Dec-2008
 
***********************************************************************/

#include "ParticleEvent/SelectedParticles.h"
#include "ParticleEvent/ParticleLinks.h"

namespace SelectedParticlesUtil
{

  template<class Container, class Contained>
  void getContainerInfo(const SelectedParticles* sel, const Container* coll, std::vector<const Contained*>& objs, 
			std::vector<int>& indices); 

  template<class Container>
      void getParticleLinks(const SelectedParticles* selparts, const Container* particles,
			    ParticleLinks<Container>& links);

  template<class Container>
      void getSelectedParticles(const ParticleLinks<Container>* links, SelectedParticles& selparts);
}

  template<class Container, class Contained>
      void SelectedParticlesUtil::getContainerInfo(const SelectedParticles* selparts, const Container* particles, 
					     std::vector<const Contained*>& objs, std::vector<unsigned>& indices){
      typename Container::const_iterator ipItr = particles->begin();
      typename Container::const_iterator ipEnd = particles->end();
      unsigned ipart=0;
      for(; ipItr != ipEnd; ++ipItr) {
	  if(selparts->isGood(ipart)){
	      objs.push_back(*ipItr);
	      indices.push_back(ipart);
	  }
	  ipart++;
      }
  }

  template<class Container>
  void SelectedParticlesUtil::getParticleLinks(const SelectedParticles* selparts, const Container* particles,
			    ParticleLinks<Container>& links){
      typename Container::const_iterator ipItr = particles->begin();
      typename Container::const_iterator ipEnd = particles->end();
      links.reserve(selparts->numGood());
      unsigned ipart=0;
      for(; ipItr != ipEnd; ++ipItr) {
	  if(selparts->isGood(ipart++)){
	      ElementLink<Container> el(*particles,ipart);
	      links.push_back(el);
	  }
      }
  }

  template<class Container>
      void SelectedParticlesUtil::getSelectedParticles(const ParticleLinks<Container>* links, SelectedParticles& selparts){
      typedef ParticleLinks<Container> Links;
      selparts.SetMaxBits(links->size());
      typename Links::const_iterator ilItr = links->begin();
      typename Links::const_iterator ilEnd = links->end();
      for(; ilItr != ilEnd; ++ilItr) {
	  selparts.SetBit((*ilItr).index());      
      }

  }

#endif // !PARTICLEEVENT_SELECTEDPARTICLESUTIL_H

