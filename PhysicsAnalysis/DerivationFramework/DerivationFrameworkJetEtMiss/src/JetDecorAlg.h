/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetDecorAlg.h

#ifndef JetDecorAlg_H
#define JetDecorAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>
#include <utility>

namespace DerivationFramework {
  class JetDecorAlg : public AthAlgorithm { 

  public: 

    /// Constructor with parameters:
    JetDecorAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~JetDecorAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    JetDecorAlg();

  private:

    std::string m_inputColl;
    std::vector<std::string> m_momentsToCopy;

    std::vector<std::pair<std::string,std::string> > m_floatMoments;
    std::vector<std::pair<std::string,std::string> > m_intMoments;
    //
    std::vector<std::pair<std::string,std::string> > m_vfloatMoments;
    std::vector<std::pair<std::string,std::string> > m_vintMoments;
  }; 

}

#endif
