/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONITORBASE_IEXTMONITORTOOL_H
#define TRIGMONITORBASE_IEXTMONITORTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "AthenaMonitoring/IMonitorToolBase.h"

using IGetter = IMonitoredAlgo::IGetter;

/**
   An extension of the monitoring tool to allow thread-safe monitoring
 
   Use as follows:
     1) add an array of monitoring tools to the algorithm:
           ToolHandleArray<IExtMonitorTool> m_monTools;
     2) during initialize():
           for (auto m : m_monTools) ATH_CHECK(m->bookHists());
     3) in execute[_R]():
           double phi(10);
           declareMonitoredVariable("phi", phi, m_monTools);
           [...]
           fillMonitored(monTools);
 */

class IExtMonitorTool : virtual public extend_interfaces<IMonitorToolBase> {
public:
  virtual void setProxy(const std::string& name, IGetter* g) = 0;
};

void declareMonitoredCustomVariable(const std::string& name, IGetter* g,
                                    ToolHandleArray<IExtMonitorTool>& monTools);

template <class U>
void declareMonitoredVariable(const std::string& name, U& var, 
                              ToolHandleArray<IExtMonitorTool>& monTools)
{ 
  declareMonitoredCustomVariable(name, new IMonitoredAlgo::SimpleGetterByRef<U,U>(var), monTools);
}

template <class T>
void declareMonitoredStdContainer(const std::string& name, T& container,
                                  ToolHandleArray<IExtMonitorTool>& monTools)
{
  typedef typename contained_type<T>::type CONTAINED;
  declareMonitoredCustomVariable(name, new IMonitoredAlgo::StdContainerSmartGetter<T,CONTAINED>(container, noop<CONTAINED>, IMonitoredAlgo::NoClear), monTools);
}

void fillMonitored(ToolHandleArray<IExtMonitorTool>& monTools);
    
#endif

