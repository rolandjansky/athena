/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_ITESTHYPOTOOL_H
#define TRIGUPGRADETEST_ITESTHYPOTOOL_H 1

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "GaudiKernel/IAlgTool.h"

namespace HLTTest {

  using namespace TrigCompositeUtils;
  static const InterfaceID IID_ITestHypoTool("ITestHypoTool", 1, 0);

  /**
   * @class $(klass)s
   * @brief 
   **/

  class ITestHypoTool
    : virtual public ::IAlgTool
  { 

  public: 

    virtual ~ITestHypoTool();
    ITestHypoTool(const std::string& name)
      : m_decisionId(name) {}
    static const InterfaceID& interfaceID();
    
    /** 
     * @brief method invoked to work out the decisions per object
     * @warning it is up to the tool implementation to store correct identifiers
     * The object passed to the tool have to have actual object linked via EL named "feature"
     * This is by no means suggested interface, in fact it involved constructing a link etc.
     **/
    virtual StatusCode decide( DecisionContainer* decisions ) const = 0;

    /**
     * @brief returns decision ID for this hypo tool
     **/
    HLT::Identifier decisionId() const { return m_decisionId; }
  protected:
    HLT::Identifier m_decisionId;
    
  }; 

  inline const InterfaceID& ITestHypoTool::interfaceID() 
  { 
    return IID_ITestHypoTool; 
  }

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_ITESTHYPOTOOL_H
