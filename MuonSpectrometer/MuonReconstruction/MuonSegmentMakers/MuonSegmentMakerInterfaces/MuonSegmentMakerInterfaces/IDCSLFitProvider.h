/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IDCSLFITPROVIDER
#define MUON_IDCSLFITPROVIDER

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IDCSLFitProvider
    ("Muon::IDCSLFitProvider",1,0);

namespace TrkDriftCircleMath {
  class DCSLFitter;
}

namespace Muon {

  /** Interface for tools resolving ambigueties and remove outliers in Muon::MuonSegment objects */
  class IDCSLFitProvider : virtual public IAlgTool 
  {      
    public:
    
    static const InterfaceID& interfaceID();

    /** @brief fitter factory
	@return provides pointer to fitter, ownsership not passed to client
    */
    virtual TrkDriftCircleMath::DCSLFitter* getFitter() = 0;

  };
  
  inline const InterfaceID& IDCSLFitProvider::interfaceID()
  {
    return IID_IDCSLFitProvider;
  }
} // end of name space

#endif // IDCSLFitProvider_H

