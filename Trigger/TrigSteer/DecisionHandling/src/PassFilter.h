/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_PASSFILTER_H
#define DECISIONHANDLING_PASSFILTER_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// STL includes
#include <string>

/**
 * @class PassFilter
 * @brief Filter algorithm that always pass, needed to fill gaps in HLT CF
 **/
class PassFilter : public AthReentrantAlgorithm {
public:
  PassFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PassFilter() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:

};

#endif // DECISIONHANDLING_PASSFILTER_H
