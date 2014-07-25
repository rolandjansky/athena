/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WIREDORDATA_H
#define WIREDORDATA_H

#include <list>
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/dbline.h"
#include "MuonRPC_Cabling/WiredOR.h"

namespace MuonRPC_Cabling {

class WiredORdata : BaseObject
{
    private:
    typedef std::list < WiredOR > WORlist;

    bool m_fail;

    int m_number;
    int m_station;
    int m_type;
    int m_start;
    int m_stop;

    WORlist wor;


    void reset_data(void);
    bool get_data(DBline&);
    bool confirm_boundary(void) const;

    public:
    WiredORdata();
    WiredORdata(DBline&,int);
    ~WiredORdata();

    WiredOR* give_wor(void);

    int station(void) const {return m_station;}

    void Print(std::ostream&,bool) const;
};

template <class X> X& operator<< (X& stream, WiredORdata& data)
{
    data.Print(stream,false);
    return stream;
}

}

#endif
