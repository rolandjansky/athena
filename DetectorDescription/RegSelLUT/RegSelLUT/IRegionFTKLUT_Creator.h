// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRegionFTKLUT_Creator.h,v 1.1 2008-05-15 21:24:17 sutt Exp $
#ifndef IREGIONFTKLUT_CREATOR_H
#define IREGIONFTKLUT_CREATOR_H

#include "GaudiKernel/IAlgTool.h"

#include "RegSelLUT/RegSelEtaPhiLUT.h"

/// Interface class to create RegionSelector look up table
/// for extent of elements for new RegSelSiLUT classes.
/// FIXME: would be better done as a template rather than have
///        two very similar classes, but I couldn't get it to 
///        work. maybe it was somthing to do with the InterfaceID.
///        hmmm 

static const InterfaceID IID_IRegionFTKLUT_CreatorID ("IRegionFTKLUT_CreatorID",1,0);


class IRegionFTKLUT_Creator : virtual public IAlgTool 
{

public:
  
  virtual ~IRegionFTKLUT_Creator() {}
  
  static const InterfaceID& interfaceID() { return IID_IRegionFTKLUT_CreatorID; }   /// The AlgTool InterfaceID
  virtual StatusCode initialize()=0;     /// standard AlgTool method
  virtual StatusCode finalize()=0;       /// standard AlgTool method

  /// abstract base method for retrieving the table
  virtual RegSelEtaPhiLUT* getLUT() const = 0;

};

#endif // IREGIONFTKLUT_CREATOR_H


