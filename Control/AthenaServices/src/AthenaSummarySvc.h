/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENASUMSVC_H
#define ATHENASERVICES_ATHENASUMSVC_H

/*****************************************************************************
 *
 *  AthenaSummarySvc.h
 *  AthenaSummarySvc
 *
 *  Author: Charles Leggett
 *
 *  Provides summary at end of athena job
 *
 *****************************************************************************/

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IAthenaSummarySvc.h"
#include "AthenaKernel/ILoggedMessageSvc.h"
#include "CxxUtils/checker_macros.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include <list>
#include <map>
#include <string>
#include <fstream>


class AthenaSummarySvc :
  public extends<AthService, IIncidentListener, IAthenaSummarySvc> {

public:

  AthenaSummarySvc( const std::string& name, ISvcLocator* svc );

  virtual StatusCode initialize() override;
  virtual StatusCode reinitialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode createSummary() override;
  virtual void setStatus(int s) override {m_status = s;}
  virtual void addListener(const std::string& incident_name) override;
  virtual void addSummary(const std::string& dict_key, const std::string& data) override;
  virtual const std::string& getOutputFile() const override { return m_summaryFile; }

private:

  void createDict(std::ofstream& );
  void createASCII(std::ofstream& );
  virtual void handle(const Incident &inc) override;
  static void newHandler();

  // properties
  StringProperty m_summaryFile{this, "SummaryFile", "AthSummary.txt",
                               "Output File"};
  StringProperty m_summaryFormat{this, "SummaryFileFormat", "both",
                                 "output format: one of 'ascii', 'python', 'both'"};
  StringArrayProperty m_extraInc{this, "ExtraIncidents", {},
                                 "user incidets to monitor"};
  StringArrayProperty m_keywords{this, "keywords", {},
                                 "kewords to scan for in MessageSvc. WARNING: THIS IS VERY SLOW!!!"};

  // member data
  ServiceHandle<IIncidentSvc> p_incSvc;
  ILoggedMessageSvc* p_logMsg{nullptr};

  static char* s_block ATLAS_THREAD_SAFE;    // used during initialize
  static bool s_badalloc ATLAS_THREAD_SAFE;  // flag set after bad-alloc

  std::list<std::string> m_inputFilesRead;
  std::list<std::string> m_outputFiles;
  std::list<std::string> m_outputFilesError;

  std::map< std::string, std::map<std::string, int> > m_extraIncidents;
  std::vector< std::pair<std::string, std::string> > m_extraInfo;

  std::new_handler m_new{nullptr};
  int m_status{0};
  unsigned int m_eventsRead{0};
  unsigned int m_eventsWritten{0};
  unsigned int m_eventsSkipped{0};
  unsigned int m_runs{0};
};

#endif
