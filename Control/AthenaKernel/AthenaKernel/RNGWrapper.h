// -*- c++ -*-
#ifndef _ATHENA_RNGWRAPPER_H
#define  _ATHENA_RNGWRAPPER_H
#include <string>
#include <cstdint>
#include <vector>
#include <functional>
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
namespace CLHEP{
  class HepRandomEngine;
}

namespace ATHRNG{
  class RNGWrapper{
    typedef std::function<CLHEP::HepRandomEngine*(void)> factoryFunc;
  public:
    RNGWrapper(factoryFunc& genFact,size_t nSlots);
    ~RNGWrapper();
    bool setSeed(const std::string& algName,const EventContext& ctx);
    // these methods should only be called with valid context!
    bool setSeed(const std::string& algName,size_t slot, uint64_t ev,uint64_t run);
    operator CLHEP::HepRandomEngine*(){
      return m_engines[Gaudi::Hive::currentContext().slot()];
    }
    CLHEP::HepRandomEngine* getEngine(const EventContext &ctx){
      return m_engines[ctx.slot()];
    }
  private:
    std::vector<CLHEP::HepRandomEngine*> m_engines;
    uint64_t m_evNum;
    uint64_t m_runNum;
  };
}

#endif
