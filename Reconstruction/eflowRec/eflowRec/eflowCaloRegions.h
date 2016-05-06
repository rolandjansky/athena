/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALO_REGIONS_H
#define CALO_REGIONS_H

/********************************************************************

NAME:     efloCaloRegions.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  03 May, 2006

********************************************************************/

#include "CaloIdentifier/CaloCell_ID.h"

class eflowCalo
{
 public:
	
  enum LAYER {
    EMB1=0, EMB2, EMB3,
    EME1, EME2, EME3,
    HEC1, HEC2, HEC3, HEC4,
    Tile1, Tile2, Tile3,
    Unknown=999
  };
  
  static const int nRegions = Tile3 + 1;

  static const std::string& name(LAYER layer);
  static LAYER translateSampl(CaloCell_ID::CaloSample sampl);
  
 private:

  static const std::string m_layerNames[];
  static const LAYER m_samplCaloMap[];

};

typedef eflowCalo::LAYER eflowCaloENUM;


class eflowFirstIntRegions
{
 public:

  enum J1STLAYER {
    EMB1=0, EMB2, EMB3,
    EME1, EME2, EME3,
    HEC,
    Tile,
    Unknown=999
  };
  
  static const int nRegions = Tile + 1;

  static const std::string& name(J1STLAYER layer);
  static J1STLAYER translateCalo(eflowCaloENUM layer);
  
 private:
  
  static const std::string m_layerNames[];
  static const J1STLAYER  m_caloFirstIntMap[];

};

typedef eflowFirstIntRegions::J1STLAYER  eflowFirstIntENUM;



#endif
