/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLVOLUMES_H
#define CALOCELLVOLUMES_H

#include <vector>
#include <string>
#include <map>
#include "Identifier/Identifier.h"

class CaloCell_ID;
class ISvcLocator;

/** This class is a modification of LArDetDescr/LArCellVolumes.
    Presently it provides volumes for LAr cells only.

*/

struct CaloCellVolume
{
  Identifier channelID;
  float volume;
};

typedef std::vector<CaloCellVolume> CaloCellVolumeVector;

class CaloCellVolumes
{
public:

  // Constructors:
  CaloCellVolumes(ISvcLocator* svcLocator,
                  const CaloCell_ID* calocell_id);
  virtual ~CaloCellVolumes();

  double CellVolume(Identifier cell_id);
  inline std::string layout() { return m_geometryLayout; }

  // For FCAL cell volumes which are calculated on the fly
  double getFcalTubeSpacing(int sampling);

  virtual void print();

private:
  const CaloCell_ID* m_calocell_id ;
  ISvcLocator* m_svcLocator;
  CaloCellVolumeVector m_cellVolumes;  
  std::string m_geometryLayout;

  // map fcal samplings on tube spacings
  typedef std::map<int, double, std::less<int> > TubeSpacingMap;
  TubeSpacingMap m_fcalTubeSpacings;
};

 
#endif
