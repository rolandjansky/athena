/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1LightRun                           //
//                                                         //
//  author: Riccardo-Maria BIANCHI <rbianchi@cern.ch>      //
//          23 May 2014                                    //
//                                                         //
//  This is the Athena algorithm starting a VP1Light GUI   //
//                                                         //
/////////////////////////////////////////////////////////////


#ifndef VP1LIGHT_VP1RUN
#define VP1LIGHT_VP1RUN

#include <string>
#include <vector>

class VP1Gui;

class VP1LightRun
{
 public:
  // VP1LightRun(const std::string& name, ISvcLocator* pSvcLocator);
  VP1LightRun(const std::string& name);
  ~VP1LightRun();

  bool initialize();
  bool execute();
  bool finalize();

  // void handle(const Incident& inc);
  void handle();

 private:
  // IToolSvc* m_toolSvc;
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
