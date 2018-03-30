/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 

#ifndef AFPSILAYERSUMMARYPROFILEBASE_H
#define AFPSILAYERSUMMARYPROFILEBASE_H

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <TProfile.h>

#include <string>

// forward declarations
class AFPHitsMonitorTool;


class AFPSiLayerSummaryProfileBase
{
public:
  AFPSiLayerSummaryProfileBase (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title);
  
  virtual ~AFPSiLayerSummaryProfileBase() {}

  void book ();

  TProfile* profile () const {return m_profile;}

protected:
  TProfile* m_profile;
  AFPHitsMonitorTool* m_parentTool;
  ManagedMonitorToolBase::MonGroup m_monGrp;
};

#endif
