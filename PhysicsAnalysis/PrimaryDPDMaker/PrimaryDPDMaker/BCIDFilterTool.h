/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  StatusCode  initialize();
  StatusCode  finalize();
  
  /** Check that the current event passes this filter */
  virtual bool eventPassesFilter() const;
  
 private:
  mutable unsigned int m_ntot;
  mutable unsigned int m_npass;
  
  std::vector<int> m_acceptBCIDs;
  std::vector<int> m_rejectBCIDs;    
  
};

#endif
