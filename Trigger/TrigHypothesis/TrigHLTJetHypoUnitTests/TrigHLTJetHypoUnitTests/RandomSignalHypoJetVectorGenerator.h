/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_RandomSignalHypoJetVectorGenerator_H
#define TRIGHLTJETHYPOUNITTESTS_RandomSignalHypoJetVectorGenerator_H

#include "TrigHLTJetHypoUnitTests/IHypoJetVectorGenerator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "./RandUint.h"
#include "./RandDouble.h"
#include <random>


class RandomSignalHypoJetVectorGenerator: public extends<AthAlgTool,
  IHypoJetVectorGenerator> {

 public:
  
  RandomSignalHypoJetVectorGenerator (const std::string& type,
				const std::string& name,
				const IInterface* parent);

  ~RandomSignalHypoJetVectorGenerator() {}

  virtual StatusCode initialize() override;
  virtual HypoJetVector get() override;
  virtual std::string toString() const override;
  
 private:

  Gaudi::Property<int>
    m_n_sig_lo{this, "n_sig_lo", 2, "Lower bound nsig jets to generate "};

  
  Gaudi::Property<int>
    m_n_sig_hi{this, "n_sig_hi", 6, "Upper bound nsig jets to generate "};

  
  Gaudi::Property<double>
    m_e_sig_lo{this, "e_sig_lo", 50., "Lower bound e sig to generate "};

  
  Gaudi::Property<double>
    m_e_sig_hi{this, "e_sig_hi", 100., "Upper bound e sig to generate "};

  
  Gaudi::Property<double>
    m_eta_sig_hi{this, "eta_sig_hi", 3.2, "Upper bound eta sig to generate "};

  
  Gaudi::Property<int>
    m_nbkgd{this, "n_bkgd", 0, "No of background jets to generate "};
  
  Gaudi::Property<double>
    m_bkgd_etmax{this, "bkgd_etmax", 0., "Max Et  background jets"};
  
  Gaudi::Property<double>
    m_bkgd_etamax{this, "bkgd_etamax", 5., "Lower limit Eta range background "};

  Gaudi::Property<bool>
    m_useEtaEtNotEtaE{this, "useEtaEtNotEtaE", true, "e values or et values "};


  std::default_random_engine m_shuffle_re;
  RandUint m_randNSig;
  RandDouble m_randEtaSig;
  RandDouble m_randEnSig;
  RandDouble m_randEtaBkd;
  RandDouble m_randEnBkd;

  RandUint m_flip;
};

#endif
