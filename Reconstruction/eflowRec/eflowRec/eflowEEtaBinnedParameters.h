/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWBINNEDPARAMETERS_H
#define EFLOWBINNEDPARAMETERS_H

/********************************************************************

NAME:     eflowEEtaBinnedParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowEEtaBinBase.h"
#include "eflowRec/eflowCaloRegions.h"

//C++ Headers
#include <vector>

class eflowRingSubtractionManager;
class eflowEEtaBinnedParameters;
#include "eflowRec/eflowFirstIntParameters.h"


class eflowParameters {
 public:

  eflowParameters() {
    m_thickness.reserve(eflowCalo::nRegions);
    for (int i = 0; i < eflowCalo::nRegions; i++) {
      m_thickness.push_back(0.0);
    }
    int nSubtRegions = eflowFirstIntRegions::nRegions;
    m_FirstIntParameters.resize(nSubtRegions);
    for (int i = 0; i < nSubtRegions; i++) {
      m_FirstIntParameters[i] = new eflowFirstIntParameters;
    }
  }

  ~eflowParameters() {
    int n = m_FirstIntParameters.size();
    for (int i = 0; i < n; i++) {
      delete m_FirstIntParameters[i];
    }
  }

  double getRingThickness(eflowCaloENUM layer) const {
    return (eflowCalo::Unknown != layer) ? m_thickness[layer] : eflowEEtaBinBase::getErrorReturnValue();
  }

  void setRingThickness(eflowCaloENUM layer, double thickness) {
    if (eflowCalo::Unknown != layer) m_thickness[layer] = thickness;
  }
  const eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) const {
    return (eflowFirstIntRegions::Unknown != j1st) ? m_FirstIntParameters[j1st] : 0;
  }
  eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) {
    return (eflowFirstIntRegions::Unknown != j1st) ? m_FirstIntParameters[j1st] : 0;
  }


 private:
  std::vector<double> m_thickness;
  std::vector<eflowFirstIntParameters*> m_FirstIntParameters;
};


class eflowEEtaBinnedParameters :  public eflowEEtaBinBase {
 public:

  eflowEEtaBinnedParameters()  {}
  ~eflowEEtaBinnedParameters();

  void initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta = true);

  /* for eflowTauTool */
  bool getOrdering(eflowRingSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const;

  /* Set data - invoked by e/p tool */
  void setRingThickness(int energyBin, int etaBin, eflowCaloENUM ringNo, double thickness) {
    if (m_bins[energyBin][etaBin]) {
      m_bins[energyBin][etaBin]->setRingThickness(ringNo, thickness);
    }
  }
  void setFudgeMean(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeMean) {
    if (m_bins[energyBin][etaBin]) {
      eflowFirstIntParameters* j1stBin = m_bins[energyBin][etaBin]->getFirstIntBin(j1st);
      if (j1stBin) j1stBin->setFudgeMean(fudgeMean);
    }
  }
  void setFudgeStdDev(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeStdDev) {
    if (m_bins[energyBin][etaBin]) {
      eflowFirstIntParameters* j1stBin = m_bins[energyBin][etaBin]->getFirstIntBin(j1st);
      if (j1stBin) j1stBin->setFudgeStdDev(fudgeStdDev);
    }
  }
  void setShapeParam(int energyBin, int etaBin, eflowFirstIntENUM j1st, eflowCaloENUM layer,
                     int paramNumber, double shapeParam) {
    if (m_bins[energyBin][etaBin]) {
      eflowFirstIntParameters* j1stBin = m_bins[energyBin][etaBin]->getFirstIntBin(j1st);
      if (j1stBin) j1stBin->setShapeParameter(layer, paramNumber, shapeParam);
    }
  }


  double getInterpolation(const eflowParameters** bin1, const eflowParameters** bin2, double e, double eta) const;
  eflowFirstIntENUM adjustLFI(double e, double eta, eflowFirstIntENUM j1st) const;

 private:

  /* For different E bin, Eta bin, different (int)paramNumber, there are different (double)shapeParam */
  std::vector< std::vector<eflowParameters*> > m_bins;

};
#endif
