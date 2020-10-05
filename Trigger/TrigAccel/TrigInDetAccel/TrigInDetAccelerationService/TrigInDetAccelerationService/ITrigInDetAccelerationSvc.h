/*   
     Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETACCELERATIONSERVICE_ITRIGINDETACCELERATIONSVC_H
#define TRIGINDETACCELERATIONSERVICE_ITRIGINDETACCELERATIONSVC_H

#include "GaudiKernel/IService.h"

#include "TrigAccelEvent/Work.h"
#include "TrigAccelEvent/OffloadBuffer.h"

#include<memory>
#include<vector>


/// Service Interface for TrigInDetAccelerationSvc class 

class ITrigInDetAccelerationSvc : virtual public IService { 

 public:

  /// Interface ID   
  DeclareInterfaceID(ITrigInDetAccelerationSvc,1,0);

  // main methods

  virtual bool isReady() const  = 0;
  virtual TrigAccel::Work* createWork(unsigned int, std::shared_ptr<TrigAccel::OffloadBuffer>) const = 0;
  
  //helper

  virtual const std::vector<short>& getLayerInformation(int) const = 0;

};

#endif

