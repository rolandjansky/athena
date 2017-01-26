/*
  Copyright (C) 1995-2017 CERN for the benefit of the ATLAS collaboration
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/** @class TrigFTKAvailableHypo                                                                                                            
    @author Jiri Masik <jiri.masik@manchester.ac.uk>

    Test availability of the FTK in the event and

*/                                                                                                                                                           

#ifndef TRIG_FTKAvailableHypo_H
#define TRIG_FTKAvailableHypo_H

// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

// trigger includes
#include "TrigInterfaces/HypoAlgo.h"


class TrigFTKAvailableHypo: public HLT::HypoAlgo  {
  
 public:
  TrigFTKAvailableHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigFTKAvailableHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode checkDetMask();

 private:

  bool m_acceptAll;            //!<  true will accept all events
  bool m_ftk_in;               //!<  FTK availability in the data / from DetMask 

};

#endif




