/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENALGS_INDETALIGNWRT_H
#define INDETALIGNGENALGS_INDETALIGNWRT_H

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODEventInfo/EventInfo.h"
class IBeamCondSvc;

class InDetAlignWrt:public AthAlgorithm
{
public:
  InDetAlignWrt(const std::string& name, ISvcLocator* pSvcLocator);
  ~InDetAlignWrt(void);

  StatusCode  initialize(void);    
  StatusCode  execute(void);
  StatusCode  finalize(void);  

private: 
  ServiceHandle<IBeamCondSvc> m_beamcondsvc; 
  ToolHandle<IInDetAlignDBTool> p_iddbtool;
  ToolHandle<IInDetAlignDBTool> p_migratetool;
  const xAOD::EventInfo* p_eventinfo;
  bool m_setup;

  // algorithm parameters
  bool m_par_create;
  bool m_par_wrt;
  bool m_par_wrtiov;
  bool m_par_print;
  bool m_par_migrate;
  std::string m_par_rfile;
  std::string m_par_wfile;
  std::string m_par_dispfile;
  bool m_par_ntuple;
  int m_par_wrtrun;
  int m_par_wrtevent;
  unsigned int m_par_run1;  // IOV to be filled
  unsigned int m_par_run2;
  unsigned int m_par_evt1;
  unsigned int m_par_evt2;
  std::string m_par_iovtag;
  std::string m_par_dbtoolinst;

  // parameters to add displacements
  int m_par_dispmode;
  int m_par_dettype;
  int m_par_bec;
  int m_par_layer;
  int m_par_ring;
  int m_par_sector;
  float m_par_rphidisp;
  float m_par_rdisp;
  float m_par_zdisp;
  int m_par_systdisp;
  int m_par_irskip;
  int m_par_dispcsc;
  bool m_par_wrtbeam;
  bool m_par_writetop;
  std::string m_par_topname;

  // private methods
  void DispDB();
  void dispFile();
  void doOutput();
  void genTopTrans();
  void doMigration();
};

#endif // INDETALIGNGENALGS_INDETALIGNWRT_H
