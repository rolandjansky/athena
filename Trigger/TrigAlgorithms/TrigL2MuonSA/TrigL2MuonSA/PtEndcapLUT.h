/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUT_H
#define TRIGL2MUONSA_PTENDCAPLUT_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include <map>
#include <cstring>

namespace TrigL2MuonSA {

class PtEndcapLUT: public AthAlgTool
{
  public:
    enum DataType { INVALID, ALPHAPOL2, BETAPOL2, TGCALPHAPOL2, INVRADIUSPOL2 };
    
  public:
    static const InterfaceID& interfaceID();

    PtEndcapLUT(const std::string& type, 
                const std::string& name,
                const IInterface*  parent);
    ~PtEndcapLUT(void);

    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    StatusCode readLUT(std::string lut_fileName);

    double alpha(double z1, double r1, double z2, double r2) const;
    double radius(double z1, double r1, double s1, double z2, double r2, double s2, double deltar) const;
    double lookup(int side, int charge, DataType type, int iEta, int iPhi, double value) const;

private:
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
