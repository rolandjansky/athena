/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTHYPOTOOL_H
#define TRIGUPGRADETEST_TESTHYPOTOOL_H 1

// STL includes
#include <string>


#include "AthenaBaseComps/AthAlgTool.h"



#include "./ITestHypoTool.h"

namespace HLTTest {

  /**
   * @class TestHypoTool
   * @brief Tool taking the decision for inclusive selection (one decision per object)
   **/

  class TestHypoTool : virtual public ITestHypoTool, public AthAlgTool
  { 

  public: 

    TestHypoTool( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

    virtual ~TestHypoTool(); 

    StatusCode  initialize() override;
    StatusCode  finalize() override;

    StatusCode decide( DecisionContainer* decisions ) const override;
  
  private: 

    //    TestHypoTool();
    float m_threshold;
    std::string m_property;

  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTHYPOTOOL_H
