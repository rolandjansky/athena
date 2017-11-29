/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file RtRelationContainer.h 
 
 @brief 
 Class for storing/accessing trt rtrelations. 

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: RtRelationContainer.h,v 1.3 2007-06-13 11:10:42 hansenph Exp $

#ifndef TRT_CONDITIONSDATA_RTRELATIONCONTAINER_H
#define TRT_CONDITIONSDATA_RTRELATIONCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TRT_ConditionsData/NestedContainer.h"
#include "TRT_ConditionsData/RtRelation.h"

namespace TRTCond
{
  typedef NestedContainerPointerTrait<RtRelation*> RtRelationTrait ;

  /** @class RtRelationContainer
     toplevel container for storing trt rt-relations
  */
  class RtRelationContainer : public NestedContainer<ExpandedIdentifier::DETECTOR, RtRelation*, RtRelationTrait>
  {
  public:
    /** default constructor */
    RtRelationContainer() {}
    /** copy constructor */
    RtRelationContainer(const RtRelationContainer& rhs) ;
    /** destructor */
    virtual ~RtRelationContainer() ;
    /** assignment */
    RtRelationContainer& operator=(const RtRelationContainer& rhs) ;
    /** classname used to talk to iovservice */
    static const char* classname() { return "TRTCond::RtRelationContainer" ; }
  private:
  } ;
}

CLASS_DEF(TRTCond::RtRelationContainer,1234815313,1)

#endif
