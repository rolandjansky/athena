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


class eflowParamEtaBin :  public eflowBaseInterpolatedEtaBin
{
 public:

  eflowParamEtaBin(const eflowBinnedParameters* parentSystem, double eMin, double eMax, double etaMin, double etaMax);
  eflowParamEtaBin();
  ~eflowParamEtaBin();

  bool getOrdering(eflowCellSubtractionManager& subtMan, eflowFirstIntENUM j1st) const;

  void setToUndefined();
  void setParent(const eflowBinnedParameters* parentSystem)        {m_parentSystem = parentSystem;}
  void setRingThickness(eflowCaloENUM layer, double thickness)     {if (eflowCalo::Unknown != layer) m_ringThicknesses[layer] = thickness;}
  void setRingThicknesses(const std::vector<double>& thicknesses)  {m_ringThicknesses = thicknesses;}

  double getRingThickness(eflowCaloENUM layer) const   {return (eflowCalo::Unknown != layer) ? m_ringThicknesses[layer] : eflowBaseEEtaBinSystem::getErrorReturnValue();}
  const std::vector<double>& getRingThicknesses() const  {return m_ringThicknesses;}

  const eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st) const;
  eflowFirstIntParameters* getFirstIntBin(eflowFirstIntENUM j1st); 

 private:
  
  const eflowBinnedParameters* m_parentSystem;
  std::vector<double> m_ringThicknesses;
  std::vector<eflowFirstIntParameters*> m_firstIntBins;
};


class eflowBinnedParameters :  public eflowBaseInterpolatedEEtaBinSystem
{
 public:

  eflowBinnedParameters(double rMax, double weightRange) :  m_rMax(rMax), m_weightRange(weightRange)  {}

  eflowBinnedParameters()  {}
  ~eflowBinnedParameters()  {}

  bool getOrdering(eflowCellSubtractionManager& subtMan, double e, double eta, eflowFirstIntENUM j1st) const;

  double getRMax() const         {return m_rMax;}
  double getWeightRange() const  {return m_weightRange;}

  void setRingThickness(int energyBin, int etaBin, eflowCaloENUM ringNo, double thickness);
  void setFudgeMean(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeMean);
  void setFudgeStdDev(int energyBin, int etaBin, eflowFirstIntENUM j1st, double fudgeStdDev);
  void setShapeParam(int energyBin, int etaBin, eflowFirstIntENUM j1st, eflowCaloENUM layer, int paramNumber, double shapeParam);
  void setShapeParam(int energyBin, int etaBin,  eflowFirstIntENUM j1st, eflowCaloENUM layer, std::vector<double> shapeParams);

  void access();

  static int nShapeParams();


 private:

  eflowBaseEtaBin* makeNewEtaBin(int eBinIndex, int etaBinIndex) const;
  void getWeightedMeanBin(eflowBaseInterpolatedEtaBin& meanBin, const eflowBaseEtaBin* bin1, const eflowBaseEtaBin* bin2, double w1) const;
  void getWeightedMeanBin(eflowFirstIntParameters& meanBin, const eflowFirstIntParameters* bin1, const eflowFirstIntParameters* bin2, double w1) const;

  static const int m_nShapeParams;

  double m_rMax;
  double m_weightRange;

};
#endif
