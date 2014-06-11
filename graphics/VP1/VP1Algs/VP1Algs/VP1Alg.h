/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1Alg                           //
//                                                         //
//  update: Riccardo-Maria BIANCHI <rbianchi@cern.ch>      //
//          23 May 2014                                    //
//                                                         //
//  This is the Athena algorithm starting a VP1 GUI        //
//                                                         //
/////////////////////////////////////////////////////////////


#ifndef VP1ALGS_VP1ALG
#define VP1ALGS_VP1ALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>
#include <vector>

class VP1Gui;

class VP1Alg: public AthAlgorithm,
	      public IIncidentListener
{
 public:
  VP1Alg(const std::string& name, ISvcLocator* pSvcLocator);
  ~VP1Alg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident& inc);

 private:
  IToolSvc* m_toolSvc;
  VP1Gui * m_vp1gui;

  std::vector<std::string> m_initialvp1files;
  std::string m_initialCruiseMode;//"NONE", "EVENT", "TAB", "BOTH".
  unsigned m_initialCruiseSeconds;

  bool m_noGui;//For testing job-options in RTT

  // Properties for multiple input files (mf)
  bool m_mfOn;                        // Flag to turn multiple files ON/OFF. Default OFF
  std::string m_mfSourceDir;          // Directory to take event files from
  std::string m_mfLocalCopyDir;       // Directory to keep local copies of processed events. Default "."
  int m_mfLimit;                      // Maximum number of local copies to keep
  std::vector<std::string> m_mfAvailableLocalInputDirectories;//Will only be used if sourcedir is set and local
};

#endif
