/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IREGIONLUT_CREATOR_H
#define IREGIONLUT_CREATOR_H

#include "GaudiKernel/IAlgTool.h"

class RegionSelectorLUT;

static const InterfaceID IID_IRegionLUT_CreatorID ("IRegionLUT_CreatorID",1,0);

/// Interface class to create RegionSelector look up table
/// for extent of elements.

class IRegionLUT_Creator : virtual public IAlgTool 
{

public:
  
  virtual ~IRegionLUT_Creator() {}
  
  static const InterfaceID& interfaceID() 
    {return IID_IRegionLUT_CreatorID;}   /// The AlgTool InterfaceID
  virtual StatusCode initialize()=0;     /// standard AlgTool method
  virtual StatusCode finalize()=0;       /// standard AlgTool method

  /// abstract base method for retrieving the table
  virtual const RegionSelectorLUT * getLUT(std::string syst="") const = 0;

};

#endif // IREGIONLUT_CREATOR_H


