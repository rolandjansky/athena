/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAROFCBIN_PHYSCALITDIFFFROMSTDNTUPLE_H
#define LAROFCBIN_PHYSCALITDIFFFROMSTDNTUPLE_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>


class LArOFCBin_PhysCaliTdiffFromStdNtuple : public AthAlgorithm
{
 public:
  LArOFCBin_PhysCaliTdiffFromStdNtuple(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArOFCBin_PhysCaliTdiffFromStdNtuple();

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
  /// key of the OFCBin collection in Storegate
  std::string m_store_key_ofcbin;
  bool m_fillofc;
  /// key of the PhysCaliTdiff collection in Storegate
  std::string m_store_key_tdiff;
  bool m_filltdiff;
  /// Grouping type.  Default is Feedthrough.
  std::string m_groupingType;
};

#endif
