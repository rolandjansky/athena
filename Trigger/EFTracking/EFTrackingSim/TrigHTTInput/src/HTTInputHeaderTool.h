/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTInputHeaderTool_H
#define HTTInputHeaderTool_H

/**
 * @file HTTInputHeaderTool.h
 *
 * This class reads HTT input data from a ROOT file (wrapper file)
 * Designed to be not thread-safe
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"

#include <atomic>

class HTTEventInputHeader;

class HTTInputHeaderTool : public extends<AthAlgTool, IHTTEventInputHeaderTool>  {
 public:
 
  HTTInputHeaderTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTTInputHeaderTool() = default;    
  virtual StatusCode initialize() override; 
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header, bool &last)  override;
  virtual StatusCode writeData(HTTEventInputHeader* header)  override; 
  
  
 private:
  StringArrayProperty   m_inpath          {this, "InFileName", {"."}, "input file paths"};
  StringProperty        m_rwoption        {this, "RWstatus", std::string("READ"), "define read or write file option: READ, RECREATE, HEADER"};


  //internal counters  
  std::atomic<unsigned> m_event = 0;
  std::atomic<unsigned> m_totevent = 0;
  std::atomic<unsigned> m_file = 0;
 
  static const std::string m_branchName;
  StatusCode openFile(std::string const & path);

};

#endif // HTTInputHeaderTool_H
