/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RINGERDEBUGROI_H
#define RINGERDEBUGROI_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

class RingerDebugRoI {
 public:

  float m_eta;
  float m_phi;

  std::vector<float> m_etaCells;
  std::vector<float> m_phiCells;
  std::vector<float> m_eCells;
  std::vector<float> m_resEtaCells;
  std::vector<float> m_resPhiCells;
  std::vector<unsigned int> m_layerCells;

  unsigned int m_numCells;
};

CLASS_DEF( RingerDebugRoI , 109094748 , 1 )
CLASS_DEF( DataVector<RingerDebugRoI> , 188508235 , 1 )

#endif
