/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_MUCTPI_ATHALG_H
#define TRIGT1MUCTPIPHASE1_MUCTPI_ATHALG_H

/*
class description
*/

// Athena/Gaudi include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

namespace LVL1MUCTPI {
  class IMuctpiSimTool;
}
namespace LVL1MUCTPIPHASE1 {

  class MUCTPI_AthTool;

  class MUCTPI_AthAlg : public AthAlgorithm
  {
    
  public:
    
    MUCTPI_AthAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MUCTPI_AthAlg();
    

    /// Regular Gaudi algorithm initialization function
    virtual StatusCode initialize() override;
    /// Regular Gaudi algorithm finalization function
    virtual StatusCode finalize() override;
    /// Regular Gaudi algorithm execute function
    virtual StatusCode execute() override;
    
  private:
    
    ToolHandle<LVL1MUCTPI::IMuctpiSimTool> m_muctpiTool;
  };
}

#endif // TRIGT1MUCTPIPHASE1_MUCTPI_ATHALG_H
