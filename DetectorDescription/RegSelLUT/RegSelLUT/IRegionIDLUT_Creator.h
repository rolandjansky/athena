// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRegionIDLUT_Creator.h,v 1.1 2008-05-15 21:24:17 sutt Exp $
#ifndef IREGIONIDLUT_CREATOR_H
#define IREGIONIDLUT_CREATOR_H

#include "GaudiKernel/IAlgTool.h"

#include "RegSelLUT/RegSelSiLUT.h"

/// Interface class to create RegionSelector look up table
/// for extent of elements for new RegSelSiLUT classes.
/// FIXME: would be better done as a template rather than have
///        two very similar classes, but I couldn't get it to 
///        work. maybe it was somthing to do with the InterfaceID.
///        hmmm 

static const InterfaceID IID_IRegionIDLUT_CreatorID ("IRegionIDLUT_CreatorID",1,0);


class IRegionIDLUT_Creator : virtual public IAlgTool 
{

public:
  
  virtual ~IRegionIDLUT_Creator() {}
  
  static const InterfaceID& interfaceID() { return IID_IRegionIDLUT_CreatorID; }   /// The AlgTool InterfaceID
  virtual StatusCode initialize()=0;     /// standard AlgTool method
  virtual StatusCode finalize()=0;       /// standard AlgTool method

  /// abstract base method for retrieving the table
  virtual RegSelSiLUT* getLUT() = 0;

};

#endif // IREGIONIDLUT_CREATOR_H


