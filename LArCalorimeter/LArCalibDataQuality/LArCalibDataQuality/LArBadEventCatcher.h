/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emeacs, this is -*-C++-*-

/**
 * @file  LArBadEventCatcher.h
 * @author Walter Lampl <walter.lampl @cern.ch>
 * @date June 2008
 * @brief Algorithm to catch corrupt events and stop the job
 */

#ifndef LARCALIBUTILS_LARBADEVENTCATCHER
#define LARCALIBUTILS_LARBADEVENTCATCHER

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>

class LArOnlineID;

/** 
  * @class LArBadEventCatcher
  * Algorithm to catch corrupt events 
  *
  * Unlike monitoring algorithms (that wait for the job to finish, need subsequent DQMF checks, etc)
  * this algorithm reacts imedatly if a corrupt event is identfied.It returns a StatusCode::FAILURE 
  * in the execute as well as in the finalize and causes athena to exit without any delay.
  */

class LArBadEventCatcher: public AthAlgorithm {
public:

  // Constructor
  LArBadEventCatcher(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArBadEventCatcher();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();
  
 private:


  std::string decipherFebError(const uint32_t errword) const;


  std::string eventDetails() const;
 
  //Online ID helper
  const LArOnlineID* m_onlineID;

  
  //jobProperties
  bool m_checkDigits;
  bool m_checkAccCalibDigits;
  bool m_checkFebHeaders;
  bool m_checkBSErrors;
  

  uint16_t  m_BSErrFailMask;
  unsigned m_nEvent;

  std::size_t m_nDigits, m_nAccCalibDigits, m_nFebheaders;
  std::size_t m_thisSize;
  std::vector<std::string> m_keyList;

  bool m_stopJob;
  bool m_stopOnError;
};

#endif
