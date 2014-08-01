/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef _INC_BASE_INTERPOLATED_E_ETA_BIN_SYSTEM_
#define _INC_BASE_INTERPOLATED_E_ETA_BIN_SYSTEM_

/********************************************************************

NAME:     eflowBaseInterpolatedEEtaBinSystem.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  17th May, 2006

********************************************************************/

#include "eflowRec/eflowBaseEEtaBinSystem.h"

class vector;

class eflowBaseInterpolatedEtaBin :  public eflowBaseEtaBin
{
 public:

  eflowBaseInterpolatedEtaBin(double eMin, double eMax, double etaMin, double etaMax) :  eflowBaseEtaBin::eflowBaseEtaBin(eMin, eMax, etaMin, etaMax)  {}
  eflowBaseInterpolatedEtaBin()  {}
  virtual ~eflowBaseInterpolatedEtaBin()  {};

  virtual void setToUndefined() = 0;
};




class eflowBaseInterpolatedEEtaBinSystem :  public eflowBaseEEtaBinSystem
{
 public:

  enum EnergyInterpolationMode { LIN, LOG };

  eflowBaseInterpolatedEEtaBinSystem(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, EnergyInterpolationMode mode = LOG, bool useAbsEta = true);
  eflowBaseInterpolatedEEtaBinSystem(const std::vector<double>& eBinBounds, int nEtaBins, double etaMin, double etaMax, EnergyInterpolationMode mode = LOG, bool useAbsEta = true);
  eflowBaseInterpolatedEEtaBinSystem() :  m_mode(LOG)  {}
  virtual ~eflowBaseInterpolatedEEtaBinSystem()  {}
  
  bool getBin(eflowBaseInterpolatedEtaBin& meanBin, double e, double eta) const;  // builds interpolated bin

  int getNumEBins() const    {return m_eBinBounds.size();}

 protected:

  virtual eflowBaseEtaBin* makeNewEtaBin(int eBinIndex, int etaBinIndex) const = 0;

  virtual void getWeightedMeanBin(eflowBaseInterpolatedEtaBin& meanBin, const eflowBaseEtaBin* bin1, const eflowBaseEtaBin* bin2, double w1) const = 0;
  // for usual purposes, only need to implement for data specific to the derived class. GetBin will take care of bin bounds.

  void getInterpolation(const eflowBaseEtaBin** bin1, const eflowBaseEtaBin** bin2, double& w1, double e, double eta) const;

  int getEBinIndex(double e) const;

  EnergyInterpolationMode m_mode;
};



#endif
