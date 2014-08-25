// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRegionLUT_Creator.h,v 1.2 2008-05-14 13:25:23 sutt Exp $
#ifndef IREGIONLUT_CREATOR_H
#define IREGIONLUT_CREATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "RegionSelector/RegionSelectorLUT.h"


/// Interface class to create RegionSelector look up table
/// for extent of elements.

static const InterfaceID IID_IRegionLUT_CreatorID("IRegionLUT_CreatorID",1,0);

class IRegionLUT_Creator : virtual public IAlgTool 
{

public:
  
  virtual ~IRegionLUT_Creator() {}
  
  static const InterfaceID& interfaceID() { return IID_IRegionLUT_CreatorID; }   /// The AlgTool InterfaceID
  virtual StatusCode initialize()=0;     /// standard AlgTool method
  virtual StatusCode finalize()=0;       /// standard AlgTool method

  /// abstract base method for retrieving the table
  virtual const RegionSelectorLUT* getLUT(std::string syst="") const = 0;

};

#endif // IREGIONLUT_CREATOR_H


