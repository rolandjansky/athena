/** @class IAthRNGSvc
  * @brief manage multiple RandomEngines in thread-safe way.
  * 
  * Access to the RandomEngine is wrapped in the RNGWrapper
  * in order to ensure thread-safe access in the event loop.
  *
  * @author Sami Kama <sami.kama@cern.ch>
  */

#ifndef ATHENAKERNEL_IATHRNGSVC_H
#define ATHENAKERNEL_IATHRNGSVC_H

#include "GaudiKernel/IService.h"
#include <string>
#include "stdint.h"

namespace ATHRNG {
  class RNGWrapper;
}

class IAthRNGSvc : virtual public IService {
public:
  /// Interface to the CLHEP engine
  //@{
  virtual ATHRNG::RNGWrapper* GetEngine(const std::string& streamName)=0;
  virtual ATHRNG::RNGWrapper* setOnDefinedSeeds(uint64_t theSeed,
                                                const std::string& streamName)=0;
  virtual ATHRNG::RNGWrapper* setOnDefinedSeeds(uint64_t eventNumber,
                                                uint64_t runNumber,
                                                const std::string& streamName)=0;
  ///seed all streams we manage, combining theSeed and the stream names
  virtual bool setAllOnDefinedSeeds (uint64_t theSeed)=0; 
  ///seed all streams, combining eventNumber, runNumber and the stream names
  virtual bool setAllOnDefinedSeeds (uint64_t eventNumber, uint64_t runNumber)=0;
  //@}

  /// out-of-line destructor
  virtual ~IAthRNGSvc();
  
  /// Print methods
  //@{
  virtual void print(const std::string& streamName)=0;
  virtual void print()=0;
  //@}

  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();

};

inline
const InterfaceID& 
IAthRNGSvc::interfaceID() {
  static const InterfaceID IID("IAthRNGSvc", 1, 0);
  return IID;
}

#endif // ATHENAKERNEL_IATHRNGSVC_H
