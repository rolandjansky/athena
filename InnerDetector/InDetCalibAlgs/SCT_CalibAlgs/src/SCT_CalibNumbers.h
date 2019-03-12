/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CalibNumbers_h
#define SCT_CalibNumbers_h
namespace SCT_CalibAlgs {

  // SCT specific numbers
  enum { nbins       = 6*128, firstStrip     =  0, lastStrip     = nbins-1,
         ntimeBins   =     3, firstTimeBin   =  0, lastTimeBin   =  2,
         n_hitBins   =    50, first_hitBin   =  0, last_hitBin   = 50,
         n_etaBins   =    13, first_etaBin   = -6, last_etaBin   =  6,
         n_phiBins   =    56, first_phiBin   =  0, last_phiBin   = 55,
         n_barrels   =     4, first_barrel   =  0, last_barrel   =  3,
         n_disks     =     9, first_disk     =  0, last_disk     =  8,
         n_etaBinsEC =     3, first_etaBinEC =  0, last_etaBinEC =  2,
         n_phiBinsEC =    52, first_phiBinEC =  0, last_phiBinEC = 51
  };
}

#endif
