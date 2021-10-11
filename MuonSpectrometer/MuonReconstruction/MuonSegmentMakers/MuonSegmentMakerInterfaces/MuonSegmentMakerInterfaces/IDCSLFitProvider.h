/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IDCSLFITPROVIDER
#define MUON_IDCSLFITPROVIDER

#include "GaudiKernel/IAlgTool.h"


namespace TrkDriftCircleMath {
  class DCSLFitter;
}

namespace Muon {

  /** Interface for tools resolving ambigueties and remove outliers in Muon::MuonSegment objects */
  class IDCSLFitProvider : virtual public IAlgTool {      
    public:
    
      static const InterfaceID& interfaceID(){
          static const InterfaceID IID_IDCSLFitProvider("Muon::IDCSLFitProvider",1,0);
         return IID_IDCSLFitProvider;
      }
      /** @brief fitter factory
        	@return provides pointer to fitter, ownsership not passed to client
      */
      virtual const TrkDriftCircleMath::DCSLFitter* getFitter() const = 0;
    /**
     * @brief default destructor
     * 
     */
     virtual ~IDCSLFitProvider() = default;

     /**
      * @brief Helper struct to overload the destructors of smart pointers
      * 
      */
     struct Unowned{
           void operator()(const TrkDriftCircleMath::DCSLFitter*) const {}  
      };

  };
  
  } // end of name space

#endif // IDCSLFitProvider_H

