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


    Gaudi::Property<float>  m_threshold{ this, "Threshold", 0, "Threshold to apply" };
    StringProperty m_property{ this, "Property", "Et", "Propery to use to test hypothesis (Et, pt)" };

   
    // float m_threshold;
    //std::string m_property;
    StringProperty m_linkName {this, "LinkName", "initialRoI",  "name of the link to the features in the decision, e.g. 'feature', 'initialRoI'"};
    

    
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTHYPOTOOL_H
