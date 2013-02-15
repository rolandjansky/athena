/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TileRawUtils Package

This package provides tables for region HLT region selectors

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>
@author Ambreesh Gupta <Ambreesh Gupta@cern.ch>

HLT region selector need to know correspondance between ROB ID and
part of the detector in eta-phi space to unpack appropriate ByteStream fragment
when ceratain part of the detector is picked up in Region Of Interest (ROI)
This mapping is provided by TileRegionSelectorTable class.

TileRawDataOrdering and TileLogicalOrdering are used by TileByteStream package
to order all RawData objects according to hardware (online) and logical (offline)

*/
