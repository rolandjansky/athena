/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENASUMSVC_H
#define ATHENASERVICES_ATHENASUMSVC_H

/*****************************************************************************
 *
 *  AthenaSummarySvc.h
 *  AthenaSummarySvc
 *
 *  Author: Charles Leggett
 *  $Id: $
 *
 *  Provides summary at end of athena job
 *
 *****************************************************************************/

#include "AthenaBaseComps/AthService.h"
#ifndef KERNEL_STATUSCODES_H
 #include "GaudiKernel/StatusCode.h"
#endif
#ifndef GAUDIKERNEL_CLASSID_H
 #include "GaudiKernel/ClassID.h"
#endif
#ifndef GAUDIKERNEL_MSGSTREAM_H
 #include "GaudiKernel/MsgStream.h"
#endif
#ifndef SGTOOLS_DATAPROXY_H
 #include "SGTools/DataProxy.h"
#endif

#ifndef  ATHENAKERNEL_IATHENASUMMARYSVC_H
 #include "AthenaKernel/IAthenaSummarySvc.h"
#endif

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/ILoggedMessageSvc.h"

#include <list>
#include <map>
#include <string>
#include <fstream>

// Forward declarations



class AthenaSummarySvc: virtual public AthService, 
		 virtual public IIncidentListener,
		 virtual public IAthenaSummarySvc {

public:

  AthenaSummarySvc( const std::string& name, ISvcLocator* svc );
  
  // Destructor.
  virtual ~AthenaSummarySvc();

  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface );

  StatusCode createSummary();
  void setStatus(int s) {m_status = s;}
  void addListener(const std::string& incident_name);
  void addSummary(const std::string& dict_key, const std::string& data);

  const std::string& getOutputFile() const { return m_summaryFile; }

private:

  void createDict(std::ofstream& );
  void createASCII(std::ofstream& );

  // methods
  void handle(const Incident &inc);
  static void newHandler();

  // properties
  StringProperty m_summaryFile, m_summaryFormat;
  StringArrayProperty m_extraInc, m_keywords;

  // member data
  mutable MsgStream m_log;
  ServiceHandle<IIncidentSvc> p_incSvc;
  ILoggedMessageSvc* p_logMsg;
  std::new_handler m_new;
  static char* s_block;
  static bool s_badalloc;
  int m_status;


  std::list<std::string> m_inputFilesRead;
  std::list<std::string> m_inputFilesError;
  std::list<std::string> m_outputFiles;
  std::list<std::string> m_outputFilesError;

  std::list<std::string> m_tagFilesRead;
  std::list<std::string> m_tagFilesError;

  std::map< std::string, std::map<std::string, int> > m_extraIncidents;
  std::vector< std::pair<std::string, std::string> > m_extraInfo;

  unsigned int m_eventsRead, m_eventsWritten, m_eventsSkipped, m_runs;
  
  


};

#endif
