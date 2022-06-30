/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCIDFilterTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PRIMARYDPDMAKER_BCIDFILTERTOOL_H
#define PRIMARYDPDMAKER_BCIDFILTERTOOL_H 

#include<string>
#include <vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

/** @class BCIDFilterTool
  
    @author Christian Ohm -at- cern.ch
   */
class BCIDFilterTool : public AthAlgTool, public DerivationFramework::ISkimmingTool {
  
 public: 
  /** Constructor with parameters */
  BCIDFilterTool( const std::string& t, const std::string& n, const IInterface* p );
  
  /** Destructor */
  ~BCIDFilterTool();
  
  // Athena algtool's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;
  
  /** Check that the current event passes this filter */
  virtual bool eventPassesFilter() const override;
  
 private:
  mutable std::atomic<unsigned int> m_ntot{0};
  mutable std::atomic<unsigned int> m_npass{0};
  
  std::vector<int> m_acceptBCIDs;
  std::vector<int> m_rejectBCIDs;    
  
};

#endif
