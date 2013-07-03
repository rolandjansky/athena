/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TileIdentifier Package

This package provides helper classes for TileCal identifiers

@author Alexander Solodkov <Sanya.Solodkov@cern.ch>

TileHWID helper class is the class to build TileCal online identifiers
which are used to identify individual raw channels and ADCs.

TileTBID is the helper class to identify additional ancillary detectors
at the testbeam as well as 32 MBTS counters.

TileFragHash creates fragment ID hash for Identifiable Containers like
TileRawChannelContainer, TileDigitsContainer, TileBeamElemContainer (see TileEvent package)
This hash is used as a key in StoreGage for every collection of Identifiable Container.
To have more than one TileRawChannelContainers in StoreGate, the hash can be
built with an offset, which is determined by TileFragHash::TYPE.
Addional offset is calculated as m_offset = (int)TYPE * 0x1000;

TileRodIdHash provides conversion between fragment ID hash (defined in TileFragHash)
and ROB ID in ByteStream file. It is used by BS conversion service to provide
pointer to correct ROB fragment when fragment with certain hash ID need to be unpacked

TileTTL1Hash builds the hash ID table for all Level 1 Trigger Towers

TileTBFrag.h header file contains fragment IDs of all possible fragments which
can appear in "ancillary ROD" fragment - all ancillary detectors associated
with TileCal, e.g. Muon Wall at the testbeam or Trigger Boards in cosmic data

*/
