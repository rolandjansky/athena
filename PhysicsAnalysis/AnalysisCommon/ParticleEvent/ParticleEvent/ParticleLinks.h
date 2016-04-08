// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_PARTICLELINKS_H
#define PARTICLEEVENT_PARTICLELINKS_H 1

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ParticleLinks                                                    //
//                                                                      // 
// Store vector<ElementLink> of selected particles                      //
//                                                                      //
// S. Protopopescu 9 Dec. 2008                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////   Creation //////////////////////////////////////////////////////////////
//
//  ParticleLinks is a std::vector of ElementLink<Container>
//  there should be only one instance in a branch, one can have multiple versions per Container
//
//
///////   Useage ///////////////////////////////////////////////////
//
//  it inherits vector, so all vector methods are available
//
//  Additional methods:
//  isGood(i) will return true if ith particle was selected
//  isGood(ptr) will return true if pointer ptr is in ParticleLinks
//
//
////////////////////////////////////////////////////////////////////
#include <vector>
#include<string>
#include "AthLinks/ElementLink.h"
#include "SGTools/CLASS_DEF.h"

template<class Container>
class ParticleLinks: public  std::vector< ElementLink<Container> >
{
 public:

  ParticleLinks();
  virtual ~ParticleLinks(){;}
    // find if ith particle was selected
    bool isGood(unsigned i) const;
    // find if particle pointer ptr is in ParticleLinks
    //bool isGood(ElementConstPointer ptr) const;
 private:
};

template<class Container>
ParticleLinks<Container>::ParticleLinks():std::vector< ElementLink<Container> >(){;}

//   check by index

template<class Container>
bool ParticleLinks<Container>::isGood(unsigned i) const{
    typedef ParticleLinks<Container> Links;
    typename Links::const_iterator itr=this->begin();
    for(;itr!=this->end();++itr){
	if(i==(*itr).index()) return true;
    }
    return false;
}

#endif // !PARTICLEEVENT_PARTICLELINKS_H

