/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// IShiftMapTools.h
//   Header file for class IShiftMapTools
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// der.andi@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef ISHIFTMAPTOOLS_H
#define ISHIFTMAPTOOLS_H

// Include files
// from Gaudi
#include "GaudiKernel/IService.h"

// forward declarations
class Identifier;

namespace MuonCalib {

/**
   @class IShiftMapTools
   Interface for the ShiftMapTools inheriting from MdtCalibrationShiftMapBase

   @author der.andi@cern.ch
*/

class IShiftMapTools : virtual public IService {
 public:
  IShiftMapTools() { ; }
  virtual ~IShiftMapTools() { ; }

  /* Creates the InterfaceID and interfaceID() method */
  DeclareInterfaceID(IShiftMapTools, 1, 0);

  /* get shift value */
  virtual float getValue(const Identifier& id) = 0;
};

}  // namespace MuonCalib

#endif
