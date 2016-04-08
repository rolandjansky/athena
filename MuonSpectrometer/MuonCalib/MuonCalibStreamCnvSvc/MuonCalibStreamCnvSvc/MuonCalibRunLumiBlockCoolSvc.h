/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibRunLumiBlockCoolSvc_h
#define MuonCalibRunLumiBlockCoolSvc_h

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IService.h"

//c - c++
#include <string>

const InterfaceID IID_IMuonCalibRunLumiBlockCoolSvc("MuonCalibRunLumiBlockCoolSvc", 1, 0);

class MuonCalibRunLumiBlockCoolSvc: public AthService {
 public:
  MuonCalibRunLumiBlockCoolSvc(const std::string & name, ISvcLocator *svc_locator);
  virtual ~MuonCalibRunLumiBlockCoolSvc();
  static const InterfaceID& interfaceID() { return IID_IMuonCalibRunLumiBlockCoolSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize();
  virtual StatusCode finalize();
	
  //retrieve lumiblock for timestamp. If run_number<0 get run number too.	
  StatusCode GetRunEventNumber(unsigned int timestamp,  int &run_number,  int &lb_nr);
		
 private:
  //job options
  std::string m_lumi_block_fldr_by_lumiblock;
  std::string m_lumibloc_fldr_by_time;
  
  //lumiblock map for current run		
  std::map<unsigned int, int> m_lumiblock_map;
  int m_valid_for_run;
  //time range of current run
  unsigned int m_min_run_time, m_max_run_time;		
  //current lumi block - reduce map lookups
  int m_current_lumi_block;
  unsigned int m_min_lb_time, m_max_lb_time;
  //load run number for time stamp	
  inline StatusCode get_run_number(const unsigned int & timestamp, int &run_number);
  //load lumiblock map for current run
  inline StatusCode load_lumiblock_map(const int & m_run_number);
};

#endif
