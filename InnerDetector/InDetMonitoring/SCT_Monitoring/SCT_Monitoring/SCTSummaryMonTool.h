/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTSummaryMonTool.h
 *   Class declaration for SCTSummaryMonTool
 *
 *
 *
 *    @author Shaun Roe
 *
 *
 *
 *
 */

#ifndef SCTSummaryMonTool_H
#define SCTSummaryMonTool_H

#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "SCT_Monitoring/SCTMotherTrigMonTool.h"

// Forward declarations
class IInterface;
class StatusCode;


///Concrete monitoring tool derived from SCTMotherTrigMonTool
class SCTSummaryMonTool : public SCTMotherTrigMonTool{
 public:
  SCTSummaryMonTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~SCTSummaryMonTool() = default;
   /**    @name Book, fill & check (reimplemented from baseclass) */
//@{
  ///Book histograms in initialization
  virtual StatusCode bookHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() ;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms();
 
//@}

private:
 
  int m_numberOfEvents;
};

#endif
