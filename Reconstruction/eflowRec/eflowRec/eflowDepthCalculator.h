/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWDEPTHCALCULATOR_H
#define EFLOWDEPTHCALCULATOR_H

/********************************************************************

NAME:     eflowDepthCalculator.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

#include "eflowRec/eflowCaloRegions.h"

class eflowDepthCalculator {

 public:
    
  eflowDepthCalculator();
  ~eflowDepthCalculator() {}

  static int NDepth()  {return m_nDepth;}

  const double* calcDepthArray(double eta, double filler = 0.0);
  const double* getDepthArray() const  {return m_layerDepth;}

  enum DEPTHLAYER {
    ORIGIN=0, EMB1, EMB2, EMB3, EMB_back,
    EME1, EME2, EME3, EME_back,
    HEC1, HEC2, HEC3, HEC4, HEC_back,
    Tile1, TileGap12, Tile2, TileGap23, Tile3, Tile_back,
    Unknown=999
  };

  static DEPTHLAYER depthIndex(eflowCaloENUM layer);

  static const double m_preEMBat0eta;
  static const double m_cryostatAt0eta;
  static const double m_LArGap;
  static const double m_preEMEat19eta;
  static const double m_preHEC;
  static const double m_tileGapAt1eta;
  static const double m_inclusivePreTileExt;
  
  static const double m_EMBlambdaPerUnitLength;
  static const double m_tileLambdaPerUnitLength;
  
  static const double m_EMBlayerR[4];
  static const double m_EMBzMin;
  static const double m_EMBzMax;

  static const double m_EMErMin;
  static const double m_EMErMax;
  static const double m_EMElayerZ[4];

  static const double m_HECrMin;
  static const double m_HECrMax;
  static const double m_HEClayerZ[5];
  
  static const double m_tileBarLayerR[4];
  static const double m_tileBarZmin;
  static const double m_tileBarZmax;

  static const double m_tileExtLayerR[4];
  static const double m_tileExtZmin;
  static const double m_tileExtZmax;
  
  static const double m_itc1Zmin;
  static const double m_itc1Zmax;
  static const double m_itc1Rmin;
  static const double m_itc1Rmax;

  static const double m_itc2Zmin;
  static const double m_itc2Zmax;
  static const double m_itc2Rmin;
  static const double m_itc2Rmax;

 private:

  double lengthThroughBox(double theta, double zMin, double zMax, double rMin, double rMax) const;
  
  static const int m_nDepth = Tile_back;  // number of region bounds
  static const DEPTHLAYER m_layerDepthMap[eflowCalo::nRegions];

  double m_layerDepth[m_nDepth+1];
};


typedef eflowDepthCalculator::DEPTHLAYER eflowDepthLayerENUM;

#endif
