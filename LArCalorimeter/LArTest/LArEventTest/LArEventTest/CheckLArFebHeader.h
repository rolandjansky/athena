//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CHECKLARFEBHEADERS_H
#define CHECKLARFEBHEADERS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"

// Small algorithm to test TTC information between FEB and ROD
// All this is stored in FebHeader objects
// Created July 17th 2004 by R. Lafaye

class CheckLArFebHeader : public AthAlgorithm
{
 public:
  CheckLArFebHeader(const std::string & name, ISvcLocator * pSvcLocator);

  ~CheckLArFebHeader();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  int m_count;
  const LArOnlineID* m_onlineHelper;
};


#endif
