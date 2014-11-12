// Implementation of SysStreamTest for the streaming unit-test
// Author: Steve Farrell

#include <iostream>

#include <PATInterfaces/StreamTestWrapper.h>

namespace SysStreamTest
{

  // Constructor
  StreamTestWrapper::StreamTestWrapper(CP::SystematicVariation& sysVar,
                                       CP::SystematicSet& sysSet,
                                       std::vector<CP::SystematicSet>& sysList)
    : m_sysVar(sysVar), m_sysSet(sysSet), m_sysList(sysList)
  {}

  // Dump the systematics
  void StreamTestWrapper::print()
  {
    std::cout << "StreamTestWrapper: printing my systematics" << std::endl;
    std::cout << "  sysVar:  " << m_sysVar.name() << std::endl;
    std::cout << "  sysSet:  " << m_sysSet.name() << std::endl;
    std::cout << "  sysList: " << std::endl;
    for(unsigned int i = 0; i < m_sysList.size(); ++i){
      if(m_sysList[i].size() == 0)
        std::cout << "           nominal" << std::endl;
      else
        std::cout << "           " << m_sysList[i].name() << std::endl;
    }
  }

}
