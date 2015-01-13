/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARPHYSWAVEFROMTuple_H
#define LARPHYSWAVEFROMTuple_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

/** @class LArPhysWaveFromTuple

This algorithm allows to read wave forms from ntuples and builds a 
LArPhysWaveContainer conating the corresponding PhysWave. The root tree should
be named "outfit" and contain the following branches :
- BarEC :  int:  0 = Barrel  1 = EndCap
    for backwards compatibility, a missing BarEC branch assumes all values to
    be Barrel.
- Side (or BarAC) :  int:  0 = barrel C   1 = barrel A
- FT :  int: feedthrough number (0-31) 
- Channel :  int:  channel number (0-127)
- Slot :  int: slot number (1-14)
- tphys :  float array:  waveform
 */


class LArPhysWaveFromTuple : public AthAlgorithm
{
 public:
  LArPhysWaveFromTuple(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArPhysWaveFromTuple();

  //standard algorithm methods
  /// implements IAlgorithm::initialize() 
  StatusCode initialize() ; 

  /// implements IAlgorithm::execute()  : Does nothing
  StatusCode execute() {return StatusCode::SUCCESS;}

  /// IAlgorithm::finalize() : Where the action takes place...
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();
 
 private:
  /// number of points of the waveform in the ntuple
  unsigned int m_NPoints;
  /// the first  m_skipPoints points of the waveform in the ntuple are skipped
  unsigned int m_skipPoints;
  /// make a PhysWave with the first m_prefixPoints as zeros
  unsigned int m_prefixPoints;
  /// time step of the the waveform in the ntuple
  double m_deltaT;
  /// flag for the PhysWave container
  unsigned int m_flag;
  /// list of input ntuple file names 
  std::vector<std::string> m_root_file_names;
  /// key of the PhysWave collection in Storegate
  std::string m_store_key;
  /// Grouping type.  Default is Feedthrough.
  std::string m_groupingType;
};

#endif
