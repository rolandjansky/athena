//==============================================================================
// FexDefs.h
//==============================================================================
/*
 *  Class to define constants common to all three FEX modules.
 */

#ifndef FexDefs_H
#define FexDefs_H

class FexDefs {
public:
  // Here "ribbon" is what goes to a minipod.
  static int numFibresPerRibbon() { return 12; }
  
  // For 11.2 Gbit/s.
  static int num32BitWordsPerFibre() { return 7; }
  
  // N. BCs of playback memory.
  // **FIXME** This is very likely wrong!
  // **FIXME** And may be different on each FEX.
  static int inputPlaybackDepth() { return 256; }
};

#endif  // FexDefs_H
