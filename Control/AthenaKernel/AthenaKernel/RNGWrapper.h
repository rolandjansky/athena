// -*- c++ -*-
#ifndef _ATHENA_RNGWRAPPER_H
#define  _ATHENA_RNGWRAPPER_H
#include <string>
#include <cstdint>

class EventContext;
namespace CLHEP{
  class HepRandomEngine;
}
namespace ATHRNG{
  class RNGWrapper{
  public:
    RNGWrapper(CLHEP::HepRandomEngine* engine);
    ~RNGWrapper();
    bool setSeed(const std::string& algName,const EventContext& ctx);
    operator CLHEP::HepRandomEngine*(){
      return m_engine;
    }
  private:
    CLHEP::HepRandomEngine* m_engine;
    uint64_t m_evNum;
    uint64_t m_runNum;
  };
}

#endif
