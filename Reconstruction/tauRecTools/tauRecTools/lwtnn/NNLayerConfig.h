/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NN_LAYER_CONFIG_HH_TAURECTOOLS
#define NN_LAYER_CONFIG_HH_TAURECTOOLS

// Layer Configiruation for Lightweight Tagger
//
// The structures below are used to initalize
// `LightweightNeuralNetwork` and the simpler `Stack`.
//
// Author: Dan Guest <dguest@cern.ch>

#include <vector>
#include <map>
#include <string>

namespace lwtDev {
  enum class Activation {NONE, LINEAR, SIGMOID, RECTIFIED, SOFTMAX, TANH,
      HARD_SIGMOID, ELU, LEAKY_RELU, SWISH, ABS};
  enum class Architecture {NONE, DENSE, NORMALIZATION, MAXOUT, HIGHWAY,
      LSTM, GRU, BIDIRECTIONAL, EMBEDDING};
  // components (for LSTM, etc)
  enum class Component {
    I, O, C, F,                 // LSTM
      Z, R, H,                  // GRU
      T, CARRY};                // Highway

  // structure for embedding layers
  struct EmbeddingConfig
  {
    std::vector<double> weights;
    int index;
    int n_out;
  };

  struct ActivationConfig
  {
    Activation function = Activation::NONE;
    double alpha = 0;
  };

  // main layer configuration
  struct LayerConfig
  {
    // dense layer info
    std::vector<double> weights;
    std::vector<double> bias;
    std::vector<double> U;      // TODO: what is this thing called in LSTMs?
    ActivationConfig activation;
    ActivationConfig inner_activation; // for LSTMs and GRUs
    bool go_backwards = false; // for LSTMs and GRUs
    bool return_sequence = false; // for LSTMs and GRUs
    std::string merge_mode; // for Bidirectional


    // additional info for sublayers
    std::vector<LayerConfig> sublayers;
    std::map<Component, LayerConfig> components;
    std::vector<EmbeddingConfig> embedding;

    // arch flag
    Architecture architecture = Architecture::NONE;
  };

  // graph node configuration
  struct NodeConfig
  {
    enum class Type {
      INPUT, INPUT_SEQUENCE, FEED_FORWARD, CONCATENATE, SEQUENCE,
      TIME_DISTRIBUTED, SUM };
    Type type;
    std::vector<size_t> sources;
    int index;                  // input node size, or layer number
  };
}

#endif
