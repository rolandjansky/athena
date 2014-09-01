/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IStaticFlavorSvcFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_ISTATICFLAVORSVCFILTER_H
#define ISF_INTERFACES_ISTATICFLAVORSVCFILTER_H 1

// Framework include
#include "GaudiKernel/IAlgTool.h"
// ISF Interfaces includes
#include "ISF_Interfaces/IFlavorSvcFilter.h"
 
namespace ISF {

  class StackParticle;
 
  static const InterfaceID IID_IStaticFlavorSvcFilter("IStaticFlavorSvcFilter", 1, 0);
   
  /**
   @class IStaticFlavorSvcFilter
    
   The static version of the FlavorSvc filter, realized as an AlgTool because
   this can be done at configuration level  
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IStaticFlavorSvcFilter : virtual public IAlgTool, public IFlavorSvcFilter {
     public:
     
       /** Virtual destructor */
       virtual ~IStaticFlavorSvcFilter(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IStaticFlavorSvcFilter; }

  };

} // end of namespace

#endif // ISF_INTERFACES_ISTATICFLAVORSVCFILTER_H
