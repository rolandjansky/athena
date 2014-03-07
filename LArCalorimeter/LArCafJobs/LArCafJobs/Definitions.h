/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArSamples_Definitions_H
#define LArSamples_Definitions_H

#include "TMatrixTSym.h"

namespace LArSamples {
  typedef TMatrixTSym<double> CovMatrix;

  struct Definitions {
    static unsigned int nChannels;
    static unsigned int samplingInterval;
    static double samplingTime(unsigned int i) { return samplingInterval*i; }
    static double none;
    static bool isNone(double x) { return (x < 0.999*none); }
  };
  
  enum ShapeErrorType { CellShapeError, 
                        LowGainCellShapeError, MedGainCellShapeError, HighGainCellShapeError, 
                        RingShapeError,
                        LowGainRingShapeError, MedGainRingShapeError, HighGainRingShapeError,
                        NoShapeError, NShapeErrorTypes, BestShapeError };
}

#endif
