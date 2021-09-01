/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSNewExtrapolationWeightsTester_h
#define ISF_FASTCALOSIMEVENT_TFCSNewExtrapolationWeightsTester_h

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include <string>
#include "TH2D.h"

// forward declare lwtnn dependencies
namespace lwt
{
  class LightweightNeuralNetwork;
}

class TFCSNewExtrapolationWeightsTester : public TFCSLateralShapeParametrizationHitBase {
public:

  TFCSNewExtrapolationWeightsTester(const char* name=nullptr, const char* title=nullptr);
  virtual ~TFCSNewExtrapolationWeightsTester();

  // Used to decorate Hit with extrap center positions
  virtual FCSReturnCode simulate_hit(Hit& hit, TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) override;

  ///Status bit for chain persistency
  enum FCSfreemem {
     kfreemem = BIT(17) ///< Set this bit in the TObject bit if the memory for m_input should be freed after reading in athena
  };

  bool freemem() const {return TestBit(kfreemem);};
  void set_freemem() {SetBit(kfreemem);};

  bool initializeNetwork(int pid, std::string etaBin, std::string FastCaloNNInputFolderName);
  bool getNormInputs(int pid, std::string etaBin, std::string FastCaloTXTInputFolderName);
  
  static void unit_test(TFCSSimulationState* simulstate=nullptr,const TFCSTruthState* truth=nullptr, const TFCSExtrapolationState* extrapol=nullptr);
  
private:
  
  //Persistify configuration in string m_input. A custom Streamer(...) builds m_graph on the fly when reading from file.
  //Inside Athena, if GANfreemem() is true, the content of m_input is deleted after reading in order to free memory
  std::string*                   m_input       = nullptr;
  lwt::LightweightNeuralNetwork* m_nn          = nullptr; //! Do not persistify
  std::vector<int>*              m_normLayers  = nullptr; // vector of index layers (-1 corresponds to truth energy)
  std::vector<float>*            m_normMeans   = nullptr; // vector of mean values for normalizing energy fraction per layer, last index is for total energy
  std::vector<float>*            m_normStdDevs = nullptr; // vector of std dev values for normalizing energy fraction per layer, last index is for total energy

  ClassDefOverride(TFCSNewExtrapolationWeightsTester,1)  //TFCSNewExtrapolationWeightsTester
};

#endif
