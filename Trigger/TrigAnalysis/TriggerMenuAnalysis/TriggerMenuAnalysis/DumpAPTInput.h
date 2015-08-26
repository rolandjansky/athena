/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __DumpAPTInput_h__
#define __DumpAPTInput_h__
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
  std::string mOutputFileName;

  std::ofstream mOutputFile;

  ToolHandle<Trig::TrigDecisionTool> mTrigDecisionTool;
  const Trig::ChainGroup* mChainGroup_AllL1;
  const Trig::ChainGroup* mChainGroup_AllL2;
  const Trig::ChainGroup* mChainGroup_AllEF;
  std::map<int, std::string> mL1NameIdMap;
  std::map<int, std::string> mL2NameIdMap;
  std::map<int, std::string> mEFNameIdMap;

  std::bitset<N_L1_BITS> mL1_TBP;
  std::bitset<N_L1_BITS> mL1_TAP;
  std::bitset<N_L1_BITS> mL1_TAV;

  std::bitset<N_L2_BITS> mL2_Raw;
  std::bitset<N_L2_BITS> mL2_Rerun;

  std::bitset<N_EF_BITS> mEF_Raw;
  std::bitset<N_EF_BITS> mEF_Rerun;

};

#endif // __DumpAPTInput_h__
