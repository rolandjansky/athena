/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_HitsMoniTool_H
#define LUCID_HitsMoniTool_H

#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "TH1F.h"

class TH1F;

class LUCID_HitsMoniTool: public ManagedMonitorToolBase {

 public:
  
  LUCID_HitsMoniTool(const std::string& type, 
		     const std::string& name,
		     const IInterface* parent); 
  
  virtual ~LUCID_HitsMoniTool();
  
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  
 protected:

  const LUCID_RawDataContainer* m_LUCID_RawDataContainer;
  
  std::vector<TH1F*> m_LUCID_Histos;

  static const int m_nLucidTubes = 40;
};

#endif
