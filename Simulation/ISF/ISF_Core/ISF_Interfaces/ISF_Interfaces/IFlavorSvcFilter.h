/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFlavorSvcFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IFLAVORSVCFILTER_H
#define ISF_INTERFACES_IFLAVORSVCFILTER_H 1

// Simulation includes
#include "ISF_Event/SimSvcID.h" 
 
namespace ISF {

  class StackParticle;
    
  /**
   @class IFlavorSvcFilter
    
   The base class of flavor service filters, allows to also create
   dynamic flavor service filters that are no AlgTools
   
   The FlavorSvcFilter registers the nextGeoSvcID to the StackParticle.
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IFlavorSvcFilter {
     public:
       /** Virtual destructor */
       virtual ~IFlavorSvcFilter(){}
       
       /** A Flavor filter that returns the SimSvcFlavorID : either flavorID or fNone */
       virtual SimSvcFlavorID flavorID(const ISFParticle& sp) const = 0;
       
       /** AThe flavorID for this filter*/
       virtual SimSvcFlavorID flavorID() const = 0;

  };

} // end of namespace

#endif // ISF_INTERFACES_IFLAVORSVCFILTER_H
