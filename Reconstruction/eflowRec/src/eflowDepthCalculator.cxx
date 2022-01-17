/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowDepthCalculator.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowDepthCalculator.h"
#include "eflowRec/eflowCaloRegions.h"
#include "GaudiKernel/SystemOfUnits.h"

//C++ Headers
#include <cmath>




// maps layer index to element containing depth to end of that layer
const eflowDepthLayerENUM eflowDepthCalculator::m_layerDepthMap[eflowCalo::nRegions] = { EMB1, EMB2, EMB3, EME1, EME2, EME3, HEC1, HEC2, HEC3, HEC4, Tile1, Tile2, Tile3, FCAL0, FCAL1, FCAL2 };

// gaps, dead regions etc. in interaction lengths
const double eflowDepthCalculator::m_preEMBat0eta = 0.5793;  // fit to TDR diagram
const double eflowDepthCalculator::m_cryostatAt0eta = 0.4666;  // fit to TDR diagram
const double eflowDepthCalculator::m_LArGap = 0.4;
const double eflowDepthCalculator::m_preEMEat19eta = 0.5;  // '' '' '' '' '' 
const double eflowDepthCalculator::m_preHEC = 0.4;  //  '' '' '' '' ''
const double eflowDepthCalculator::m_preFCAL = 0.5; 
const double eflowDepthCalculator::m_tileGapAt1eta = 0.5;  // a fudge (approx fit to TDR diagram)
const double eflowDepthCalculator::m_inclusivePreTileExt = 3.9603;  // a fudge (approx fit to TDR diagram)

// no. interaction lengths per m in LAr and tile
const double eflowDepthCalculator::m_EMBlambdaPerUnitLength = 2.5189 / Gaudi::Units::meter;
const double eflowDepthCalculator::m_tileLambdaPerUnitLength = 4.9979 / Gaudi::Units::meter;

const double eflowDepthCalculator::m_fcalLambdaPerUnitLength[3] = {27.5/(0.45*Gaudi::Units::meter),91.3/(0.45*Gaudi::Units::meter),89.2/(0.45*Gaudi::Units::meter)};

// EMB geometry in (r, z)
const double eflowDepthCalculator::m_EMBlayerR[4] = {1.5*Gaudi::Units::meter, 1.586*Gaudi::Units::meter, 1.93*Gaudi::Units::meter,  1.96*Gaudi::Units::meter};  // layer boundaries
const double eflowDepthCalculator::m_EMBzMin = 0.0*Gaudi::Units::meter;
const double eflowDepthCalculator::m_EMBzMax = 3.15*Gaudi::Units::meter;

// EME geometry in (r, z)
const double eflowDepthCalculator::m_EMErMin = 0.33*Gaudi::Units::meter;  //  from calo TDR plan (prob. not very accurate)
const double eflowDepthCalculator::m_EMErMax = 1.96*Gaudi::Units::meter;  //
const double eflowDepthCalculator::m_EMElayerZ[4] = { 3.55*Gaudi::Units::meter, 3.65*Gaudi::Units::meter, 3.95*Gaudi::Units::meter, 4.08*Gaudi::Units::meter };  //  TOTAL GUESS !!!!!

// HEC geometry in (r, z)
const double eflowDepthCalculator::m_HECrMin = 0.50*Gaudi::Units::meter;  //  from calo TDR plan (prob. not very accurate)
const double eflowDepthCalculator::m_HECrMax = 1.96*Gaudi::Units::meter;  //
const double eflowDepthCalculator::m_HEClayerZ[5] = { 4.14*Gaudi::Units::meter, 4.61*Gaudi::Units::meter, 5.08*Gaudi::Units::meter, 5.55*Gaudi::Units::meter, 6.00*Gaudi::Units::meter };  //  TOTAL GUESS !!!!!

// FCAL geometry in (r, z)
const double eflowDepthCalculator::m_FCALrMin = 0.6*Gaudi::Units::meter;  //  from calo TDR plan (prob. not very accurate)
const double eflowDepthCalculator::m_FCALrMax = 0.45*Gaudi::Units::meter;  //
const double eflowDepthCalculator::m_FCALlayerZ[4] = { 4.71*Gaudi::Units::meter, (4.71+.45)*Gaudi::Units::meter, (4.71+2*.45)*Gaudi::Units::meter, (4.71+3*.45)*Gaudi::Units::meter };  //  TOTAL GUESS !!!!!

// tile barrel geometry in (r, z)
const double eflowDepthCalculator::m_tileBarLayerR[4] = {2.3*Gaudi::Units::meter, 2.6*Gaudi::Units::meter, 3.44*Gaudi::Units::meter, 3.82*Gaudi::Units::meter};
const double eflowDepthCalculator::m_tileBarZmin = 0.0*Gaudi::Units::meter;
const double eflowDepthCalculator::m_tileBarZmax = 2.82*Gaudi::Units::meter;

// tile ext. geometry in (r, z)
const double eflowDepthCalculator::m_tileExtLayerR[4] = {2.3*Gaudi::Units::meter, 2.6*Gaudi::Units::meter, 3.14*Gaudi::Units::meter, 3.82*Gaudi::Units::meter};
const double eflowDepthCalculator::m_tileExtZmin = 3.42*Gaudi::Units::meter;  // ??
const double eflowDepthCalculator::m_tileExtZmax = 6.33*Gaudi::Units::meter;

// ITC1 geometry in (r, z)
const double eflowDepthCalculator::m_itc1Zmin = 3.34*Gaudi::Units::meter;
const double eflowDepthCalculator::m_itc1Zmax = 3.42*Gaudi::Units::meter;
const double eflowDepthCalculator::m_itc1Rmin = 2.99*Gaudi::Units::meter;
const double eflowDepthCalculator::m_itc1Rmax = 3.44*Gaudi::Units::meter;

// ITC2 geometry in (r, z)
const double eflowDepthCalculator::m_itc2Zmin = 3.17*Gaudi::Units::meter;
const double eflowDepthCalculator::m_itc2Zmax = 3.42*Gaudi::Units::meter;
const double eflowDepthCalculator::m_itc2Rmin = 3.44*Gaudi::Units::meter;
const double eflowDepthCalculator::m_itc2Rmax = 3.82*Gaudi::Units::meter;


eflowDepthCalculator::eflowDepthCalculator(){

  //initialize the array
  for (int i = 0; i <= m_nDepth; i++)  m_layerDepth[i] = 0.0;

}


eflowDepthLayerENUM eflowDepthCalculator::depthIndex(eflowCaloENUM layer) 
{
  if (eflowCalo::Unknown == layer)
    return Unknown;
  else
    return m_layerDepthMap[layer];
}




// returns the length of a track at a given theta trough a box in (r, z)

double eflowDepthCalculator::lengthThroughBox(double theta, double zMin, double zMax, double rMin, double rMax) 
{
  double thetaTopLeft, thetaBottomLeft, thetaTopRight, thetaBottomRight;


  if (0.0 == zMin) {
    thetaTopLeft = M_PI / 2.0;
    thetaBottomLeft = M_PI / 2.0;
  }
  else {
    thetaTopLeft = atan(rMax / zMin);
    thetaBottomLeft = atan(rMin / zMin);
  }
  thetaBottomRight = atan(rMin / zMax);
  thetaTopRight = atan(rMax / zMax);


  if (theta > thetaTopLeft || theta <= thetaBottomRight) {

    return 0.0;
  }
  else if (theta > thetaBottomLeft) {

    if (theta <= thetaTopRight)
      return (zMax - zMin) / cos(theta);
    else
      return (rMax - zMin * tan(theta)) / sin(theta);
  }
  else {

    if (theta >= thetaTopRight)
      return (rMax - rMin) / sin(theta);
    else
      return (zMax * tan(theta) - rMin) / sin(theta);
  }
}




// returns an array containing the depth of the boundaries between all parts of the detector
// in interaction lengths

const double* eflowDepthCalculator::calcDepthArray(double eta, double filler)
{
  static const double thetaTileExtBottomRight = atan( m_tileExtLayerR[0] / m_tileExtZmax );
  static const double thetaEMBbottomRight = atan( m_EMBlayerR[0] / m_EMBzMax );
  static const double thetaEMEtopLeft = atan( m_EMErMax / m_EMElayerZ[0] );
  static const double thetaHECtopLeft = atan( m_HECrMax / m_HEClayerZ[0] );
  static const double thetaFCALtopLeft = atan( m_FCALrMax / m_FCALlayerZ[0] );
  static const double preEMBbottomRight = m_preEMBat0eta / sin(thetaEMBbottomRight);
  static const double etaEMBbottomRight = fabs( log(tan(thetaEMBbottomRight / 2)) );

  for (int i = 0; i <= m_nDepth; i++)  m_layerDepth[i] = 0.0;
  eta = fabs(eta);

  if (eta >= 4.0) {
    return nullptr;
  }
  else {
    const double theta = 2.0 * atan( exp(-eta) );
    const double sinTheta = sin(theta);

    if (theta > thetaEMBbottomRight) {
      m_layerDepth[EMB1] = m_preEMBat0eta / sinTheta;
      if (theta < thetaEMEtopLeft) m_layerDepth[EME1] = m_LArGap;
    }
    else {
      m_layerDepth[EME1] = m_preEMEat19eta;
      if (eta < 1.9) m_layerDepth[EME1] += (1.9 - eta) * (preEMBbottomRight - m_preEMEat19eta) / (1.9 - etaEMBbottomRight);
    }
  
    if (theta < thetaHECtopLeft) m_layerDepth[HEC1] = m_preHEC;
    if (theta < thetaFCALtopLeft) m_layerDepth[FCAL0] = m_preFCAL;
    if (theta > thetaTileExtBottomRight)  m_layerDepth[Tile1] = m_cryostatAt0eta / sinTheta;
  
    for (int i = 0; i < 3; i++) {
      m_layerDepth[EMB2+i] = m_EMBlambdaPerUnitLength * lengthThroughBox(theta, m_EMBzMin, m_EMBzMax, m_EMBlayerR[i], m_EMBlayerR[i+1]);
      m_layerDepth[EME2+i] = m_EMBlambdaPerUnitLength * lengthThroughBox(theta, m_EMElayerZ[i], m_EMElayerZ[i+1], m_EMErMin, m_EMErMax);
    }

    for (int i = 0; i < 4; i++)  m_layerDepth[HEC2+i] = m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_HEClayerZ[i], m_HEClayerZ[i+1], m_HECrMin, m_HECrMax);
    for (int i = 0; i < 3; i++)  m_layerDepth[FCAL0+i] = m_fcalLambdaPerUnitLength[i] * lengthThroughBox(theta, m_FCALlayerZ[i], m_FCALlayerZ[i+1], m_FCALrMin, m_FCALrMax);

    if (eta <= 0.7) {

      for (int i = 0; i < 3; i++)
	m_layerDepth[2*i+TileGap12] = m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_tileBarZmin, m_tileBarZmax, m_tileBarLayerR[i], m_tileBarLayerR[i+1]);
    }
    else if (eta < 1.0) {
    
      bool haveIncludedGap = false;
      double tileGap = m_tileGapAt1eta;
      double tileBar, tileExt, temp;
    
      for (int i = 0; i < 3; i++) {

	tileBar = m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_tileBarZmin, m_tileBarZmax, m_tileBarLayerR[i], m_tileBarLayerR[i+1]);
	tileExt = m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_tileExtZmin, m_tileExtZmax, m_tileExtLayerR[i], m_tileExtLayerR[i+1]);
      
	if (1 == i)
	  tileExt += m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_itc1Zmin, m_itc1Zmax, m_itc1Rmin, m_itc1Rmax);
	else if (2 == i)
	  tileExt += m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_itc2Zmin, m_itc2Zmax, m_itc2Rmin, m_itc2Rmax);
      
	if (tileBar > 0.0 && tileExt > 0.0) {

	  temp = tileBar + tileExt;
	
	  if (!haveIncludedGap) {
	    temp += tileGap;
	    haveIncludedGap = true;
	  }
	
	  m_layerDepth[2*i+TileGap12] = temp;
	}
	else if (tileBar == 0.0 && tileExt > 0.0) {
	
	  m_layerDepth[2*i+TileGap12] = tileExt;
	
	  if (i > 0 && !haveIncludedGap) {
	    m_layerDepth[2*i+Tile1] = tileGap;
	    haveIncludedGap = true;
	  }
	}
	else {
	
	  m_layerDepth[2*i+TileGap12] = tileBar;
	}
      }      
    }
    else {
    
      double temp = 0.0;
    
      for (int i = ORIGIN; i <= Tile1; i++)  temp += m_layerDepth[i];
      if (temp < m_inclusivePreTileExt)  m_layerDepth[Tile1] += m_inclusivePreTileExt - temp;
    
      for (int i = 0; i < 3; i++)
	m_layerDepth[2*i+TileGap12] = m_tileLambdaPerUnitLength * lengthThroughBox(theta, m_tileExtZmin, m_tileExtZmax, m_tileExtLayerR[i], m_tileExtLayerR[i+1]);
    }
  
    for (int i = 1; i <= m_nDepth; i++) {
      if (m_layerDepth[i] == 0.0) m_layerDepth[i] = filler;
      m_layerDepth[i] += m_layerDepth[i-1];
    }
  }
  
  return m_layerDepth;
}


