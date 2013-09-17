/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READLARCALIBDIGITS_H
#define READLARCALIBDIGITS_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include  "StoreGate/StoreGateSvc.h"
#include "TBEvent/TBLArCalibDigitContainer.h"

#include <fstream>

class LArOnlineID;

class ReadTBLArCalibDigits : public Algorithm
{
 public:
  ReadTBLArCalibDigits(const std::string & name, ISvcLocator * pSvcLocator);

  ~ReadTBLArCalibDigits();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  int m_count;
  StoreGateSvc *m_storeGateSvc;
  const LArOnlineID* m_onlineHelper;
  std::ofstream m_outfile;
  std::string m_containerKey;
  std::string m_dumpFile;

};

#endif
