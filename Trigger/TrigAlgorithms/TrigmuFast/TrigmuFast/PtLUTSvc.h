/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUFAST_PTLUTSVC_H
#define TRIGMUFAST_PTLUTSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"

#include <map>
#include <cstring>

//class ISvcLocator;

//static const InterfaceID IID_IPtLUTSvc("PtLUTSvc", 1, 0);


class PtLUTSvc : public Service, virtual public IInterface
{
public:
    enum DataType { INVALID, ALPHA, BETA, ALPHAPOL2, BETAPOL2, RADIUS, DELTAR };

    PtLUTSvc(const std::string& name, ISvcLocator* sl);
    virtual ~PtLUTSvc() {}

    static const InterfaceID& interfaceID() { 
        static const InterfaceID _IID(1498, 0 , 0);
        return _IID;
    }

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    double alpha(double z1, double r1, double z2, double r2) const;
    double radius(double z1, double r1, double s1, double z2, double r2, double s2, double deltar) const;
    double lookup(int side, int charge, DataType type, int iEta, int iPhi, double value) const;

private:
    enum sizes { ETAS = 30, PHIS = 12 };
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
        double m_xcepts[ETAS][PHIS];
        double m_slopes[ETAS][PHIS];
        TableType()
        {
            memset(m_xcepts, 0, sizeof(m_xcepts));
            memset(m_slopes, 0, sizeof(m_slopes));
        }
    };
    typedef std::map<KeyType, TableType*> TableMap;
    static DataType s2dt(const char* type);
    static const char* dt2s(DataType type);
    
    StringProperty    m_fileName;
    TableMap          m_tables;
    mutable MsgStream m_log;
};

#endif //TRIGMUFAST_PTLUTSVC_H
