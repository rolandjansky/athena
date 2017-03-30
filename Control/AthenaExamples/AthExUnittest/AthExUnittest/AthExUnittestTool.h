// -*- mode: c++ -*-
#ifndef ATHEXUNITTEST_ATHEXUNITTESTTOOL_H
#define ATHEXUNITTEST_ATHEXUNITTESTTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExUnittest/IAthExUnittestTool.h"

class AthExUnittestTool: public AthAlgTool, 
                         public virtual IAthExUnittestTool { 
public: 

  // Add another constructor for non-athena use cases
  AthExUnittestTool( const std::string& type,
		     const std::string& name,
		     const IInterface* parent,
		     bool unittest=false );
  
  // Initialize is required by AsgTool base class
  virtual StatusCode initialize() override;
  virtual double useTheProperty() override;

private: 

  double m_nProperty;
  unsigned int m_enumProperty; 
  bool m_unittest;

}; 

#endif //> !ATHEXUNITTEST_ATHEXUNITTESTTOOL_H
