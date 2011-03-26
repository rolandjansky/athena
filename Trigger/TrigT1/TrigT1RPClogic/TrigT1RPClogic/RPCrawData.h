/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[500000];
    for (int i=0;i<500000;++i) buffer[i] = '\0';
    __osstream display(buffer,500000);
#endif

    data.PrintElement(display,data.name(),false);
    stream << display.str();
    return stream;
}

#endif
