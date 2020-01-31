/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_IMonitoredVariable_h
#define AthenaMonitoringKernel_IMonitoredVariable_h

#include <string>
#include <vector>

namespace Monitored {
  class MonitoredGroup;

  class IMonitoredVariable {
  public:
    friend MonitoredGroup;
    virtual ~IMonitoredVariable() {}

    const std::string& name() const { return m_name; }
    virtual std::vector<double> getVectorRepresentation() const = 0;
    virtual std::vector<std::string> getStringVectorRepresentation() const = 0;
    virtual bool hasStringRepresentation() const = 0; //!< indcates that the stored content can be converted to strings
    virtual size_t size() const = 0; //!< gives size of vector representation
    
  protected:

    IMonitoredVariable(std::string name)
      : m_name(std::move(name)) {}

  private:
    const std::string m_name;
  };
}

#endif /* AthenaMonitoringKernel_IMonitoredVariable_h */
