/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSHAPEFROMSTDNTUPLE_H
#define LARSHAPEFROMSTDNTUPLE_H

#include "LArRawConditions/LArMCSym.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>
#include <string>

/** @class LArShapeFromStdNtuple

This algorithm allows to read wave forms from ntuples and builds a 
LArPhysWaveContainer containing the corresponding PhysWave. 
Version for standard Ntuple, produced by LArCalibTools algos....
 */

class LArShapeFromStdNtuple : public AthAlgorithm
{
 public:
  LArShapeFromStdNtuple(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~LArShapeFromStdNtuple();

  //standard algorithm methods
  /// implements IAlgorithm::initialize() 
  virtual StatusCode initialize() override;

  /// implements IAlgorithm::execute()  : Does nothing
  virtual StatusCode execute() override {return StatusCode::SUCCESS;}

  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}
  virtual StatusCode stop() override;
 
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

  SG::ReadCondHandleKey<LArMCSym> m_mcSymKey
  {this, "MCSymKey", "LArMCSym", "SG Key of LArMCSym object"};
};

#endif
