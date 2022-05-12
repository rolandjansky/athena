/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _InDet_LWTNNCondAlg_H_
#define _InDet_LWTNNCondAlg_H_

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "PoolSvc/IPoolSvc.h"

//#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"
#include "SiClusterizationTool/LWTNNCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <string>

class IPoolSvc;

namespace lwt {
  class NanReplacer;    
  //class LightweightGraph;
  namespace atlas { class FastGraph; }
}

namespace InDet {

  /**
  */
class LWTNNCondAlg : public AthReentrantAlgorithm {

 public:

  LWTNNCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~LWTNNCondAlg() = default;

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
//  TTrainedNetwork* retrieveNetwork(TFile &input_file, const std::string& folder) const;

  //StatusCode configureLwtnn(std::unique_ptr<lwt::LightweightGraph> & thisNN, const std::string& thisJson);
  StatusCode configureLwtnn(std::unique_ptr<lwt::atlas::FastGraph> & thisNN, const std::string& thisJson) const;

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/PixelClustering/PixelNNCalibJSON", "Cool folder name for the cluster NN input histogram file."};

  SG::WriteCondHandleKey<LWTNNCollection> m_writeKey
    {this, "WriteKey", "PixelClusterNNJSON", "The conditions statore key for the pixel cluster NNs"};

  // as of now, the number and position networks all use the same variables
  // only need one of these
  Gaudi::Property< std::vector<std::string> > m_variableOrder
    {this, "VariableOrder", {
          "NN_matrix0",
          "NN_matrix1",
          "NN_matrix2",
          "NN_matrix3",
          "NN_matrix4",
          "NN_matrix5",
          "NN_matrix6",
          "NN_matrix7",
          "NN_matrix8",
          "NN_matrix9",
          "NN_matrix10",
          "NN_matrix11",
          "NN_matrix12",
          "NN_matrix13",
          "NN_matrix14",
          "NN_matrix15",
          "NN_matrix16",
          "NN_matrix17",
          "NN_matrix18",
          "NN_matrix19",
          "NN_matrix20",
          "NN_matrix21",
          "NN_matrix22",
          "NN_matrix23",
          "NN_matrix24",
          "NN_matrix25",
          "NN_matrix26",
          "NN_matrix27",
          "NN_matrix28",
          "NN_matrix29",
          "NN_matrix30",
          "NN_matrix31",
          "NN_matrix32",
          "NN_matrix33",
          "NN_matrix34",
          "NN_matrix35",
          "NN_matrix36",
          "NN_matrix37",
          "NN_matrix38",
          "NN_matrix39",
          "NN_matrix40",
          "NN_matrix41",
          "NN_matrix42",
          "NN_matrix43",
          "NN_matrix44",
          "NN_matrix45",
          "NN_matrix46",
          "NN_matrix47",
          "NN_matrix48",
          "NN_pitches0",
          "NN_pitches1",
          "NN_pitches2",
          "NN_pitches3",
          "NN_pitches4",
          "NN_pitches5",
          "NN_pitches6",
          "NN_layer",
          "NN_barrelEC",
          "NN_phi",
          "NN_theta"},
        "List of training variables for the LWTNN networks in the order they are fed to evaluate the networks"};

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
