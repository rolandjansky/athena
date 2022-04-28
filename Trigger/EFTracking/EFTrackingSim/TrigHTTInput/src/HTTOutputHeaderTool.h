/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_READOUTPUTHEADERTOOL_H
#define HTT_READOUTPUTHEADERTOOL_H

/**
 * @file HTTOutputHeaderTool.h
 *
 * This class reads/write HTT output data from/to a ROOT file 
 * Designed to be not thread-safe
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/IHTTEventOutputHeaderTool.h"
#include <numeric>
#include <atomic>

class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;

class HTTOutputHeaderTool : public extends<AthAlgTool, IHTTEventOutputHeaderTool>  
{

public:

  HTTOutputHeaderTool(std::string const &, std::string const &, IInterface const *);
  virtual ~HTTOutputHeaderTool()  = default;
  virtual StatusCode initialize() override; 
  virtual StatusCode finalize()   override;

  virtual StatusCode readData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader, bool &last) override;
  virtual StatusCode writeData(HTTLogicalEventInputHeader* INheader_1st, HTTLogicalEventInputHeader* INheader_2nd, HTTLogicalEventOutputHeader* OUTheader)            override;
  
  std::string fileName() { return std::accumulate(m_inpath.value().begin(), m_inpath.value().end(), std::string{}); }

private:
  // JO configuration
  StringArrayProperty   m_inpath          {this, "InFileName", {"."}, "input file paths"};
  StringProperty        m_rwoption        {this, "RWstatus", std::string("READ"), "define read or write file option: READ, RECREATE, HEADER"};
  BooleanProperty       m_runSecondStage  {this, "RunSecondStage",false, "flag to enable running the second stage fitting"};

  // internal counters  
  std::atomic<unsigned> m_event = 0;
  std::atomic<unsigned> m_totevent = 0;
  std::atomic<unsigned> m_file = 0;
 
  // branches
  static const std::string m_branchNameIn_1st;
  static const std::string m_branchNameIn_2nd;
  static const std::string m_branchNameOut;    
  
  StatusCode openFile(std::string const & path);

};

#endif // HTT_READOUTPUTHEADERTOOL_H
