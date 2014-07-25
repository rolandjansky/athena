/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CABLINGDATA_H
#define CABLINGDATA_H

#include <iostream>
#include <list>
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/RPCdef.h"
#include "MuonCablingTools/dbline.h"
#include "MuonRPC_Cabling/EtaCMA.h"
#include "MuonRPC_Cabling/EvenPhiCMA.h"
#include "MuonRPC_Cabling/OddPhiCMA.h"

namespace MuonRPC_Cabling {

class CMAcablingdata : public BaseObject
{
    private:
    typedef std::list < EtaCMA > ETAlist;

    bool m_fail;

    int m_number;
    int m_station;
    int m_type;

    int m_eta_index;
    int m_phi_index;

    int m_lowPt_start_co;
    int m_lowPt_stop_co;
    int m_lowPt_number_co;

    int m_highPt_start_co;
    int m_highPt_stop_co;
    int m_highPt_number_co;

    ViewType    m_view;
    CMAcoverage m_coverage;
    std::string m_covtag;

    ETAlist etaCMA;

    void reset_data(void);
    bool get_data(DBline&);
    bool confirm_data(ViewType);

    public:
    CMAcablingdata();
    CMAcablingdata(DBline&,int);
    ~CMAcablingdata();

    EtaCMA* give_eta_cma(void);

    void Print(std::ostream&,bool) const;
};

template <class X> X& operator<< (X& stream, CMAcablingdata& data)
{
    data.Print(stream,false);
    return stream;
}


}
#endif
