/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexWeightCalculator_NNVertexWeightCalculator_H
#define TrkVertexWeightCalculator_NNVertexWeightCalculator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"
#include <TString.h>
#include <vector>
#include <memory>
// xAOD include
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexFwd.h"

#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
/**
 * @class Trk::NNVertexWeightCalculator
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

class NNVertexWeightCalculator
  : public AthAlgTool
  , virtual public IVertexWeightCalculator
{
public:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /**
   * constructor
   */
  NNVertexWeightCalculator(const std::string& t,
                           const std::string& n,
                           const IInterface* p);

  /**
   * destructor
   */
  virtual ~NNVertexWeightCalculator() = default;

  /**
   * WeightCalculator
   */
  virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex) const override final;

private:
  double mEstimateSignalCompatibility(
    const std::vector<const Trk::TrackParameters*>& tpList) const;

  double sphericityFunc(std::vector<std::vector<double>> v_P) const;
  double LogFunc(double value) const;

  ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool;
  std::unique_ptr<TTrainedNetwork> m_NN;
  int m_NLayers;
  ITHistSvc* m_iTHistSvc;
  std::string m_directoryName;
  std::vector<TH1*> m_NNhistos;
}; // end of class description
} // end of namespace definition

#endif
