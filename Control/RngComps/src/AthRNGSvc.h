// -*- c++ -*-
#ifndef _ATHENA_ATHRNGSVC_H
#define _ATHENA_ATHRNGSVC_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <functional>

#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaBaseComps/AthService.h"

namespace ATHRNG {
  class RNGWrapper;
}
namespace CLHEP{
  class HepRandomEngine;
}

class AthRNGSvc : virtual public IAthRNGSvc,
		  public AthService
{

public:
  virtual ATHRNG::RNGWrapper* GetEngine(const std::string& streamName) override final;
  virtual ATHRNG::RNGWrapper* setOnDefinedSeeds(uint64_t theSeed,
						const std::string& streamName) override final;
  virtual ATHRNG::RNGWrapper* setOnDefinedSeeds(uint64_t eventNumber,
						uint64_t runNumber,
						const std::string& streamName) override final;
  ///seed all streams we manage, combining theSeed and the stream names
  virtual bool setAllOnDefinedSeeds (uint64_t theSeed) override final; 
  ///seed all streams, combining eventNumber, runNumber and the stream names
  virtual bool setAllOnDefinedSeeds (uint64_t eventNumber, uint64_t runNumber)override final;
  AthRNGSvc(const std::string& name, ISvcLocator* svc);
  virtual ~AthRNGSvc();
  
  virtual void print(const std::string& streamName) override final;
  virtual void print() override final;
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  StatusCode initialize() override final;
  StatusCode finalize() override final;

private:
  void CreateStream(uint64_t seed1, uint64_t seed2, 
			    const std::string& streamName);
  size_t hashCombine(size_t h1,size_t h2){ return (h1^(h2+(h1<<6)+(h1>>2)));};
  std::unordered_map<std::string,std::pair<ATHRNG::RNGWrapper*,CLHEP::HepRandomEngine*>> m_wrappers;
  std::mutex m_mutex;
  std::string m_RNGType;
  typedef std::function<CLHEP::HepRandomEngine*(void)> factoryFunc;
  factoryFunc m_fact;

};
#endif

