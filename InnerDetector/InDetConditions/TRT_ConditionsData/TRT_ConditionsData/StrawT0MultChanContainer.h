/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @file StrawT0MultChanContainer.h
//
// @brief
// Class for storing/accessing trt t0 data with channels
//
// @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
//
// $Id: StrawT0MultChanContainer.h,v 1.5 2007-06-13 11:10:44 hansenph Exp $

#ifndef TRTCONDITIONSDATA_STRAWT0MULTCHANCONTAINER_H
#define TRTCONDITIONSDATA_STRAWT0MULTCHANCONTAINER_H

#include "TRT_ConditionsData/StrawT0Container.h"
#include "TRT_ConditionsData/MultChanContainer.h"
#include "AthenaKernel/CondCont.h"

namespace TRTCond 
{
  typedef StrawT0ContainerTemplate<ExpandedIdentifier::LAYERWHEEL> StrawT0LayerContainer ;
  
  /** @class StrawT0MultChanContainer
     A COOL Multichannel container for StrawT0 objects
  */
  class StrawT0MultChanContainer : public MultChanContainer<StrawT0ContainerTemplate<ExpandedIdentifier::LAYERWHEEL> >
  {
  public:
    /// typedefs
    typedef StrawT0ContainerTemplate<ExpandedIdentifier::LAYERWHEEL>::FlatContainer FlatContainer ;

    /// constructor
    StrawT0MultChanContainer()  {}

    /// destructor
    virtual ~StrawT0MultChanContainer() {}

    /// access to unpacked t0
    float getT0(const ExpandedIdentifier& id) const { 
      ContainerWithValue valwithcontainer = getWithContainer( id ) ;
      return StrawT0Trait::isvalid(*valwithcontainer.second) ? valwithcontainer.first->unpackT0( *valwithcontainer.second ) : 0 ;
    }

    /// access to unpacked t0 error
    float getT0Err(const ExpandedIdentifier& id) const {
      ContainerWithValue valwithcontainer = getWithContainer( id ) ;
      return StrawT0Trait::isvalid(*valwithcontainer.second) ? valwithcontainer.first->unpackT0Err( *valwithcontainer.second ) : 0 ;
    }
    
    /// set t0
    void setT0(const ExpandedIdentifier& id, float t0, float t0err) {
      if( id.level()==ExpandedIdentifier::DETECTOR ) 
	findContainer(id)->setT0(t0,t0err) ; 
      else if( id.level()==ExpandedIdentifier::BARRELEC ) 
	std::cout << "Sorry: TRTCond::StrawT0MultChanContainer cannot store containers at BARREL_EC granularity" << std::endl ;
      else 
	findContainer(id)->setT0( id, t0,t0err) ;
    }
    
    /// public method to unpack a StrawT0 object
    void unpack(const ExpandedIdentifier& id, const StrawT0& sd, float& t0 , float& t0err) const {
      const StrawT0LayerContainer* container = getContainer(channelId(id)) ;
      if(container==0) std::cout << "ERRRRRRORRRRRR: cannot find container ..." << id << " " << channelId(id) << std::endl ;
      container->unpack( sd,t0,t0err ) ;
    }

    /// change the packing limits. NOTE: this only works for channels that already exist!
    void setPackingLimits( float t0min, float t0max, float t0errmin, float t0errmax) ;
    
    /// classname used to talk to iovservice
    static const char* classname() { return "TRTCond::StrawT0MultChanContainer" ; }
  } ;

  inline void StrawT0MultChanContainer::setPackingLimits( float t0min, float t0max, float t0errmin, float t0errmax) 
  {
    CondMultChanCollection<StrawT0ContainerTemplate<ExpandedIdentifier::LAYERWHEEL> >::iterator dauit ;
    for(dauit = this->begin() ; dauit != this->end(); ++dauit) 
      (*dauit)->setPackingLimits( t0min,t0max,t0errmin,t0errmax ) ;
  }
}


CLASS_DEF(TRTCond::StrawT0MultChanContainer,1178829403,1)
CONDCONT_DEF(TRTCond::StrawT0MultChanContainer,182150756);


#endif
