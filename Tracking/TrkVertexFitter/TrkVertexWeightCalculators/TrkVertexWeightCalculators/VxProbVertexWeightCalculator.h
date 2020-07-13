/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexWeightCalculator_VxProbVertexWeightCalculator_H
#define TrkVertexWeightCalculator_VxProbVertexWeightCalculator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"

// xAOD include
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexFwd.h"

/**
 * @class Trk::VxProbVertexWeightCalculator
 *
 * @author Johanna Bronner, April 2009
 *
 * ---------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-06-01)
 *
 *   EDM Migration to xAOD - remove method using VxCandidate
 *
 */

class TH1;
class ITHistSvc;

namespace Trk {

class VxProbVertexWeightCalculator
  : public AthAlgTool
  , virtual public IVertexWeightCalculator
{
public:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  /**
   * constructor
   */
  VxProbVertexWeightCalculator(const std::string& t,
                               const std::string& n,
                               const IInterface* p);

  /**
   * destructor
   */
  virtual ~VxProbVertexWeightCalculator() = default;

  /**
   * WeightCalculator
   */

  virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex) const override final;

private:
  TH1* m_hMinBiasPt;
  std::string m_HistoFileLocation;
  ITHistSvc* m_iTHistSvc;

}; // end of class description
} // end of namespace definition

#endif
