/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileCalibBlobObjs_page TileCalibBlobObjs

This package provides classes able to create and interpret TileCalorimeter conditions data stored in COOL BLOBs.

@author Nils Gollub <Nils.Gollub@cern.ch>

@section TileCalibBlobObjs_Package_Structure Package Structure

Tile conditions data is stored in COOL folders. Within a folder,
conditions data is stored in units of TileCal drawers. Etc..

The TileCalibDrawer classes always act on an existing coral::Blob
obtained by reference from a coral::AttributeList. In order to
guarantee correct behaviour towards const and non-const coral::Blobs,
all TileCalibDrawer class constructors are protected. An instance of a
TileCalibDrawer class can be obtained by the respective static
getInstance(...) method, which returns a const pointer to the
TileCalibDrawer class in case a const coral::Blob reference is passed
as argurment. This prevents TileCalibDrawer classes from modifying
const coral::Blobs.

 

*/
