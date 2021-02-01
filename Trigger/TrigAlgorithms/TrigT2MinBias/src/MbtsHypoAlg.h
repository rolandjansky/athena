/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_MBTSHYPOALG_H
#define TRIGT2MINBIAS_MBTSHYPOALG_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// STL includes
#include <string>

/**
 * @class MbtsHypoAlg
 * @brief 
 **/
class MbtsHypoAlg : public AthReentrantAlgorithm {
public:
  MbtsHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MbtsHypoAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  //Gaudi::Property<int> m_myInt{this, "MyInt", 0, "An Integer"};
};

#endif // TRIGT2MINBIAS_MBTSHYPOALG_H
