/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef _INC_BASE_E_ETA_BIN_SYSTEM_
#define _INC_BASE_E_ETA_BIN_SYSTEM_

/********************************************************************

NAME:     eflowBaseEEtaBinSystem.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHOR:   R Duxfield
CREATED:  8th May, 2006

********************************************************************/

#include <vector>

class eflowBaseEtaBin
{
 public:

  eflowBaseEtaBin(double eMin, double eMax, double etaMin, double etaMax) :  m_eMin(eMin), m_eMax(eMax), m_etaMin(etaMin), m_etaMax(etaMax)  {}
    eflowBaseEtaBin() ; 
  virtual ~eflowBaseEtaBin();

  void setBinBounds(double eMin, double eMax, double etaMin, double etaMax)  {m_eMin = eMin; m_eMax = eMax; m_etaMin = etaMin; m_etaMax = etaMax;}

  double getBinEnergyMin() const      {return m_eMin;}
  double getBinEnergyMax() const      {return m_eMax;}
  double getBinEtaMin() const         {return m_etaMin;}
  double getBinEtaMax() const         {return m_etaMax;}

 protected:
  
  double m_eMin;
  double m_eMax;
  double m_etaMin;
  double m_etaMax;
};

class eflowBaseEEtaBinSystem {
 public:

  eflowBaseEEtaBinSystem(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta = true);
  eflowBaseEEtaBinSystem(const std::vector<double>& eBinBounds, int nEtaBins, double etaMin, double etaMax, bool useAbsEta = true);
  eflowBaseEEtaBinSystem();
  virtual ~eflowBaseEEtaBinSystem();

  void initialise(const std::vector<double>& eBinBounds, const std::vector<double>& etaBinBounds, bool useAbsEta = true);
  void initialise(const std::vector<double>& eBinBounds, int nEtaBins, double etaMin, double etaMax, bool useAbsEta = true);
  
  void createBins();

  bool binExists(double e, double eta) const  {return (getEBinIndex(e) >= 0 && getEtaBinIndex(eta) >= 0);}

  eflowBaseEtaBin* getBin(int eBin, int etaBin);  // gives direct access to stored bins

  virtual int getNumEBins() const   {return m_eBinBounds.size();}
  int getNumEtaBins() const  {return m_etaBinBounds.size();}

  double getEBinEnergyMin(int eBinIndex) const;
  double getEBinEnergyMax(int eBinIndex) const;

  double getEtaBinEtaMin(int etaBinIndex) const;
  double getEtaBinEtaMax(int etaBinIndex) const;

  static double getErrorReturnValue()  {return m_errorReturnValue;}

 protected:

  virtual eflowBaseEtaBin* makeNewEtaBin(int eBinIndex, int etaBinIndex) const = 0;

  virtual int getEBinIndex(double e) const;
  int getEtaBinIndex(double eta) const;
  int getBinIndex(double x, const std::vector<double>& binBounds) const;

  static const double m_errorReturnValue;

  bool m_useAbsEta;
  std::vector<double> m_eBinBounds;
  std::vector<double> m_etaBinBounds;
  std::vector< std::vector<eflowBaseEtaBin*> > m_bins;
};



#endif
