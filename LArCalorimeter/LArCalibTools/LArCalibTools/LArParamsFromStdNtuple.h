/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARPARAMSFROMSTDNTUPLE_H
#define LARPARAMSFROMSTDNTUPLE_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

/** @class LArParamsFromStdNtuple

This algorithm allows to read wave forms from ntuples and builds a 
LArCaliPulseParamsComplete and/or LArDetCellParamsComplete . 
Version for standard Ntuple, produced by LArCalibTools algos....
 */


class LArParamsFromStdNtuple : public AthAlgorithm
{
 public:
  LArParamsFromStdNtuple(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArParamsFromStdNtuple();

  //standard algorithm methods
  /// implements IAlgorithm::initialize() 
  StatusCode initialize() ; 

  /// implements IAlgorithm::execute()  : Does nothing
  StatusCode execute() {return StatusCode::SUCCESS;}

  /// IAlgorithm::finalize() : Where the action takes place...
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();
 
 private:
  /// list of input ntuple file names 
  std::vector<std::string> m_root_file_names;
  /// ntuple name
  std::string m_ntuple_name;
  /// key of the CaliPulseParams collection in Storegate
  std::string m_store_key_cali;
  /// key of the DetCellParams collection in Storegate
  std::string m_store_key_det;
  /// Grouping type.  Default is Feedthrough.
  std::string m_groupingType;
};

#endif
