/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _InDet_TTrainedNetworkCondAlg_H_
#define _InDet_TTrainedNetworkCondAlg_H_

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "PoolSvc/IPoolSvc.h"

#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"

#include "SiClusterizationTool/TTrainedNetworkCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <string>

#include "TFile.h"

class IPoolSvc;
class TTrainedNetwork;

namespace InDet {

  /**
  */
class TTrainedNetworkCondAlg : public AthAlgorithm {

 public:

  TTrainedNetworkCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~TTrainedNetworkCondAlg() = default;

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  TTrainedNetwork* retrieveNetwork(TFile &input_file, const std::string& folder) const;

  ServiceHandle<ICondSvc> m_condSvc
    {this, "CondSvc", "CondSvc", "The conditions service to register new conditions data."};
  ServiceHandle<IPoolSvc> m_poolsvc
    {this, "PoolSvc", "PoolSvc", "The service to retrieve files by GUID."};
  ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool
    {this,"NetworkToHistoTool", "Trk::NeuralNetworkToHistoTool/NeuralNetworkToHistoTool", "Tool to create a neural network from a set of histograms." };

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/PixelClustering/PixelClusNNCalib", "Cool folder name for the cluster NN input histogram file."};

  SG::WriteCondHandleKey<TTrainedNetworkCollection> m_writeKey
    {this, "WriteKey", "PixelClusterNN", "The conditions statore key for the pixel cluster NNs"};

  Gaudi::Property< std::vector<std::string> > m_nnOrder
    {this, "NetworkNames", {
          "NumberParticles_NoTrack/",
          "ImpactPoints1P_NoTrack/",
          "ImpactPoints2P_NoTrack/",
          "ImpactPoints3P_NoTrack/",
          "ImpactPointErrorsX1_NoTrack/",
          "ImpactPointErrorsX2_NoTrack/",
          "ImpactPointErrorsX3_NoTrack/",
          "ImpactPointErrorsY1_NoTrack/",
          "ImpactPointErrorsY2_NoTrack/",
          "ImpactPointErrorsY3_NoTrack/"},
        "List of network names, which are created in this order"};

  Gaudi::Property<std::string> m_layerInfoHistogram
  {this, "LayerInfoHistogram",      "LayersInfo","Name about the layer info histogram."};

  Gaudi::Property<std::string> m_layerPrefix
  {this, "LayerPrefix",             "Layer",     "Prefix of the pre layer weight and threshold histograms."}; 

  Gaudi::Property<std::string> m_weightIndicator
  {this, "LayerWeightIndicator",    "_weights",  "Suffix of the weight histograms."};

  Gaudi::Property<std::string> m_thresholdIndicator
  {this, "LayerThresholdIndicator", "_thresholds","Suffix of the threshold histograms."};

  Gaudi::Property<bool> m_getInputsInfo
  {this, "GetInputsInfo", false,"Also read a histogram which contains information about the inputs (Run I)."};
};
}
#endif
