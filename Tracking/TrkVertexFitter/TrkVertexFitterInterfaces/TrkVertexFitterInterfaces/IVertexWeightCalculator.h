/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexWeightCalculator.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVERTEXWEIGHTCALCULATOR_H
#define TRK_IVERTEXWEIGHTCALCULATOR_H

#include "GaudiKernel/IAlgTool.h"

// xAOD include
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexFwd.h"

namespace Trk {
static const InterfaceID IID_IVertexWeightCalculator("IVertexWeightCalculator",
                                                     1,
                                                     0);

/**
 @class IVertexWeightCalculaculator
 @brief Interface class for signal vertex selection.

 For more detailed information, take a look at the header file of the actual
 implementation files.

 ---------------------------------------------------
 Changes:

 David Shope <david.richard.shope@cern.ch> (2016-06-01)
   EDM Migration to xAOD - remove method using VxCandidate

*/

class IVertexWeightCalculator : virtual public IAlgTool
{

public:
  /** Virtual destructor */
  virtual ~IVertexWeightCalculator() = default;

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID()
  {
    return IID_IVertexWeightCalculator;
  };

  /** Interface for @c xAOD::Vertex */
  virtual double estimateSignalCompatibility(
    const xAOD::Vertex& vertex) const = 0;
};
}

#endif

