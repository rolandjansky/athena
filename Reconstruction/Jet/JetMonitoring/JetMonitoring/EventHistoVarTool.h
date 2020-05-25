/* this file is -*- C++ -*- 
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EventMONITORING_EventVARTOOL_H
#define EventMONITORING_EventVARTOOL_H

#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/EventVariable.h"


static const InterfaceID IID_IEventHistoVarTool("IEventHistoVarTool", 1 , 0); 

///////////////////////////////////////////////////////////
/// \class EventHistoVarTool
///
/// This class is a wrapper around EventVar::Variable inheriting AthAlgTool.
/// Its purpose is to allow to define a Event variable in the configuration
/// and to use it from other tools.
/// Client tools include EventHistoAttributeFiller and EventHistoSelectSort
///

class IEventHistoVarTool : virtual public IAlgTool                         
{
public:
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IEventHistoVarTool; }

  virtual ~IEventHistoVarTool(){}
  
  /// the value of the variable for a given Event
  virtual float value(const xAOD::EventInfo &) const = 0;
  /// The actual variable represented by this tool
  virtual const EventVar::Variable * variable() const = 0;
  /// a compact description of the variable.
  virtual std::string describe() const =0;
};


class EventHistoVarTool : public AthAlgTool, virtual public IEventHistoVarTool {
public:
  EventHistoVarTool( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~EventHistoVarTool(){}

  virtual StatusCode initialize() ;  

  virtual float value(const xAOD::EventInfo &) const ;  
  virtual const EventVar::Variable * variable() const {return m_v.get();};
  virtual std::string describe() const {
    return m_name+", "+m_type+","+std::to_string(m_index);
  }
  
  
private:

  std::unique_ptr<EventVar::Variable>  m_v;

  Gaudi::Property<std::string> m_name {this,"Name", ""};
  Gaudi::Property<std::string> m_type {this,"Type", "float"};
  Gaudi::Property<int> m_index {this,"Index", -1};
  Gaudi::Property<float> m_scale {this,"Scale", 1.};
  
};


#endif
