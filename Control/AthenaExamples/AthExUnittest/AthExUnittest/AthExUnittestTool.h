// -*- mode: c++ -*-
//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXUNITTEST_ATHEXUNITTESTTOOL_H
#define ATHEXUNITTEST_ATHEXUNITTESTTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExUnittest/IAthExUnittestTool.h"

class AthExUnittestTool: public AthAlgTool, 
                         public virtual IAthExUnittestTool { 
public: 

  // Constructor
  AthExUnittestTool( const std::string& type,
		     const std::string& name,
		     const IInterface* parent );
  
  // Initialize is required by AsgTool base class
  virtual StatusCode initialize() override;
  virtual double useTheProperty() override;

private: 

  double m_nProperty;
  unsigned int m_enumProperty;

}; 

#endif //> !ATHEXUNITTEST_ATHEXUNITTESTTOOL_H
