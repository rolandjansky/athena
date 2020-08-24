/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexWeightCalculator_SumPtVertexWeightCalculator_H
#define TrkVertexWeightCalculator_SumPtVertexWeightCalculator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"

// xAOD include
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexFwd.h"

/**
 * @class Trk::SumPtVertexWeightCalculator
 *
 * @author Johanna Bronner, November 2009
 *
 * ---------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-06-01)
 *
 *   EDM Migration to xAOD - remove method using VxCandidate
 *
 */

namespace Trk {

class SumPtVertexWeightCalculator
  : public AthAlgTool
  , virtual public IVertexWeightCalculator
{
public:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /**
   * constructor
   */

  SumPtVertexWeightCalculator(const std::string& t,
                              const std::string& n,
                              const IInterface* p);

  /**
   * destructor
   */

  virtual ~SumPtVertexWeightCalculator() = default;

  /**
   * WeightCalculator
   */

  virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex) const override final;

private:
  /**
   * Flag to Set SumPt^2 not SumPt as selection criteria
   */
  bool m_doSumPt2Selection;

}; // end of class description
} // end of namespace definition

#endif
