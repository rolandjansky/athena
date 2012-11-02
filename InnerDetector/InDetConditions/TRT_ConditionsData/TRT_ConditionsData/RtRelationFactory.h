/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCond_RtRelationFactory_h
#define TRTCond_RtRelationFactory_h

#include "TRT_ConditionsData/BasicRtRelation.h"
#include "TRT_ConditionsData/DinesRtRelation.h"
#include "TRT_ConditionsData/BinnedRtRelation.h"

namespace TRTCond
{
  
  /** @class RtRelationFactory
      construct RtRelation from text file. Write text file.
  */
  class RtRelationFactory
  {
  public:
    /** enumerates 3rd deg polynomial coefficients or table */
    enum RtRelationType {EBasicRtRelation, EBinnedRtRelation, EDinesRtRelation} ;
    /** constructor */
    RtRelationFactory() ;
    /** destructor */
    ~RtRelationFactory() ;
    /** read method */
    static RtRelation* readFromFile(std::istream& is) ;
    /** write method */
    static void writeToFile(std::ostream& os, const RtRelation& rt) ;
  } ;
  
  inline RtRelation* RtRelationFactory::readFromFile(std::istream& is) {
    RtRelation* rt(0) ;
    int type ;
    is >> type ;
    switch(type) {
    case EBasicRtRelation:
      rt = new BasicRtRelation() ;
      is >> *rt ;
      break; 
    case EBinnedRtRelation:
      rt = new BinnedRtRelation() ;
      is >> *rt ;
      break;
    case EDinesRtRelation:
      rt = new DinesRtRelation() ;
      is >> *rt ;
      break; 
    }
    return rt ;
  }
  
  inline void RtRelationFactory::writeToFile(std::ostream& os, const RtRelation& rt) {
    int type(-1) ;
    if(      dynamic_cast<const BasicRtRelation*>(&rt) )  type=EBasicRtRelation ;
    else if( dynamic_cast<const BinnedRtRelation*>(&rt) ) type=EBinnedRtRelation ;
    else if( dynamic_cast<const DinesRtRelation*>(&rt) ) type=EDinesRtRelation ;
    os << type << " " << rt ;
  }
  
}


#endif
