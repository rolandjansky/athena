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
  bool par_create;
  bool par_wrt;
  bool par_wrtiov;
  bool par_print;
  bool par_migrate;
  std::string par_rfile;
  std::string par_wfile;
  std::string par_dispfile;
  bool par_ntuple;
  int par_wrtrun;
  int par_wrtevent;
  unsigned int par_run1;  // IOV to be filled
  unsigned int par_run2;
  unsigned int par_evt1;
  unsigned int par_evt2;
  std::string par_iovtag;
  std::string par_dbtoolinst;

  // parameters to add displacements
  int par_dispmode;
  int par_dettype;
  int par_bec;
  int par_layer;
  int par_ring;
  int par_sector;
  float par_rphidisp;
  float par_rdisp;
  float par_zdisp;
  int par_systdisp;
  int par_irskip;
  int par_dispcsc;
  bool par_wrtbeam;
  bool par_writetop;
  std::string par_topname;

  // private methods
  void DispDB();
  void dispFile();
  void doOutput();
  void genTopTrans();
  void doMigration();
};

#endif // INDETALIGNGENALGS_INDETALIGNWRT_H
