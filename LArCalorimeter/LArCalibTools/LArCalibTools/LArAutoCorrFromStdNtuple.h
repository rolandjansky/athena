/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARAUTOCORRFROMSTDNTUPLE_H
#define LARAUTOCORRFROMSTDNTUPLE_H

#include "LArRawConditions/LArMCSym.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>
#include <string>

/** @class LArAutoCorrFromStdNtuple

This algorithm allows to read autocorrs from ntuples and builds a 
Version for standard Ntuple, produced by LArCalibTools algos....
With hardcoded numbers for sFcal
 */


class LArAutoCorrFromStdNtuple : public AthAlgorithm
{
 public:
  LArAutoCorrFromStdNtuple(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~LArAutoCorrFromStdNtuple();

  //standard algorithm methods
  /// implements IAlgorithm::initialize() 
  virtual StatusCode initialize() override;

  /// implements IAlgorithm::execute()  : Does nothing
  virtual StatusCode execute() override {return StatusCode::SUCCESS;}

  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}
  virtual StatusCode stop() override;
 
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

   SG::ReadCondHandleKey<LArMCSym> m_mcSymKey
   {this, "MCSymKey", "LArMCSym", "SG Key of LArMCSym object"};
   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
};

#endif
