/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Interface to the Tool responsible for wrapping ATLAS identifier to Standalone Muon Fixed Identifier
 * --------------------------------------
 *
 * Author       : Zdenko van Kesteren
 * Creation Date: 20 May 2005
 * Last Update  : 20 May 2005
 ***************************************************************************/

#ifndef MUONCALIB_IIDTOFIXEDID_H
#define MUONCALIB_IIDTOFIXEDID_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "Identifier/Identifier.h"
#include <string>

namespace MuonCalib {

/** 
  @class IIdToFixedIdTool
  Interface for tools performing the conversion between Athena Identifiers
  and Muon Calibration Standalone Fixed ones.
*/

  static const InterfaceID IID_IIdToFixedIdTool( "MuonCalib::IdToFixedIdTool", 1, 0 );

  class IIdToFixedIdTool: virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() { return IID_IIdToFixedIdTool; } //!< retrieve Interface ID
    virtual ~IIdToFixedIdTool(){}                                            //!< virtual IAlgTool destructor
    
    virtual void print(const Identifier& id) const = 0;               //!< pure virtual member to print (ATHENA) Identifier 
    virtual void print(const MuonFixedId& fid) const = 0;             //!< pure virtual member to print (Calib-EDM) MuonFixedId 
    virtual Identifier fixedIdToId(const MuonFixedId& fid) const = 0; //!< pure virtual member to convert (Calib-EDM) MuonFixedId to (ATHENA) Identifier
    virtual MuonFixedId idToFixedId(const Identifier& id) const = 0;  //!< pure virtual member to convert (ATHENA) Identifier to (Calib-EDM) MuonFixedId
    virtual Identifier regionKeyToId(std::string key) const = 0;      //!< pure virtual member to retrieve an (ATHENA) Identifier for a given regionKey
  };
}
#endif
