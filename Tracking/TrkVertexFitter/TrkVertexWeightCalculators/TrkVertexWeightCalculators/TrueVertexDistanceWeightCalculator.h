/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexWeightCalculator_TrueVertexDistanceWeightCalculator_H
#define TrkVertexWeightCalculator_TrueVertexDistanceWeightCalculator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"

// xAOD include
#include "GeneratorObjects/McEventCollection.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexFwd.h"

/**
 * @class Trk::TrueVertexDistanceWeightCalculator
 *
 * @author Giacinto Piacquadio, August 2010
 *
 * ---------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-06-01)
 *
 *   EDM Migration to xAOD - remove method using VxCandidate
 *
 */

class McEventCollection;

namespace Trk {

class TrueVertexDistanceWeightCalculator
  : public AthAlgTool
  , virtual public IVertexWeightCalculator
{
public:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /**
   * constructor
   */

  TrueVertexDistanceWeightCalculator(const std::string& t,
                                     const std::string& n,
                                     const IInterface* p);

  /**
   * destructor
   */

  virtual ~TrueVertexDistanceWeightCalculator() = default;

  /**
   * WeightCalculator
   */

  virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex) const override final;

private:
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey{
    this,
    "McTruthCollection",
    "TruthEvent",
    "key to retrieve MC truth"
  };

  double mEstimateSignalCompatibility(const Amg::Vector3D& vtxPosition) const;

}; // end of class description
} // end of namespace definition

#endif
