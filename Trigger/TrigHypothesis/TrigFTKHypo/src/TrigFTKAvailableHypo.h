/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
<<<<<<< HEAD
=======
*/
/*
>>>>>>> upstream/master
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




