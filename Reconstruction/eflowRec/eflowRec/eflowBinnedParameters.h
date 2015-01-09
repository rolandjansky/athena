/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWBINNEDPARAMETERS_H
#define EFLOWBINNEDPARAMETERS_H

/********************************************************************

NAME:     eflowBinnedParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowBaseInterpolatedEEtaBinSystem.h"
#include "eflowRec/eflowCaloRegions.h"

//C++ Headers
#include <vector>

class eflowCellSubtractionManager;
class eflowBinnedParameters;
class eflowFirstIntParameters;


class eflowParamEtaBin {
 public:

  eflowParamEtaBin(const eflowBinnedParameters* parentSystem);
  eflowParamEtaBin();
  ~eflowParamEtaBin();

  double getRingThickness(eflowCaloENUM layer) const {
    return (eflowCalo::Unknown != layer) ? m_ringThicknesses[layer] : eflowBaseInterpolatedEEtaBinSystem::getErrorReturnValue();
  }

  void setRingThickness(eflowCaloENUM layer, double thickness) {
    if (eflowCalo::Unknown != layer) m_ringThicknesses[layer] = thickness;
  }
  const eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) const {
    return (eflowFirstIntRegions::Unknown != j1st) ? m_firstIntBins[j1st] : 0;
  }
  eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) {
    return (eflowFirstIntRegions::Unknown != j1st) ? m_firstIntBins[j1st] : 0;
  } 

 private:
  const eflowBinnedParameters* m_parentSystem;
  std::vector<double> m_ringThicknesses;
  std::vector<eflowFirstIntParameters*> m_firstIntBins;
};


class eflowBinnedParameters :  public eflowBaseInterpolatedEEtaBinSystem {
 public:

  eflowBinnedParameters()  {}
  ~eflowBinnedParameters();

  void initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta = true);

  bool getOrdering(eflowCellSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const;

  /* Set data - invoked by e/p tool */
  void setRingThickness(int energyBin, int etaBin, eflowCaloENUM ringNo, double thickness);
  void setFudgeMean(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeMean);
  void setFudgeStdDev(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeStdDev);
  void setShapeParam(int energyBin, int etaBin, eflowFirstIntENUM j1st, eflowCaloENUM layer, int paramNumber, double shapeParam);

  static int nShapeParams();


 private:

  void getWeightedMeanBin(eflowFirstIntParameters& meanBin, const eflowFirstIntParameters* bin1, const eflowFirstIntParameters* bin2, double w1) const;
  eflowFirstIntENUM adjustLFI(double e, double eta, eflowFirstIntENUM j1st) const;

  static const int m_nShapeParams;

};
#endif
