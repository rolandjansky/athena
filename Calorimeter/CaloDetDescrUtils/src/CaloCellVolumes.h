/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETDESCRUTILS_CALOCELLVOLUMES_H
#define CALODETDESCRUTILS_CALOCELLVOLUMES_H

#include "Identifier/Identifier.h"
#include <vector>
#include <string>
#include <map>

class CaloCell_ID;
class ISvcLocator;

struct CaloCellVolume
{
  Identifier channelID;
  float volume;
};

typedef std::vector<CaloCellVolume> CaloCellVolumeVector;

class CaloCellVolumes
{
 public:
  CaloCellVolumes(ISvcLocator* svcLocator,
                  const CaloCell_ID* calocell_id);
  ~CaloCellVolumes();

  double CellVolume(Identifier cell_id);
  inline std::string layout() { return m_geometryLayout; }

  // For FCAL cell volumes which are calculated on the fly
  double getFcalTubeSpacing(int sampling);

  void print();

 private:
  const CaloCell_ID* m_calocell_id{nullptr};
  CaloCellVolumeVector m_cellVolumes;  
  std::string m_geometryLayout;

  // map fcal samplings on tube spacings
  typedef std::map<int, double> TubeSpacingMap;
  TubeSpacingMap m_fcalTubeSpacings{
    {1, 7.5}
    , {2, 8.179}
    , {3, 9.}
  };
};

#endif
