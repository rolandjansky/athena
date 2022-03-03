/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_SimpleHypoJetVectorGenerator_H
#define TRIGHLTJETHYPOUNITTESTS_SimpleHypoJetVectorGenerator_H

#include "TrigHLTJetHypoUnitTests/IHypoJetVectorGenerator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <random>


class SimpleHypoJetVectorGenerator: public extends<AthAlgTool,
  IHypoJetVectorGenerator> {

 public:
  
  SimpleHypoJetVectorGenerator (const std::string& type,
				const std::string& name,
				const IInterface* parent);

  ~SimpleHypoJetVectorGenerator() {}

  virtual StatusCode initialize() override;
  virtual HypoJetVector get()  override;
  virtual std::string toString() const override;
  
 private:

  Gaudi::Property<std::vector<double>>
    m_es{this, "ets", {}, "jet E values"};

  Gaudi::Property<std::vector<double>>
    m_etas{this, "etas", {}, "jet eta  values"};

 

  Gaudi::Property<int>
    m_nbkgd{this, "n_bkgd", 0, "No of background jets to generate "};
  
  Gaudi::Property<double>
    m_bkgd_etmax{this, "bkgd_etmax", 0., "Max Et  background jets"};
  
  Gaudi::Property<double>
    m_bkgd_etamax{this, "bkgd_etamax", 5., "Lower limit Eta range background "};

  Gaudi::Property<bool>
    m_useEtaEtNotEtaE{this, "useEtaEtNotEtaE", true, "e values or et values "};


  std::default_random_engine m_shuffle_re;
};

#endif
