/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-


//This construct is a TEMORARY solution to allow the clients of LArRawChannel to compile 
//in 13.X.Y as well as in 14.0.0 despite of the schema evolution of LArRawChannelContainer
//This file should go away as soon as the migration towards Rel 14 is finsihed.

#ifndef LARMONTOOLS_SELECTALLLARRAWCHANNELS_H
#define LARMONTOOLS_SELECTALLLARRAWCHANNELS_H

#include "LArRawEvent/LArRawChannelContainer.h"

#ifdef LARRAWCHANNELCONTAINER_IS_NOT_IDENTIFIABLE

class SelectAllLArRawChannels {
 public:
  SelectAllLArRawChannels(const LArRawChannelContainer* rc) : m_container(rc) {}

  class const_iterator : public LArRawChannelContainer::const_iterator {
  public:
    const_iterator() : LArRawChannelContainer::const_iterator() {}
    const_iterator(LArRawChannelContainer::const_iterator it) : LArRawChannelContainer::const_iterator(it) {}
    const LArRawChannel* operator*() {return &(LArRawChannelContainer::const_iterator::operator*());}
    const LArRawChannel operator->() {return (LArRawChannelContainer::const_iterator::operator*());}
  };

  const_iterator begin() 
    { return const_iterator(m_container->begin()); }
  
  const_iterator end() 
    { return const_iterator(m_container->end()); }

 private:
  const LArRawChannelContainer* m_container;

};
#else

#include "EventContainers/SelectAllObject.h" 
typedef SelectAllObject<LArRawChannelContainer> SelectAllLArRawChannels; 

#endif //LARRAWCHANNELCONTAINER_IS_NOT_IDENTIFIABLE
#endif //LARMONTOOLS_SELECTALLLARRAWCHANNELS_H
