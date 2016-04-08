/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexWeightCalculator.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef TRK_IVERTEXWEIGHTCALCULATOR_H
#define TRK_IVERTEXWEIGHTCALCULATOR_H

#include "GaudiKernel/IAlgTool.h"

//xAOD include
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleFwd.h"

namespace Trk
{
  class VxCandidate;

  static const InterfaceID IID_IVertexWeightCalculator("IVertexWeightCalculator", 1, 0);

  /**
   @class IVertexWeightCalculaculator
   @brief Interface class for signal vertex selection.

   For more detailed information, take a look at the header file of the actual
   implementation files.

  */

  class IVertexWeightCalculator : virtual public IAlgTool {

     public:
       /** Virtual destructor */
       virtual ~IVertexWeightCalculator(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IVertexWeightCalculator; };

       /** Interface for Track with starting point */
       virtual double estimateSignalCompatibility(const VxCandidate& vertex) = 0;

       /** Interface for @c xAOD::Vertex */
       virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex) = 0;

  };
}

#endif

