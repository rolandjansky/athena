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
    int nSubtRegions = eflowFirstIntRegions::nRegions;
    m_FirstIntParameters.resize(nSubtRegions);
    for (int i = 0; i < nSubtRegions; i++) {
      m_FirstIntParameters[i] = std::make_unique<eflowFirstIntParameters>();
    }
  }

  ~eflowParameters() {}

  const eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) const {
    return (eflowFirstIntRegions::Unknown != j1st) ? m_FirstIntParameters[j1st].get() : nullptr;
  }
  eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) {
    return (eflowFirstIntRegions::Unknown != j1st) ? m_FirstIntParameters[j1st].get() : nullptr;
  }

  
 private:
  std::vector<std::unique_ptr<eflowFirstIntParameters> > m_FirstIntParameters;
};


class eflowEEtaBinnedParameters :  public eflowEEtaBinBase {
 public:

  eflowEEtaBinnedParameters()  {}
  ~eflowEEtaBinnedParameters();

  void initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta = true);

  /* for eflowTauTool */
  bool getOrdering(eflowRingSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const;

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
  std::vector< std::vector<std::unique_ptr<eflowParameters> > > m_bins;
  
};
#endif
