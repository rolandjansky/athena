/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file StrawDxMultChanContainer.h

 @brief
 Class for storing/accessing trt ednpoint carrections with channels

 @author Peter Hansen <phansen@nbi.dk>
*/
// $Id: StrawDxMultChanContainer.h,v 1.2 2007-06-13 11:10:43 hansenph Exp $

#ifndef TRTCONDITIONSDATA_STRAWDXMULTCHANCONTAINER_H
#define TRTCONDITIONSDATA_STRAWDXMULTCHANCONTAINER_H

#include "TRT_ConditionsData/StrawDxContainer.h"
#include "TRT_ConditionsData/MultChanContainer.h"

namespace TRTCond 
{
  typedef StrawDxContainerTemplate<ExpandedIdentifier::LAYERWHEEL> StrawDxLayerContainer ;
  
  /** @class StrawDxMultChanContainer
    Multichannel container for StrawDx objects */
  class StrawDxMultChanContainer : public MultChanContainer<StrawDxContainerTemplate<ExpandedIdentifier::LAYERWHEEL> >
  {
  public:
    /** typedefs */
    typedef StrawDxContainerTemplate<ExpandedIdentifier::LAYERWHEEL>::FlatContainer FlatContainer ;

    /** constructor */
    StrawDxMultChanContainer()  {}

    /** destructor */
    virtual ~StrawDxMultChanContainer() {}

    /** access to unpacked dx near electronics */
    float getDx1(const ExpandedIdentifier& id) const { 
      ContainerWithValue valwithcontainer = getWithContainer( id ) ;
      return StrawDxTrait::isvalid(*valwithcontainer.second) ? valwithcontainer.first->unpackDx1( *valwithcontainer.second ) : 0 ;
    }
    /** access to unpacked dx far from electronics */
    float getDx2(const ExpandedIdentifier& id) const { 
      ContainerWithValue valwithcontainer = getWithContainer( id ) ;
      return StrawDxTrait::isvalid(*valwithcontainer.second) ? valwithcontainer.first->unpackDx2( *valwithcontainer.second ) : 0 ;
    }

    /** access to unpacked dx error */
    float getDxErr(const ExpandedIdentifier& id) const {
      ContainerWithValue valwithcontainer = getWithContainer( id ) ;
      return StrawDxTrait::isvalid(*valwithcontainer.second) ? valwithcontainer.first->unpackDxErr( *valwithcontainer.second ) : 0 ;
    }
    
    /** set dx */
    void setDx(const ExpandedIdentifier& id, float dx1, float dx2, float dxerr) {
      if( id.level()==ExpandedIdentifier::DETECTOR ) 
	findContainer(id)->setDx(dx1,dx2,dxerr) ; 
      else if( id.level()==ExpandedIdentifier::BARRELEC ) 
	std::cout << "Sorry: TRTCond::StrawDxMultChanContainer cannot store containers at BARREL_EC granularity" << std::endl ;
      else 
	findContainer(id)->setDx( id, dx1, dx2, dxerr) ;
    }
    
    /** method to unpack a StrawDx object */
    void unpack(const ExpandedIdentifier& id, float& dx1 , float& dx2, float& dxerr) const {
          const StrawDxLayerContainer* container = getContainer(channelId(id)) ;
      if(container==0) std::cout << "ERRRRRRORRRRRR: cannot find container ..." << id << " " << channelId(id) << std::endl ;
      dx1 = container->getDx1(id);
      dx2 = container->getDx2(id);
      dxerr = container->getDxErr(id);
    }

    /** classname used to talk to iovservice */
    static const char* classname() { return "TRTCond::StrawDxMultChanContainer" ; }
  } ;
  
}

CLASS_DEF(TRTCond::StrawDxMultChanContainer,1162639324,1)

#endif
