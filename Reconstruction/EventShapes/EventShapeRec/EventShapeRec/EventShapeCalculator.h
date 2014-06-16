/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEREC_EVENTSHAPECALCULATOR_H
#define EVENTSHAPEREC_EVENTSHAPECALCULATOR_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "EventShapeInterface/IEventShapeToolBase.h"

#include <vector>
#include <string>

class EventShapeCalculator : public AthAlgorithm
{
 public:
  class etaSelector
  {
  public:
    etaSelector(double etaCut) { m_cutCentral = etaCut; };
    bool operator()(const EventShapeTypes::es_fourvector_t* a)
    {  return fabs(a->eta()) > m_cutCentral && a->e() > 0;  }
    
  private:
    double m_cutCentral;
    etaSelector() {};
  };
  
  //! Standard algorithm constructor
  EventShapeCalculator(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Algorithm destructor
  virtual ~EventShapeCalculator() {};
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private:
  
  std::string m_inputCollection;
  std::string m_jetCollection;
  std::string m_OutputCollection;
  
  //  std::vector<std::string> m_eventShapeToolNames;
  //  std::string              m_forwardTermToolName;
  
  typedef ToolHandleArray<IEventShapeToolBase>  tool_store_t;
  typedef ToolHandle<IEventShapeToolBase>       tool_handle_t;
  
  tool_store_t       m_eventShapeTools;
  tool_handle_t      m_forwardTermTool;
  
  double m_cutCentral;
  double m_jetcutCentral;
  double m_jetcutEt;
};

#endif
