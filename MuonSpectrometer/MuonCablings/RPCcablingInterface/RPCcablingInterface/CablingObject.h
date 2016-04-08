/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVL1LOGICOBJECT_H
#define LVL1LOGICOBJECT_H

#include "MuonCablingTools/BaseObject.h"

class CablingObject : public BaseObject
{
    private:
    int m_number;
    int m_station;
    int m_sector_type;

    public:
    CablingObject (int,int,int,std::string);
    CablingObject (int,int,int,const char*);
    CablingObject (const CablingObject&);
    ~CablingObject(){}

    CablingObject& operator = (const CablingObject&);

    int number(void)         const {return m_number;}
    int station(void)        const {return m_station;}
    int sector_type(void)    const {return m_sector_type;}

    void error_header(void)  const;
    void no_connection_error(std::string,int);

    friend std::ostream& operator<< (std::ostream& stream,const CablingObject& obj);
};

#endif
