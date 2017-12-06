/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file RtRelationMultChanContainer.h 
 
 @brief 
 Class for storing/accessing trt rtrelations with channels 
1;95;0c
 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/

// $Id: RtRelationMultChanContainer.h,v 1.3 2007-06-13 11:10:43 hansenph Exp $

#ifndef TRT_CONDITIONSDATA_RTRELATIONMultChanCONTAINER_H
#define TRT_CONDITIONSDATA_RTRELATIONMultChanCONTAINER_H

#include "TRT_ConditionsData/RtRelationContainer.h"
#include "TRT_ConditionsData/MultChanContainer.h"
#include "AthenaKernel/CondCont.h"

namespace TRTCond
{
  typedef NestedContainer<ExpandedIdentifier::LAYERWHEEL,RtRelation*, RtRelationTrait> RtRelationLayerContainer ;

  /** @class RtRelationMultChanContainer
    Multichannel container for RtRelation objects */
  class RtRelationMultChanContainer : public MultChanContainer<RtRelationLayerContainer>
  {
  public:
    /** typedef */
    typedef RtRelationLayerContainer::FlatContainer FlatContainer ;
    /** default constructor */
    RtRelationMultChanContainer() {}
    /** copy constructor */
    RtRelationMultChanContainer(const RtRelationMultChanContainer& rhs) ;
    /** destructor */
    virtual ~RtRelationMultChanContainer() ;
    /** assignment */
    RtRelationMultChanContainer& operator=(const RtRelationMultChanContainer& rhs) ;
    /** classname used to talk to iovservice */
    static const char* classname() { return "TRTCond::RtRelationMultChanContainer" ; }
  private:
  } ;
}


CLASS_DEF(TRTCond::RtRelationMultChanContainer,1169148363,1)
CONDCONT_DEF(TRTCond::RtRelationMultChanContainer,163845982);


#endif
