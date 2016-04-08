/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDEXAMPLE_COOLHISTEXAMPLE_H
#define DETDESCRCONDEXAMPLE_COOLHISTEXAMPLE_H
// CoolHistExample - simple algorithm to demonstrate access to histograms
// stored/accesed using CoolHistSvc
// Richard Hawkings, started 09/2/07

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IOVSvcDefs.h"

class ICoolHistSvc;
class TH1;
class TObject;
class TDirectory;

class CoolHistExample: public AthAlgorithm
{
 public:
  CoolHistExample(const std::string& name, ISvcLocator* pSvcLocator);
  ~CoolHistExample(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

  StatusCode callBack( IOVSVC_CALLBACK_ARGS );

 private:
  ICoolHistSvc* p_coolhistsvc;
  const std::string m_histfolder; // COOL folder to access
  const std::string m_dirfolder; // COOL folder to access for TDirectory test
  const std::string m_histname; // histogram name
  const std::string m_objname; // generic TObject name
  const std::string m_dirname; // TDirectory name
  const int m_channel; // histogram channel number
  bool m_retrievedir;   // also test retrieval of TDirectory
  TH1* m_histptr; // cached pointer to histogram - note TH1 not TH1F
  TObject* m_objptr; // cached pointer to generic object
  TDirectory* m_dirptr; // cached pointer to directory
};

#endif // DETDESCRCONDEXAMPLE_COOLHISTEXAMPLE_H
