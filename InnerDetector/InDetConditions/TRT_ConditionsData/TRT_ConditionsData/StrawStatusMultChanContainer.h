/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CONDITIONSDATA_STRAWSTATUSMultChanCONTAINER_H
#define TRT_CONDITIONSDATA_STRAWSTATUSMultChanCONTAINER_H

#include "TRT_ConditionsData/StrawStatusContainer.h"
#include "TRT_ConditionsData/MultChanContainer.h"
#include "AthenaKernel/CondCont.h"

namespace TRTCond{
  /// typedef the channels as NestedContainer
  typedef StrawStatusContainerTemplate<ExpandedIdentifier::LAYERWHEEL> StrawStatusLayerContainer ; 
  
  /** @class StrawStatusMultChanContainer
      A COOL multichannel container for StrawStatus */
  class StrawStatusMultChanContainer : public MultChanContainer<StrawStatusContainerTemplate<ExpandedIdentifier::LAYERWHEEL> >
    {
      // pasted from RtRelationMultChanContainer
    public:
      //typedefs
      typedef StrawStatusContainerTemplate<ExpandedIdentifier::LAYERWHEEL>::FlatContainer FlatContainer ; 
      /// default constructor
      StrawStatusMultChanContainer() {}
      /// destructor
      virtual ~StrawStatusMultChanContainer() {} // needed to destruct owned pointers.
      /// classname used to talk to iovservice
      static const char* classname() { return "TRTCond::StrawStatusMultChanContainer" ; }

                                                                                                                                                             
      /// set t0
      void setStatus(const ExpandedIdentifier& id, unsigned int status) {
	if( id.level()==ExpandedIdentifier::DETECTOR )
	  findContainer(id)->setStatus(id,status) ;
	else if( id.level()==ExpandedIdentifier::BARRELEC )
	  std::cout << "Sorry: TRTCond::StrawStatusMultChanContainer cannot store containers at BARREL_EC granularity" << std::endl ;
	else
	  findContainer(id)->setStatus( id, status) ;
      }
                                                                                                                                                             

    private:
    } ;
  
}


CLASS_DEF(TRTCond::StrawStatusMultChanContainer,1112780409,1)
CONDCONT_DEF(TRTCond::StrawStatusMultChanContainer,1335119793);


#endif
