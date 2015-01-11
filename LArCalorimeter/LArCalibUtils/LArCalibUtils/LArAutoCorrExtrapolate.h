/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARAUTOCORRExtrapolate_H
#define LARAUTOCORRExtrapolate_H
/********************************************************************
 
 NAME:     LArAutoCorrExtrapolate.h
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  G.Unal
 CREATED:  sept 2008
  
 PURPOSE:  Extrapolate measured covariance matrix from highest gain in random
            to all gains
                      
********************************************************************/

// Include files
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class LArOnlineID;
class ILArBadChanTool;

//-----------------------------------------------------------------------
class LArAutoCorrExtrapolate : public AthAlgorithm
//-----------------------------------------------------------------------
{
 public:

  // Constructor
  LArAutoCorrExtrapolate(const std::string & name, ISvcLocator * pSvcLocator);

  // Destructor
  ~LArAutoCorrExtrapolate();

  // Algorithm initialization   
  StatusCode initialize(); 

  // Algorithm execution
  StatusCode execute();

  // Algorithm finalization
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
 private:

  // Container key list
  std::string m_keyoutput;
  std::string m_keyPedestal;
  std::string m_keyinput;
  std::string  m_keyPedInput;
  std::string m_keyRamp;

  int m_Nsamples;

  const LArOnlineID*        m_onlineId;

  ToolHandle<ILArBadChanTool> m_badChannelTool;
  bool m_useBad;

  
};

#endif

