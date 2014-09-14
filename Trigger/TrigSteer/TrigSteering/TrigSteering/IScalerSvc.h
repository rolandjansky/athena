/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteering_IScalerSvc_h
#define TrigSteering_IScalerSvc_h

#include "GaudiKernel/IService.h"

namespace HLT {

  class IScaler;

  /**
     @class IScalerSvc
     interface for the Scaler services
  */
  class IScalerSvc : virtual public IService {

  public:

    static const InterfaceID& interfaceID(); //!< std Gaudi interface declaration

    virtual IScaler* get( const std::string& type = "" ) const = 0; //!< abstract: get a scaler object from given string

  }; // class IScalerSvc

} // namespace HLT


inline const InterfaceID& HLT::IScalerSvc::interfaceID() {
  static const InterfaceID IID_IScalerSvc( "IScalerSvc", 0, 1 );
  return IID_IScalerSvc;
}

#endif
