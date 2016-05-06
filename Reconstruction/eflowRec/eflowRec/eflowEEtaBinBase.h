/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef _INC_BASE_INTERPOLATED_E_ETA_BIN_SYSTEM_
#define _INC_BASE_INTERPOLATED_E_ETA_BIN_SYSTEM_

/********************************************************************

NAME:     eflowEEtaBinBase.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  17th May, 2006

********************************************************************/

#include <vector>

class eflowParameters;

class eflowEEtaBinBase {
 public:

  enum EnergyInterpolationMode { LIN, LOG };

  eflowEEtaBinBase() :  m_mode(LOG)  { m_useAbsEta = true; }
  virtual ~eflowEEtaBinBase();
  
  bool binExists(double e, double eta) const  { return (getEBinIndex(e) >= 0 && getEtaBinIndex(eta) >= 0); }

  static double getErrorReturnValue()  {return m_errorReturnValue;}

 protected:

  int getNumEBins() const { return m_eBinBounds.size(); }
  int getNumEtaBins() const { return m_etaBinBounds.size(); }

  int getEBinIndex(double e) const;
  int getEtaBinIndex(double eta) const;
  int getBinIndex(double x, const std::vector<double>& binBounds) const;

  EnergyInterpolationMode m_mode;

  static const double m_errorReturnValue;

  bool m_useAbsEta;
  std::vector<double> m_eBinBounds;
  std::vector<double> m_etaBinBounds;
};
#endif
