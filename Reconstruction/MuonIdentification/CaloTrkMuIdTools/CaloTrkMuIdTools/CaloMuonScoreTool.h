/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_CALOMUONSCORETOOL_H
#define CALOTRKMUIDTOOLS_CALOMUONSCORETOOL_H

#include "ICaloTrkMuIdTools/ICaloMuonScoreTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonScoreONNXRuntimeSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"

#include <vector>
#include <memory>

/** @class CaloMuonScoreTool

    Fetch the calorimeter cells around a track particle and compute the muon 
    score.
 
    The muon score is computed by doing inference on a 7-colour-channel 
    convolutional neural network. The inputs to the convolutional 
    neural network are the energy deposits in 30 eta 
    and 30 phi bins around the track particle. Seven colour channels are 
    considered, corresponding to the seven calorimeter layers 
    (CaloSamplingIDs) in the low-eta region (eta < 0.1).

    The convolutional neural network was trained using tensorflow. 
    Inference on this model is done using ONNX (the tensorflow model 
    having been converted to ONNX format).

    @author Ricardo Woelker <ricardo.woelker@cern.ch>
*/

class CaloMuonScoreTool : public AthAlgTool, virtual public ICaloMuonScoreTool {
public:
  CaloMuonScoreTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~CaloMuonScoreTool()=default;

  virtual StatusCode initialize();
  
  // Compute the muon score given a track particle
  float getMuonScore(const xAOD::TrackParticle* trk) const;

  // run the ONNX inference on the input tensor
  float runOnnxInference(std::vector<float> &tensor) const;

  // fill vectors from the particle cell association
  void fillInputVectors(std::unique_ptr<const Rec::ParticleCellAssociation>& association, std::vector<float> &eta, std::vector<float> &phi, std::vector<float> &energy, std::vector<int> &samplingId) const;

  // Compute the median of a vector of floats (can be even or odd in length)
  float getMedian(std::vector<float> v) const;

  // Get a linearly spaced vector of size `nBins`, ranging from `min` to `max` (both values included)
  std::vector<float> getLinearlySpacedBins(float min, float max, int nBins) const;

  // Given a vector of bins, return the index of the matching bin
  int getBin(std::vector<float> &bins, float &val) const;

  // Given a calo sampling ID (as integer), return the corresponding "RGB"-like channel ID (0,1,2,3,4,5,6)
  int channelForSamplingId(int &samplingId) const;

  // for a given particle, consume vectors for eta, phi, energy, sampling ID, and return the input tensor to be used in ONNX
  std::vector<float> getInputTensor(std::vector<float> &eta, std::vector<float> &phi, std::vector<float> &energy, std::vector<int> &sampling) const;

private:
  // Number of bins in eta
  int m_etaBins = 30;

  // Number of bins in phi
  int m_phiBins = 30;

  // window in terms of abs(eta) to consider around the median eta value
  float m_etaCut = 0.25;

  // window in terms of abs(phi) to consider around the median phi value
  float m_phiCut = 0.25;

  // Number of colour channels to consider in the convolutional neural network
  int m_nChannels = 7;

  ToolHandle <Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool{this, "ParticleCaloCellAssociationTool", ""}; 

  /// Handle to @c IONNXRuntimeSvc
  ServiceHandle< ICaloMuonScoreONNXRuntimeSvc > m_svc{ this, "CaloMuonScoreONNXRuntimeSvc",
      "CaloMuonScoreONNXRuntimeSvc",
      "Name of the service to use" };

  std::unique_ptr< Ort::Session > m_session;

  std::vector<const char*> m_input_node_names;

  std::vector<const char*> m_output_node_names;

  std::vector<int64_t> m_input_node_dims;

  Gaudi::Property<std::string> m_modelFileName{this,"ModelFileName","CaloMuonCNN_0.onnx"};

  Gaudi::Property<double> m_CaloMuonEtaCut {this, "CaloMuonEtaCut", 1.0, "Eta cut (absolute value) up to which a track particle's muon score will be calculated"};
};

#endif
