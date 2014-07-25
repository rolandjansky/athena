/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIVOTDATA_H
#define PIVOTDATA_H

#include <iostream>
#include <list>
#include "MuonCablingTools/ShowRequest.h"
#include "MuonCablingTools/RPCdef.h"
#include "MuonCablingTools/dbline.h"
#include "MuonRPC_Cabling/EtaCMA.h"
#include "MuonRPC_Cabling/EvenPhiCMA.h"
#include "MuonRPC_Cabling/OddPhiCMA.h"


namespace MuonRPC_Cabling {

class CMApivotdata : BaseObject
{
    private:
    typedef std::list < EtaCMA > ETAlist;
    typedef std::list < EvenPhiCMA > EvenPHIlist;
    typedef std::list < OddPhiCMA >  OddPHIlist;

    bool m_fail;

    int m_number;
    int m_station;
    int m_type;
    std::string m_layout;

    int m_eta_index;
    int m_phi_index;
    int m_PAD_index;
    int m_Ixx_index;
    int m_start_ch;
    int m_start_st;
    int m_stop_ch;
    int m_stop_st;
    int m_pivot_station;
    int m_lowPt_station;
    int m_highPt_station;
    ViewType m_view;
    CMAcoverage m_coverage;
    std::string m_covtag;

    ETAlist etaCMA;
    EvenPHIlist evenphiCMA;
    OddPHIlist oddphiCMA;

    void reset_data(void);
    bool get_data(DBline&);
    bool confirm_data(ViewType);

    public:
    CMApivotdata();
    CMApivotdata(DBline&,int,std::string);
    ~CMApivotdata();

    EtaCMA* give_eta_cma(void);
    EvenPhiCMA* give_evenphi_cma(void);
    OddPhiCMA*  give_oddphi_cma(void);

    void Print(std::ostream&,bool) const;
};

template <class X> X& operator<< (X& stream, CMApivotdata& data)
{
    data.Print(stream,false);
    return stream;
}

}
#endif
