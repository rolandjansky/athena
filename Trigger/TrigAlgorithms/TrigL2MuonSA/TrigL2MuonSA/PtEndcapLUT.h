/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUT_H
#define TRIGL2MUONSA_PTENDCAPLUT_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

#include <map>
#include <cstring>

namespace TrigL2MuonSA {

  class PtEndcapLUT
  {
  public:
    enum DataType { INVALID, ALPHAPOL2, BETAPOL2, INVRADIUSPOL2 };
    
    PtEndcapLUT(MsgStream* msg);
    ~PtEndcapLUT();

    StatusCode readLUT(std::string lut_fileName);

    MsgStream*  m_msg;
    inline MsgStream& msg() const { return *m_msg; }

    double alpha(double z1, double r1, double z2, double r2) const;
    double radius(double z1, double r1, double s1, double z2, double r2, double s2, double deltar) const;
    double lookup(int side, int charge, DataType type, int sector, int iEta, int iPhi, double value) const;

private:
    enum sizes { ETAS = 30, PHIS = 12, PHIS24 = 24 };

    struct KeyType
    {
      int      m_side;    /**< 0 = -, 1 = + */
      int      m_charge;  /**< 0 = -, 1 = + */
      DataType m_type;
      int      m_sector;
    KeyType(int side, int charge, DataType type, int sector) :
      m_side(side), m_charge(charge), m_type(type), m_sector(sector) {}
      bool operator<(const KeyType& other) const;
      std::string toString() const;
    };
    
    struct TableType
    {
      double m_xcepts[ETAS][PHIS24];
      double m_slopes[ETAS][PHIS24];
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
