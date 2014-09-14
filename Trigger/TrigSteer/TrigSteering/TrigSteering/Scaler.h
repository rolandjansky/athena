/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Scaler
 *
 * @brief : class to generate decisions for chain prescale states based on the prescale factors,
 *          and similarly for pass-through states
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Scaler.h,v 1.9 2008-10-21 06:44:24 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_Scaler_H
#define TRIGSTEERING_Scaler_H

#include "AthenaBaseComps/AthAlgTool.h"

namespace HLT {

  /** @class IScaler
      interface class for all scaler classes
  */
  class IScaler {

  public:
    virtual ~IScaler(){}
    /** @brief return prescale/pass-through decision, must be overridden.
	@param factor: the prescale or pass-through factor
    */
    virtual bool decision(float factor) = 0;

    /** @brief Reset scaler to initial state.
     */	
    virtual void reset() = 0;
  };

  /**
     @class Scaler
     Utility base class to determine if a chain should be prescaled/passed-through using its PS/PT factor.

     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */
  class Scaler : virtual public IScaler {
  public:
    Scaler() {}
    virtual ~Scaler() {}
  };

  /**
     @class IReusableScaler
     interface for reusable scaler engines, ie the random engine can be used
     by many chains (is reusable), whereas the periodic engine cannot be re-used.
  */
  class IReusableScaler : virtual public IScaler, virtual public IAlgTool {
  public:
    virtual ~IReusableScaler(){}
    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_ReusableScaler("HLT::IReusableScaler", 1, 0);      
      return IID_ReusableScaler;
    }
  };

  /**
     @class ReusableScaler
     This is a tool, which can be shared (re-used) by many clients (HLT::Chain).
  */
  class ReusableScaler : virtual public IReusableScaler, public AthAlgTool {
  public:

    ReusableScaler( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
      : AthAlgTool( type, name, parent ) {
      declareInterface<HLT::IReusableScaler>( this );
    }
    virtual ~ReusableScaler(){}
  };

}

#endif
