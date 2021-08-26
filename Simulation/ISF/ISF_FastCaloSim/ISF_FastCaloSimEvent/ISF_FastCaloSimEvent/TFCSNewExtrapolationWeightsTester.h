/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSNewExtrapolationWeightsTester_h
#define ISF_FASTCALOSIMEVENT_TFCSNewExtrapolationWeightsTester_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationBinnedChain.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include <string>
#include "TH2D.h"

// forward declare lwtnn dependencies
namespace lwt
{
  class LightweightGraph;
}

class TFCSNewExtrapolationWeightsTester:public TFCSParametrizationBinnedChain {
public:
  // type of input requested by lwtnn
  typedef std::map<std::string, std::map<std::string, double> >  NetworkInputs ;
  typedef std::map<std::string, double> NetworkOutputs;
  typedef std::map<int, TH2D> Binning;

  TFCSNewExtrapolationWeightsTester(const char* name=nullptr, const char* title=nullptr);
  virtual ~TFCSNewExtrapolationWeightsTester();

  ///Status bit for chain persistency
  enum FCSGANfreemem {
     kGANfreemem = BIT(17) ///< Set this bit in the TObject bit if the memory for m_input should be freed after reading in athena
  };

  bool GANfreemem() const {return TestBit(kGANfreemem);};
  void set_GANfreemem() {SetBit(kGANfreemem);};
  void reset_GANfreemem() {ResetBit(kGANfreemem);};

  ///Status bit for energy initialization
  enum FCSEnergyInitializationStatusBits {
     kOnlyScaleEnergy = BIT(18) ///< Set this bit in the TObject bit field the simulated energy should only be scaled by the GAN
  };

  const lwt::LightweightGraph* get_graph() const {return m_graph;};
  
  bool initializeNetwork(int pid,int etaMin,std::string FastCaloGANInputFolderName);
  
  static void unit_test(TFCSSimulationState* simulstate=nullptr,const TFCSTruthState* truth=nullptr, const TFCSExtrapolationState* extrapol=nullptr);

protected:  
  void GetBinning(int pid,int etaMax,std::string FastCaloGANInputFolderName);
  
private:
  std::vector< int > m_bin_ninit;
  
  //Persistify configuration in string m_input. A custom Streamer(...) builds m_graph on the fly when reading from file.
  //Inside Athena, if GANfreemem() is true, the content of m_input is deleted after reading in order to free memory
  std::string*     m_input=nullptr;
  lwt::LightweightGraph* m_graph=nullptr;//!Do not persistify
  
  Binning m_Binning;
  
  // specific to architecture
  // preprocessing of input
  int m_GANLatentSize = 0;

  ClassDefOverride(TFCSNewExtrapolationWeightsTester,2)  //TFCSNewExtrapolationWeightsTester
};

#endif
