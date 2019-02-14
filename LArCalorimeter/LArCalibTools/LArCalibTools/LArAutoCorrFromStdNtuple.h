/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARAUTOCORRFROMSTDNTUPLE_H
#define LARAUTOCORRFROMSTDNTUPLE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <vector>
#include <string>

/** @class LArAutoCorrFromStdNtuple

This algorithm allows to read autocorrs from ntuples and builds a 
Version for standard Ntuple, produced by LArCalibTools algos....
With hardcoded numbers for sFcal
 */

class ILArMCSymTool;

class LArAutoCorrFromStdNtuple : public AthAlgorithm
{
 public:
  LArAutoCorrFromStdNtuple(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArAutoCorrFromStdNtuple();

  //standard algorithm methods
  /// implements IAlgorithm::initialize() 
  StatusCode initialize() ; 

  /// implements IAlgorithm::execute()  : Does nothing
  StatusCode execute() {return StatusCode::SUCCESS;}

  /// IAlgorithm::finalize() : Where the action takes place...
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();
 
 private:
  int m_nsamples;
  /// list of input ntuple file names 
  std::vector<std::string> m_root_file_names;
  /// ntuple name
  std::string m_ntuple_name;
  /// key of the LArAutoCorr collection in Storegate
  std::string m_store_key;
  /// Grouping type.  
  std::string m_groupingType;
  ///  type
  bool m_isComplete;
  /// drop FCAL and change to sFCal
  bool m_sFcal;

   ToolHandle<ILArMCSymTool> m_larmcsym;
   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
};

#endif
