/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloRegions.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  03 May, 2006

********************************************************************/

#include "eflowRec/eflowCaloRegions.h"




/////    * * *  eflowCalo  * * *    /////


const eflowCaloENUM eflowCalo::m_samplCaloMap[CaloCell_ID::FCAL2+1] =
{
  Unknown, EMB1, EMB2, EMB3,
  Unknown, EME1, EME2, EME3,
  HEC1, HEC2, HEC3, HEC4,
  Tile1, Tile2, Tile3,
  Tile2, Tile3, Unknown,
  Tile1, Tile2, Tile3,
  Unknown, Unknown, Unknown
};

const std::string eflowCalo::m_layerNames[nRegions+1] =
{
  "EMB1", "EMB2", "EMB3",
  "EME1", "EME2", "EME3",
  "HEC1", "HEC2", "HEC3", "HEC4",
  "Tile1", "Tile2", "Tile3",
  "N/A"
};

eflowCaloENUM eflowCalo::translateSampl(CaloCell_ID::CaloSample sampl)
{
  if (CaloCell_ID::Unknown == sampl)
    return Unknown;
  else
    return m_samplCaloMap[sampl];
}

const std::string& eflowCalo::name(eflowCaloENUM layer)
{
  if (Unknown == layer)
    return m_layerNames[nRegions];
  else
    return m_layerNames[layer];
}


/////    * * *  eflowFirstIntRegions  * * *    /////


const eflowFirstIntENUM eflowFirstIntRegions::m_caloFirstIntMap[eflowCalo::Tile3+1] = 
{
  EMB1, EMB2, EMB3,
  EME1, EME2, EME3,
  HEC, HEC, HEC, HEC,
  Tile, Tile, Tile
};

const std::string eflowFirstIntRegions::m_layerNames[nRegions+1] =
{
  "EMB1", "EMB2", "EMB3",
  "EME1", "EME2", "EME3",
  "HEC",
  "Tile",
  "N/A"
};

eflowFirstIntENUM eflowFirstIntRegions::translateCalo(eflowCaloENUM layer)
{
  if (eflowCalo::Unknown == layer)
    return Unknown;
  else
    return m_caloFirstIntMap[layer];
}

const std::string& eflowFirstIntRegions::name(eflowFirstIntENUM layer)
{
  if (Unknown == layer)
    return m_layerNames[nRegions];
  else
    return m_layerNames[layer];
}
