/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _InDet_LWTNNCondAlg_H_
#define _InDet_LWTNNCondAlg_H_

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "PoolSvc/IPoolSvc.h"

//#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"
#include "SiClusterizationTool/LWTNNCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <string>

class IPoolSvc;

namespace lwt {
  class NanReplacer;    
  class LightweightGraph;
}

namespace InDet {

  /**
  */
class LWTNNCondAlg : public AthAlgorithm {

 public:

  LWTNNCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~LWTNNCondAlg() = default;

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
//  TTrainedNetwork* retrieveNetwork(TFile &input_file, const std::string& folder) const;

  ServiceHandle<ICondSvc> m_condSvc
    {this, "CondSvc", "CondSvc", "The conditions service to register new conditions data."};
/*  ServiceHandle<IPoolSvc> m_poolsvc
    {this, "PoolSvc", "PoolSvc", "The service to retrieve files by GUID."};
  ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool
    {this,"NetworkToHistoTool", "Trk::NeuralNetworkToHistoTool/NeuralNetworkToHistoTool", "Tool to create a neural network from a set of histograms." };
*/
  StatusCode configureLwtnn(std::unique_ptr<lwt::LightweightGraph> & thisNN, std::string thisJson);

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/PixelClustering/PixelNNCalibJSON", "Cool folder name for the cluster NN input histogram file."};

  SG::WriteCondHandleKey<LWTNNCollection> m_writeKey
    {this, "WriteKey", "PixelClusterNNJSON", "The conditions statore key for the pixel cluster NNs"};

  Gaudi::Property< std::vector<std::string> > m_nnOrder
    {this, "NetworkNames", {
          "NumberNetwork",
          "PositionNetwork_N1",
          "PositionNetwork_N2",
          "PositionNetwork_N3"},
        "List of network names, which are indexe in map in this order"};
/*
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
  */
};
}
#endif
