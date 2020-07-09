/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUT_H
#define TRIGL2MUONSA_PTENDCAPLUT_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"

#include <map>
#include <cstring>
#define ETAS1  5
#define PHIS1  6
#define PTS1   3

namespace TrigL2MuonSA {

class PtEndcapLUT: public AthAlgTool
{
  public:
  enum DataType { INVALID, ALPHAPOL2, BETAPOL2, TGCALPHAPOL2, INVRADIUSPOL2, CSCPOL2 };
    
  public:

    PtEndcapLUT(const std::string& type, 
                const std::string& name,
                const IInterface*  parent);
    ~PtEndcapLUT(void);

    StatusCode readLUT(std::string lut_fileName);
    StatusCode readLUTSigmaMean(std::string lut_mean, std::string lut_sigma);

    double alpha(double z1, double r1, double z2, double r2) const;
    double radius(double z1, double r1, double s1, double z2, double r2, double s2, double deltar) const;
    double lookup(int side, int charge, DataType type, int iEta, int iPhi, double value) const;
    double ptcombined(int iEta, int iPhi, double ApT, double BpT, double &CApT, double &CBpT) const;
private:
    double m_meana[ETAS1][PHIS1][PTS1];
    double m_meanb[ETAS1][PHIS1][PTS1];
    double m_meanc[ETAS1][PHIS1][PTS1];
    double m_sigmaa[ETAS1][PHIS1][PTS1];
    double m_sigmab[ETAS1][PHIS1][PTS1];
    double m_sigmac[ETAS1][PHIS1][PTS1];

    enum sizes { ETAS = 30, PHIS = 12, PHISEE = 192};

    struct KeyType
    {
      int      m_side;    /**< 0 = -, 1 = + */
      int      m_charge;  /**< 0 = -, 1 = + */
      DataType m_type;
    KeyType(int side, int charge, DataType type) :
      m_side(side), m_charge(charge), m_type(type) {}
      bool operator<(const KeyType& other) const;
      std::string toString() const;
    };
    
    struct TableType
    {
      double m_xcepts[ETAS][PHISEE];
      double m_slopes[ETAS][PHISEE];
      TableType()
      {
	memset(m_xcepts, 0, sizeof(m_xcepts));
	memset(m_slopes, 0, sizeof(m_slopes));
      }
    };

    typedef std::map<KeyType, TableType*> TableMap;

    static DataType s2dt(const char* type);
    static const char* dt2s(DataType type);
    
    TableMap          m_tables;

};

}

#endif
