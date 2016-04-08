/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALSYIS_DUMPAPTINPUT_H
#define TRIGGERMENUANALSYIS_DUMPAPTINPUT_H
/*
  DumpAPTInput.h
*/
#include <string>
#include <map>
#include <fstream>
#include <bitset>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

const int N_L1_BITS = 256;
const int N_L2_BITS = 1024;
const int N_EF_BITS = 1024;

class DumpAPTInput : public AthAlgorithm {
public:
  DumpAPTInput(const std::string& name, ISvcLocator* svcloc);
  ~DumpAPTInput();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  void readL1Map(const std::vector<std::string>& names, 
		 std::map<int, std::string>& name_map);
  void readHLTMap(const std::vector<std::string>& names, 
		  std::map<int, std::string>& name_map);

  void processL1(int id, const std::string& name);
  void processL2(int id, const std::string& name);
  void processEF(int id, const std::string& name);

  void dump();

  void reset();
  
private:
  // Properties
  std::string m_outputFileName;

  std::ofstream m_outputFile;

  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  const Trig::ChainGroup* m_chainGroup_AllL1;
  const Trig::ChainGroup* m_chainGroup_AllL2;
  const Trig::ChainGroup* m_chainGroup_AllEF;
  std::map<int, std::string> m_L1NameIdMap;
  std::map<int, std::string> m_L2NameIdMap;
  std::map<int, std::string> m_EFNameIdMap;

  std::bitset<N_L1_BITS> m_L1_TBP;
  std::bitset<N_L1_BITS> m_L1_TAP;
  std::bitset<N_L1_BITS> m_L1_TAV;

  std::bitset<N_L2_BITS> m_L2_Raw;
  std::bitset<N_L2_BITS> m_L2_Rerun;

  std::bitset<N_EF_BITS> m_EF_Raw;
  std::bitset<N_EF_BITS> m_EF_Rerun;

};

#endif // TRIGGERMENUANALSYIS_DUMPAPTINPUT_H
