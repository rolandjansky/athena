/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILESTAGERALG_H
#define FILESTAGERALG_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>
#include <vector>

class StoreGateSvc;
class TStopwatch;
class TH1D;

/////////////////////////////////////////////////////////////////////////////

class FileStagerAlg : public Algorithm, 
                      public IIncidentListener {

public:
  FileStagerAlg( const std::string& name, ISvcLocator* pSvcLocator );
  ~FileStagerAlg(); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident& inc);

private:

  void configStager();
  void loadStager();
  void releasePrevFile();
  void setupNextFile();

  int  m_pipeLength;
  bool m_verbose;
  bool m_verboseWait;
  bool m_firstFileAlreadyStaged;
  std::string m_treeName;
  std::string m_infilePrefix;
  std::string m_outfilePrefix;
  std::string m_cpCommand;
  std::string m_baseTmpdir;
  std::string m_logfileDir;
  bool m_keepLogfiles;
  bool m_storeStats;
  std::vector< std::string > m_cpArg;
  std::vector< std::string > m_inCollection;
  std::vector< std::string > m_outCollection;

  std::vector< std::string >::iterator m_fItr;
  std::string m_prevFile;

  TStopwatch* m_stopwatch;
  double m_waittime;
  TH1D* m_waithist;
};

#endif // FILESTAGERALG_H
