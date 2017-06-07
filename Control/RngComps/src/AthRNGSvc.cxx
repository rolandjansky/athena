#include "AthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include <functional>
#include "AtlasCLHEP_RandomGenerators/dSFMTEngine.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanecuEngine.h"


AthRNGSvc::AthRNGSvc(const std::string& name,ISvcLocator* svc):AthService(name,svc),m_RNGType("dSFMT"){
  declareProperty("EngineType", m_RNGType,
		  "CLHEP RandomEngine type");
}

StatusCode AthRNGSvc::initialize(){
  ATH_CHECK( AthService::initialize() );
  if(m_RNGType=="dSFMT"){
    m_fact=[](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::dSFMTEngine();
    };
  }else if(m_RNGType=="Ranlux64"){
    m_fact=[](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::Ranlux64Engine();
    };

  }else if(m_RNGType=="Ranecu"){
    m_fact=[](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::RanecuEngine();
    };
  }else{
    ATH_MSG_WARNING("Supported Generator types are 'dSFMT' , 'Ranlux64', 'Ranecu'");
    ATH_MSG_FATAL("Generator type \""<<m_RNGType<<"\" is not known. Check Joboptions");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode AthRNGSvc::finalize(){
  return StatusCode::SUCCESS;
}

ATHRNG::RNGWrapper* AthRNGSvc::GetEngine(const std::string& streamName){
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it=m_wrappers.find(streamName);
  if(it==m_wrappers.end()){
    ATH_MSG_INFO("Creating engine for "<<streamName);
    auto eng=m_fact();
    auto wrp=new ATHRNG::RNGWrapper(eng);
    m_wrappers.insert(std::make_pair(streamName,std::make_pair(wrp,eng)));
    return wrp;
  }
  ATH_MSG_WARNING("Returning engine for "<<streamName);

  return it->second.first;
}

void AthRNGSvc::CreateStream(uint64_t seed1, uint64_t seed2, 
			  const std::string& streamName){
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it=m_wrappers.find(streamName);
  if(it!=m_wrappers.end()){
    return;
  }
  size_t hash=hashCombine(seed1,seed2);
  hash=hashCombine(std::hash<std::string>{}(streamName),hash);
}

ATHRNG::RNGWrapper* AthRNGSvc::setOnDefinedSeeds(uint64_t theSeed,
							 const std::string& streamName){
  ATH_MSG_ERROR("You should not call this method");
  return nullptr;
}

ATHRNG::RNGWrapper* AthRNGSvc::setOnDefinedSeeds(uint64_t eventNumber,
							 uint64_t runNumber,
							 const std::string& streamName){
  ATH_MSG_ERROR("You should not call this method");
  return nullptr;
}
///seed all streams we manage, combining theSeed and the stream names
bool AthRNGSvc::setAllOnDefinedSeeds (uint64_t theSeed){
  ATH_MSG_ERROR("You should not call this method");
  return false;
}
///seed all streams, combining eventNumber, runNumber and the stream names
bool AthRNGSvc::setAllOnDefinedSeeds (uint64_t eventNumber, uint64_t runNumber){
  ATH_MSG_ERROR("You should not call this method");
  return false;
}

AthRNGSvc::~AthRNGSvc(){
  std::unordered_map<std::string,std::pair<ATHRNG::RNGWrapper*,CLHEP::HepRandomEngine*>>::iterator it=m_wrappers.begin();
  for(;it!=m_wrappers.end();++it){
    delete it->second.second;
    delete it->second.first;
  }
}

void AthRNGSvc::print(const std::string& streamName){
}

void AthRNGSvc::print(){
}
  
StatusCode AthRNGSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
    if ( IAthRNGSvc::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (IAthRNGSvc*)this;
    }
    else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}
