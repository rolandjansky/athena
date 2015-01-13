/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMPHYSOVERMCALFROMTuple_H
#define LARMPHYSOVERMCALFROMTuple_H

#include "AthenaBaseComps/AthAlgorithm.h"



#include <vector>
#include <string>

/** @class LArMphysOverMcalFromTuple

This algorithm allows to build a LArMphysOverMcalComplete container from the 
content of a root ntuple. 

The root tree should be named "outfit" and contain the following branches :
- BarAC :  int:  0 = barrel C   1 = barrel A
- FT :  int: feedthrough number (0-31) 
- Channel :  int:  channel number (0-14)
- Slot :  int: slot number (1-14)
- PhysOCal :  float :  value of Mphy/Mcal
 */


class LArMphysOverMcalFromTuple : public AthAlgorithm
{
 public:
  LArMphysOverMcalFromTuple(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArMphysOverMcalFromTuple();

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

  /// key of the PhysWave collection in StoreGate
  std::string m_store_key;

};

#endif
