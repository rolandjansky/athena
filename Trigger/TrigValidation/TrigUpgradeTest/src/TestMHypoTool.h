/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTMHYPOTOOL_H
#define TRIGUPGRADETEST_TESTMHYPOTOOL_H 1

// STL includes
#include <string>


#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigCompositeUtils/HLTIdentifier.h"

#include "./ITestHypoTool.h"

namespace HLTTest {

  /**
   * @class TestMHypoTool
   * @brief Tool taking the decision for multiplicity selection (one decision per object)
   * The implementation is limited to same threshold only (it is a test).
   **/

  class TestMHypoTool : virtual public ITestHypoTool, public AthAlgTool
  { 
  public: 
    TestMHypoTool( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

    virtual ~TestMHypoTool(); 

    StatusCode  initialize() override;
    StatusCode  finalize() override;    
    StatusCode decide( DecisionContainer* decisions ) const override;
  
  private: 

    //    TestMHypoTool();
    float m_threshold;
    size_t m_multiplicity;
    std::string m_property;
    StatusCode iterate( DecisionContainer* decisions, std::function<void(Decision*)> f) const;
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTMHYPOTOOL_H
