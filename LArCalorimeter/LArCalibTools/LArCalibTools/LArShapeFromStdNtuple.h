/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSHAPEFROMSTDNTUPLE_H
#define LARSHAPEFROMSTDNTUPLE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <string>

/** @class LArShapeFromStdNtuple

This algorithm allows to read wave forms from ntuples and builds a 
LArPhysWaveContainer containing the corresponding PhysWave. 
Version for standard Ntuple, produced by LArCalibTools algos....
 */

class ILArMCSymTool;

class LArShapeFromStdNtuple : public AthAlgorithm
{
 public:
  LArShapeFromStdNtuple(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArShapeFromStdNtuple();

  //standard algorithm methods
  /// implements IAlgorithm::initialize() 
  StatusCode initialize() ; 

  /// implements IAlgorithm::execute()  : Does nothing
  StatusCode execute() {return StatusCode::SUCCESS;}

  /// IAlgorithm::finalize() : Where the action takes place...
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();
 
 private:
  /// the first  m_skipPoints points of the waveform in the ntuple are skipped
  unsigned int m_skipPoints;
  /// make a Shape with the first m_prefixPoints as zeros
  unsigned int m_prefixPoints;
  /// list of input ntuple file names 
  std::vector<std::string> m_root_file_names;
  /// ntuple name
  std::string m_ntuple_name;
  /// key of the LArShape collection in Storegate
  std::string m_store_key;
  /// Grouping type.  
  std::string m_groupingType;
  /// Shape type
  bool m_isComplete;

  bool m_done;

   ToolHandle<ILArMCSymTool> m_larmcsym;
};

#endif
