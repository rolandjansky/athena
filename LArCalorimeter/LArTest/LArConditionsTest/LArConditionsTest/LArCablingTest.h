/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCABLINGTEST_H
#define LARCABLINGTEST_H

//Dear emacs, this is -*-c++-*-
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/HWIdentifier.h"

class LArCablingService;
class LArOnlineID;
class CaloCell_ID;

class LArCablingTest : public AthAlgorithm
{
 public:
  LArCablingTest(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCablingTest();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() ;
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:
  ToolHandle<LArCablingService> m_larCablingSvc;
  bool m_print;
  unsigned m_mode;
  const LArOnlineID* m_onlineId;
  const CaloCell_ID* m_caloCellId;

  void print (const HWIdentifier& hwid, std::ostream& out); 
};

#endif
