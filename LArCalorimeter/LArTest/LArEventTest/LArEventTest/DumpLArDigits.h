/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPLARDIGITS_H
#define DUMPLARDIGITS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArDigitContainer.h"
//#include "LArRawEvent/LArDigitCollection.h"
#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"

//#include "LArDetDescr/LArDetDescrManager.h"
#include <fstream>

class DumpLArDigits : public AthAlgorithm
{
 public:
  DumpLArDigits(const std::string & name, ISvcLocator * pSvcLocator);

  ~DumpLArDigits();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  int m_count, m_chan;
  //LArDigitContainer* m_larDigitCont;
  const LArOnlineID*       m_onlineHelper; 
  LArCablingService *m_larCablingSvc;
  const LArEM_ID* m_emId;
  std::ofstream m_outfile;
  std::string m_key, m_FileName;
 private:
  class mySort {
  public:
    bool operator()(const LArDigit* a, const LArDigit* b);
  };
};

#endif
