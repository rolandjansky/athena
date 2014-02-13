/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonTofTool.h
//   Header file for class MuonPattern
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUON_IMUONTOFTOOL_H
#define MUON_IMUONTOFTOOL_H

/** 
    @class IMuonTofTool

    Interface class for tools calculating time of flight for a given Identifier and position

    @author Niels.Van.Eldik@cern.ch
*/

#include "GaudiKernel/IAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"

class Identifier;

namespace Muon {

  /** Interface ID for IMuonCalibTool*/  
  static const InterfaceID IID_IMuonTofTool("Muon::IMuonTofTool", 1, 0);

  class IMuonTofTool : virtual public IAlgTool {
  public:
     /**Virtual destructor*/
    virtual ~IMuonTofTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMuonTofTool; }
    
    /** Calculate time of flight from global position  */
    virtual double timeOfFlight( const Identifier& id, const Amg::Vector3D& pos ) const = 0;
    
  };

}

#endif
