/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITRT_ELECTRONTOTTOOL_H
#define ITRT_ELECTRONTOTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Trk { class Track ; } 
namespace Trk { class TrackStateOnSurface ; } 

static const InterfaceID IID_ITRT_ElectronToTTool("ITRT_ElectronToTTool", 1, 0);
  
class ITRT_ElectronToTTool : virtual public IAlgTool {
  
 public:

  /** Virtual destructor */
  virtual ~ITRT_ElectronToTTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITRT_ElectronToTTool; };
  
  // get the ToT from the bitpattern and correct for local variations
  //virtual double GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer) const = 0;
  
  // get the distance used to normalize the ToT
  //virtual double GetD(double R_track) const = 0;

};

#endif // ITRT_ELECTRONTOTTOOL_H
