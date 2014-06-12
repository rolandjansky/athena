/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCALIBTEST_DUMPCALOBADCHANNELS
#define LARCALIBTEST_DUMPCALOBADCHANNELS

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloConditions/ICaloBadChanTool.h"
#include "CaloIdentifier/CaloCell_ID.h"

class DumpCaloBadChannels : public Algorithm
{
 public:
  DumpCaloBadChannels(const std::string & name, ISvcLocator * pSvcLocator);
  ~DumpCaloBadChannels();

  StatusCode initialize(); 
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle<ICaloBadChanTool> m_caloBCT;
  std::string m_fileName;
  const CaloCell_ID* m_cellID;


};

#endif
