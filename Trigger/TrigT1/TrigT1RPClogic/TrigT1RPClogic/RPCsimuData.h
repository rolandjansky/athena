/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RPCSIMUDATA_H
#define RPCSIMUDATA_H

#include <iostream>
#include <list>

#include "TrigT1RPClogic/RPCdata.h"
#include "TrigT1RPClogic/MuonSimuTrack.h"
#include "TrigT1RPClogic/RPCsimuDigit.h"

class RPCsimuData : public RPCdata
{
    public:
    typedef std::list < MuonSimuTrack > MuonList;
    typedef std::list < RPCsimuDigit > DigitList;

    private:
    MuonList m_muon_tracks;
    DigitList m_eta_digits;
    DigitList m_phi_digits;


    public:
    RPCsimuData();
    RPCsimuData(const RPCsimuData&);
    ~RPCsimuData();

    RPCsimuData operator = (const RPCsimuData&);

    public:
    void load_data(MuonSimuTrack&);
    void load_data(RPCsimuDigit&);

    const MuonList& muon_tracks(void) const {return m_muon_tracks;}
    const DigitList& eta_digits(void) const {return m_eta_digits;}
    const DigitList& phi_digits(void) const {return m_phi_digits;}

    RPCsimuData& operator <<(MuonSimuTrack&);
    RPCsimuData& operator <<(RPCsimuDigit&);

    RPCdata::digitList eta_digits_list(void) const;
    RPCdata::digitList phi_digits_list(void) const;
    virtual int how_many(int type,
		         int sector,
		         int station,
		         int layer,
		         int chamber,
		         int strip ) const;

    void PrintElement(std::ostream&,std::string,bool) const;
};


template <class X> X& operator<<(X& stream,const RPCsimuData& data)
{
    std::ostringstream display;
    data.PrintElement(display,data.name(),false);
    stream << display.str();
    return stream;
}

#endif
