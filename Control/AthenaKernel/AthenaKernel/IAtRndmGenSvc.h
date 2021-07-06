/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @class IAtRndmGenSvc
  * @brief manage multiple CLHEP random engines as named streams
  * 
  * @author Giorgos Stavropoulos <George.Stavropoulos@cern.ch> 
  * @author Paolo Calafiura <pcalafiura@lbl.gov> 
  */

#ifndef ATHENAKERNEL_IATRNDMGENSVC_H
# define ATHENAKERNEL_IATRNDMGENSVC_H

#include "GaudiKernel/IService.h"
#include <string>
#include "stdint.h"

namespace CLHEP {
   class HepRandomEngine;
}

class IAtRndmGenSvc : virtual public IService {
public:
  DeclareInterfaceID( IAtRndmGenSvc, 1, 0);

  /// Interface to the CLHEP engine
  //@{
  virtual CLHEP::HepRandomEngine* GetEngine(const std::string& streamName)=0;
  virtual void CreateStream(uint32_t seed1, uint32_t seed2, 
			    const std::string& streamName)=0;
  virtual CLHEP::HepRandomEngine* setOnDefinedSeeds(uint32_t theSeed,
	 				            const std::string& streamName)=0;
  virtual CLHEP::HepRandomEngine* setOnDefinedSeeds(uint32_t eventNumber,
						    uint32_t runNumber,
	 				            const std::string& streamName)=0;
  ///seed all streams we manage, combining theSeed and the stream names
  virtual bool setAllOnDefinedSeeds (uint32_t theSeed)=0; 
  ///seed all streams, combining eventNumber, runNumber and the stream names
  virtual bool setAllOnDefinedSeeds (uint32_t eventNumber, uint32_t runNumber)=0;
  //@}

  /// out-of-line destructor
  virtual ~IAtRndmGenSvc();
  
  /// Print methods
  //@{
  virtual void print(const std::string& streamName)=0;
  virtual void print()=0;
  //@}
};

#endif // ATHENAKERNEL_IATRNDMGENSVC_H
