/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////
//TRTStrawStatusWrite.h
//Ashfaq Ahmad 17/02/2005 Based on SCT CondtionsAlg
////////////////////////////////////////////////////


#ifndef TRTSTRAWSTATUSWRITE_H
#define TRTSTRAWSTATUSWRITE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

class ITRT_StrawStatusSummarySvc;
class ITRT_ConditionsSvc;

class TRTStrawStatusWrite : public AthAlgorithm
{

 public:

  TRTStrawStatusWrite( const std::string &name, ISvcLocator *pSvcLocator);
  ~TRTStrawStatusWrite();

  // Gaudi hooks
  StatusCode initialize( );
  StatusCode execute( );
  StatusCode finalize( );

 private:

  ServiceHandle<ITRT_ConditionsSvc> m_trtStrawStatusIF;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_trtStrawStatus;
  std::string  par_statusfile;
  std::string  par_statusfilepermanent;
  std::string  par_statusfileHT;
  bool m_setup;
};


#endif
