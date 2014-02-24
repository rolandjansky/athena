/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELL_VOLUMES_H
#define LARCELL_VOLUMES_H

class Identifier;
class CaloCell_ID;
class IRDBRecordset;

/** This class builds a vector of LArCellVolumeDB objects, read from 
    the Oracle database ( see constructor), and uses it to provide 
    the volume of each LAr cell ( public method CellVolume )

    Author : V. Tsulaia  &  C. Adam
    may 2005

*/

#include "LArDetDescr/LArCellVolumeDB.h" 

class LArCellVolumes
{
public:

  // Constructors:
  LArCellVolumes(const IRDBRecordset* cellVolRec);

  virtual ~LArCellVolumes();

  double CellVolume (Identifier cell_id);

  virtual void print();

private:
  const CaloCell_ID* m_calocell_id ;

  LArCellVolumeDB m_cellVolume;  
};

 
#endif
