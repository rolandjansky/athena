/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @file StrawStatusContainer.h 
// 
// @brief 
// Class for storing/accessing trt straw status data
//
// @author Esben Klinkby <klinkby@nbi.dk>
//
//

#ifndef TRT_CONDITIONSDATA_STRAWSTATUSCONTAINER_H
#define TRT_CONDITIONSDATA_STRAWSTATUSCONTAINER_H

#include <string>
#include "AthenaKernel/CLASS_DEF.h"
#include "TRT_ConditionsData/NestedContainer.h"
#include "TRT_ConditionsData/StrawStatus.h"


namespace TRTCond
{
  
  /// trait class to deal with StrawStatus in NestedContainers

  class DeadStrawTrait
  {
  public:
    static void erase( StrawStatus& x) { x.setstatus(StrawStatus::Undefined) ; }
    static void copy( StrawStatus& out, const StrawStatus& in) { out = in ; }
    static StrawStatus initialvalue() { return StrawStatus() ; }
    
    static bool isvalid( const StrawStatus& x ) { return x.getstatus()!=StrawStatus::Undefined ;}
    static size_t footprint( const StrawStatus& x) { return sizeof(x) ; }
    static void print(const StrawStatus& x) { x.print() ; }
    static bool isequal(const StrawStatus& x, const StrawStatus& y) { return x==y ; }
  };


  template <int LEVEL>
    class StrawStatusContainerTemplate : public NestedContainer<LEVEL, StrawStatus,DeadStrawTrait>
    {
    private:
    
    public:
    /// constructor
    StrawStatusContainerTemplate() {}

    /// destructor
    virtual ~StrawStatusContainerTemplate() {}

    /// set status
    void setStatus(const ExpandedIdentifier& id, unsigned int status) {
      NestedContainer<LEVEL,StrawStatus,DeadStrawTrait>::set(id,status) ;
      // what is called here...?
    }
    
    };
  class StrawStatusContainer : public StrawStatusContainerTemplate<ExpandedIdentifier::DETECTOR> 
    {
    } ;
}

CLASS_DEF(TRTCond::StrawStatusContainer,1222305359,1) //correct

#endif
