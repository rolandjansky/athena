/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
*  RoIBDefs contains some definitions used throughout the
*  RoIB algorithms (and throughout the L1 ROD world). However,
*  most of the defs might be superflous now due to the use of the
*  eformat lib.
*  Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
*
********************************************************** */

#ifndef TRIGT1ROIB_ROIBDEFS_H
#define TRIGT1ROIB_ROIBDEFS_H

namespace ROIB {

  /* All detector RODs have 15 words, if empty. This counts also
     the 1word in the beginning and end which is later stripped of. */
  static const unsigned int wordsPerEmptyEMTauSlink = 15;   // As agreed with E. Moyse.
  /* 4 egamma slinks */
  static const unsigned int numEMTauSlinks = 4;             // L1 definition.
  /* 2 jet energy slinks */
  static const unsigned int numJetEnergySlinks = 2;         // L1 definition.
  /* a rod header as used in the l1 has 8 words, not c0ounting the one
     hardware word in the beginning */
  static const unsigned int wordsPerHeader = 8;             // ATL-DA-ES-0029.
  /* a rod trailer as used in the l1 has 5 words, not counting the one
     hardware word in the end */
  static const unsigned int wordsPerTrailer = 5;	    // ATL-DA-ES-0029.

}

#endif // TRIGT1ROIB_ROIBDEFS_H
