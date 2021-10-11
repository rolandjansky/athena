/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RPCRAWDATA_H
#define RPCRAWDATA_H

#include <iostream>
#include <list>

#include "TrigT1RPClogic/RPCdata.h"
#include "TrigT1RPClogic/RPCrawDigit.h"

class RPCrawData : public RPCdata
{
    public:
    typedef std::list < RPCrawDigit > DigitList;

    private:
    DigitList m_eta_digits;
    DigitList m_phi_digits;


    public:
    RPCrawData();
    RPCrawData(const RPCrawData&);
    ~RPCrawData();

    RPCrawData operator = (const RPCrawData&);

    public:
    const RPCrawDigit* load_data(RPCrawDigit&);

    const DigitList& eta_digits(void) const {return m_eta_digits;}
    const DigitList& phi_digits(void) const {return m_phi_digits;}

    const RPCrawDigit* operator <<(RPCrawDigit&);

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


template <class X> X& operator<<(X& stream,const RPCrawData& data)
{
    std::ostringstream display;
    data.PrintElement(display,data.name(),false);
    stream << display.str();
    return stream;
}

#endif
