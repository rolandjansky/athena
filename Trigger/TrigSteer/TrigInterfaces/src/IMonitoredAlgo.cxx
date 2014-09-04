/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <iostream>
#include "TrigInterfaces/IMonitoredAlgo.h"
using namespace std;


IMonitoredAlgo::IGetter* IMonitoredAlgo::declareMonitoredCustomVariable(const std::string& name, IGetter* g ) {
  assesName(name);
  m_proxies[name] = g;
  g->name(name);
  return g;
}


IMonitoredAlgo::~IMonitoredAlgo() {
  for (std::map<std::string, IMonitoredAlgo::IGetter*>::iterator i = m_proxies.begin();
       i != m_proxies.end(); ++i)
    delete i->second;
}


bool IMonitoredAlgo::assesName(const std::string& name) {
  if (m_proxies.find(name) != m_proxies.end()) {
    
    cout << "ERROR " << "Attempt to register the monitored varaible: " << name << " many times" << endl;
    return false;
  }
  return true;
}
