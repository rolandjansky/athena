/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSEnergyAndHitGAN_h
#define ISF_FASTCALOSIMEVENT_TFCSEnergyAndHitGAN_h

#include "ISF_FastCaloSimEvent/TFCSParametrizationBinnedChain.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "lwtnn/LightweightGraph.hh"
#include <string>
#include "TH2D.h"

class TFCSEnergyAndHitGAN:public TFCSParametrizationBinnedChain {
public:
  // type of input requested by lwtnn
  typedef std::map<std::string, std::map<std::string, double> >  NetworkInputs ;
  typedef std::map<std::string, double> NetworkOutputs;
  typedef std::map<int, TH2D> Binning;

  TFCSEnergyAndHitGAN(const char* name=nullptr, const char* title=nullptr);
  virtual ~TFCSEnergyAndHitGAN();

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

  bool OnlyScaleEnergy() const {return TestBit(kOnlyScaleEnergy);};
  void set_OnlyScaleEnergy() {SetBit(kOnlyScaleEnergy);};
  void reset_OnlyScaleEnergy() {ResetBit(kOnlyScaleEnergy);};

  /// use the layer to be done as binning of the GAN chain
  virtual int get_bin(TFCSSimulationState& simulstate,const TFCSTruthState*, const TFCSExtrapolationState*) const override {return simulstate.getAuxInfo<int>("GANlayer"_FCShash);};
  virtual const std::string get_variable_text(TFCSSimulationState& simulstate,const TFCSTruthState*, const TFCSExtrapolationState*) const override;
  
  unsigned int get_nr_of_init(unsigned int bin) const;
  void set_nr_of_init(unsigned int bin,unsigned int ninit);
  
  const Binning& get_Binning() const {return m_Binning;};
  const lwt::LightweightGraph* get_graph() const {return m_graph;};
  const std::string* get_input() const {return m_input;};
  
  bool initializeNetwork(int pid,int etaMin,std::string FastCaloGANInputFolderName);
  
  bool fillFastCaloGanNetworkInputs(TFCSSimulationState& simulstate,const TFCSTruthState* truth, NetworkInputs & inputs,double & trueEnergy) const;
  bool fillEnergy(TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol, NetworkInputs inputs) const;
  virtual FCSReturnCode simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const override;
  
  virtual void Print(Option_t *option="") const override;

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

  ClassDefOverride(TFCSEnergyAndHitGAN,2)  //TFCSEnergyAndHitGAN
};

#endif
