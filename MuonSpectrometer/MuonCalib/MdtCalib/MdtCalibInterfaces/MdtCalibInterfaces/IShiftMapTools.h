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

namespace MuonCalib {

/**
   @class IShiftMapTools
   Interface for the ShiftMapTools inheriting from MdtCalibrationShiftMapBase

   @author der.andi@cern.ch
*/

class IShiftMapTools : virtual public IService {
 public:
  IShiftMapTools() { ; }
  virtual ~IShiftMapTools() = 0;

  // Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IShiftMapTools, 1, 0);

  /* initialization */
  virtual StatusCode initialize();

  /* finalization */
  virtual StatusCode finalize();

  /*
   * initalization of map cannot happen before first event
   * special function required
   */
  virtual StatusCode initializeMap() = 0;

  /* dump the map in binary file, given a path */
  virtual StatusCode dumpMapAsFile() = 0;

  /* load the map from a binary file, given a path */
  virtual StatusCode loadMapFromFile() = 0;

  /* get shift */
  virtual float getValue(const Identifier& id) = 0;
};

}  // namespace MuonCalib

#endif
