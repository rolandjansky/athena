/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------------------------------------
// Selecting a LArCaliWave according DAC
//
// Author: P. Strizenec
// September 2008
// ----------------------------------------------------------------------------

#ifndef LARCALIWAVESELECTOR_H
#define LARCALIWAVESELECTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>
#include <map>

typedef std::pair< std::pair<int,int>, int> DetGain;

class LArOnlineID;
class CaloCell_ID;
class LArOnOffIdMap;

class LArCaliWaveSelector : public AthAlgorithm
{
 public:
  LArCaliWaveSelector(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCaliWaveSelector();

  StatusCode initialize();
  StatusCode execute() { return StatusCode::SUCCESS;};
  StatusCode stop();
  StatusCode finalize(){ return StatusCode::SUCCESS;}
    
 private:

  void parseSelection();

  const CaloCell_ID *m_cellID;

  std::map<DetGain, int> m_mapDAC;
  unsigned short m_gmask;

  std::vector<std::string> m_keyList;
  std::vector<std::string> m_selectionList;
  std::string m_outputKey;
  std::string m_groupingType;

};  

#endif
