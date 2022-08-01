//==============================================================================
// EfexDefs.h
//==============================================================================
/*
 *  Class to define eFEX constants.
 */

#ifndef EfexDefs_H
#define EfexDefs_H

#include "defsL1Calo/FexDefs.h"

class EfexDefs {
public:
  // Here "ribbon" is what goes to a minipod.
  static int numFibresPerRibbon() { return FexDefs::numFibresPerRibbon(); }
  static int numRibbonsPerConnector() { return 4; }
  static int numFibresPerConnector() { return numFibresPerRibbon() * numRibbonsPerConnector(); }
  
  // For the moment this ignores the spare connector.
  static int numInputConnectors() { return 4; }
  static int numInputRibbons() { return numInputConnectors() * numRibbonsPerConnector(); }
  static int numInputFibres() { return numInputRibbons() * numFibresPerRibbon(); }
  // NB what if not all input ribbons are fully used (which seems likely)?
  static int numInputChannels() { return numInputFibres(); }
  
  static int maxSuperCellsPerFibre() { return 20; }
  static int num32BitWordsPerFibre() { return FexDefs::num32BitWordsPerFibre(); }
  static int numBitsPerSuperCell() { return 10; }  // TODO: CHECK THIS!!
  
  static int numOutputRibbons() { return 2; }  // TODO: CHECK THIS!!
  static int numOutputFibres() { return numOutputRibbons() * numFibresPerRibbon(); }

  static int nProcessorFpgas() { return 4; }
  static int numInputFibresPerFpga() { return 49; }  // 40 EM + 9 Hadronic (total number actually in use)
  static int numInputMgtsPerFpga() { return 80; }    // NB not all are used, but index is 0-79 for all FPGAs
  static int numQuadsPerFpga() {return 20; } 
  static int numMgtsPerQuad()  {return 4; } 

  static int nCaloReadoutStreams() { return 4; }  // Sent separately
  static int nTobReadoutStreams()  { return 1; }  // Merged in control FPGA
  
  // Maximum number of eta bins. NB only one end FPGA uses more than four
  // (and the TOB coordinate may be 0-4 or 1-5 in those cases).
  static int maxEtaBinsPerFpga() { return 5; }
  static int numEtaBinsPerModule() { return 17; }
  static int maxEtaBinsPerModule() { return maxEtaBinsPerFpga() * nProcessorFpgas(); }
  
  // N. BCs of playback memory. **FIXME** This is very likely wrong!
  static int inputPlaybackDepth() { return FexDefs::inputPlaybackDepth(); }
  static int roSpyMemoryDepth() { return 0x800; }

  // Mapping between minipods and MTP connectors. **FIXME** This is very likely wrong!
  static int connectorForMinipod(int minipod);
  static int connectorFirstFibre(int minipod);
  
  // Methods for converting internal to global eta.
  static unsigned int globalEtaIndex(unsigned int efexLogicalID,
                                     unsigned int fpgaNumber,
                                     unsigned int etaWithinFpga);
  static          int globalEtaValue(unsigned int efexLogicalID,
                                     unsigned int fpgaNumber,
                                     unsigned int etaWithinFpga);
};

#endif  // EfexDefs_H
