/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCSEnergyBinParametrization_h
#define ISF_FASTCALOSIMEVENT_TFCSEnergyBinParametrization_h

#include "ISF_FastCaloSimEvent/TFCSEnergyParametrization.h"
#include<map>
#include<vector>

class TFCSEnergyBinParametrization:public TFCSEnergyParametrization
{
 public:
  TFCSEnergyBinParametrization(const char* name=nullptr, const char* title=nullptr);

  virtual void set_pdgid(int id);
  virtual void set_pdgid(const std::set< int > &ids);
  virtual void add_pdgid(int id);
  virtual void clear_pdgid();
  
  virtual int n_bins() const {return m_number_of_Ekin_bins;};

  /// current convention is to start Ekin_bin counting at 1, to be updated to start counting with 0
  void set_number_of_Ekin_bins(int n_Ekin_bin) {m_number_of_Ekin_bins=n_Ekin_bin;resize();};

  /// set_pdgid_Ekin_bin_probability(): format of probability should be a vector of float with probability values, 
  /// e.g. for equal probability of 5 Ekin bins specify a vector with {0,1,1,1,1,1} or {0,0.2,0.2,0.2,0.2,0.2}
  /// This would give a probability of 0 for Ekin_bin=0 and a probability of 20% for 1<=Ekin_bin<=5
  /// the function will normalize probabilities automatically, if the sum of values is not 1
  /// current convention is to start Ekin_bin counting at 1, to be updated to start counting with 0
  virtual void set_pdgid_Ekin_bin_probability(int id,std::vector< float > prob);
  
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  virtual bool is_match_Ekin_bin(int Ekin_bin) const;
  
  void Print(Option_t *option = "") const;
 private:
  int m_number_of_Ekin_bins;
  std::map< int, std::vector< float > > m_pdgid_Ebin_probability;
  
  void resize();
  
  ClassDef(TFCSEnergyBinParametrization,1)  //TFCSEnergyBinParametrization
 
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCSEnergyBinParametrization+;
#endif

#endif
