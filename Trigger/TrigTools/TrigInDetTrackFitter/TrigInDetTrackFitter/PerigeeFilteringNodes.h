/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __PERIGEEFILTERINGNODES_H__
#define __PERIGEEFILTERINGNODES_H__

class TrigSiSpacePoint;

class BasePerigeeFilteringNode
{
 public:
  BasePerigeeFilteringNode();
  virtual ~BasePerigeeFilteringNode(){};
  virtual double m_getChi2(double*,double*) = 0;
  virtual void m_runFilter(double*,double*);
  virtual void m_applyMultScatt(double*,double*);
  virtual void m_applyEnergyLoss(double*,double*,int);
  virtual int m_getNdof() = 0;
  virtual double m_getRho() = 0;
  void m_setType(char type)
    {
      m_nType=type;
    }
  char m_getType()
    {
      return m_nType;
    }
  void m_setLayer(long lay)
    {
      m_nLayer=lay;
    }
  long m_getLayer()
    {
      return m_nLayer;
    }
  const TrigSiSpacePoint* m_getTrigSp()
    {
      return m_pTrigSp;
    }
  bool m_isAccepted();
  void m_acceptIt();
  void m_rejectIt();
 protected:
  char m_nType,m_nStatus;
  long m_nLayer;
  double m_dChi2;
  double m_D[2][2];
  double m_B[2][5];
  double m_resid[2];
  double m_EffSigmaMS;
  double m_EffRadLength;
  double m_Rho;
  const TrigSiSpacePoint* m_pTrigSp;
};

class BarrelPerigeeFilteringNode: public BasePerigeeFilteringNode
{
 private:
  double m_Ri;
  double m_yPhi,m_sigmaPhi;
  double m_yZ,m_sigmaZ;
 public:
  BarrelPerigeeFilteringNode(const TrigSiSpacePoint*);
  ~BarrelPerigeeFilteringNode(){};
  double m_getChi2(double*,double*);
  void m_runFilter(double*,double*);
  void m_applyMultScatt(double*,double*);
  void m_applyEnergyLoss(double*,double*,int);
  double m_getEffRadLength(double*);
  int m_getNdof();
  double m_getRho()
    {
      return m_Ri;
    }  
};

class DiscPerigeeFilteringNode: public BasePerigeeFilteringNode
{
 private:
  double m_Zi;
  double m_yPhi,m_sigmaPhi;
  double m_yR,m_sigmaR;
 public:
  DiscPerigeeFilteringNode(const TrigSiSpacePoint*);
  ~DiscPerigeeFilteringNode(){};
  double m_getChi2(double*,double*);
  void m_applyMultScatt(double*,double*);
  void m_applyEnergyLoss(double*,double*,int);
  double m_getEffRadLength(double*);
  void m_runFilter(double*,double*);
  int m_getNdof();
  double m_getRho()
    {
      return m_Rho;
    }
};

#endif
