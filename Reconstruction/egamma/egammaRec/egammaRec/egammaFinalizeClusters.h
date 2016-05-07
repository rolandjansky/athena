/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_FINALIZECLUSTERS_H
#define EGAMMAREC_FINALIZECLUSTERS_H
/**
  @class egammaFinalizeClusters 
*/
// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>

class egammaFinalizeClusters : public AthAlgorithm{
 public:
  /** @brief Default constructor*/
  egammaFinalizeClusters(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Destructor*/
  ~egammaFinalizeClusters();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();

 private:
  StatusCode impl(const std::string& name) const ;

  std::string m_outputClusterKey;
  std::string m_outputForwardClusterKey;
  std::string m_outputTopoSeededClusterKey;
};

#endif









