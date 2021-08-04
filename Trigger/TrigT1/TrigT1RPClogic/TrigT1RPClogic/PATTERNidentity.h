/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATTERNIDENTITY_H
#define PATTERNIDENTITY_H

#include "RPC_CondCabling/CMAparameters.h"

#include <sstream>

class PATTERNidentity
{
    private:
    static const char s_inputTag[4][9];

    CMAinput m_input_id;
    int      m_layer_id;

    public:
    PATTERNidentity(CMAinput,int);
    PATTERNidentity(const PATTERNidentity&);

    PATTERNidentity operator=(const PATTERNidentity&);

    bool operator ==(const PATTERNidentity&) const;
    bool operator !=(const PATTERNidentity&) const;
    bool operator < (const PATTERNidentity&) const;

    CMAinput input_id(void) const {return m_input_id;}
    int layer_id(void)      const {return m_layer_id;}

    void Print(std::ostream&,bool) const;
};


template <class X> X& operator<<(X& stream,const PATTERNidentity& data)
{
    std::ostringstream display;
    data.Print(display,false);
    stream << display.str();
    return stream;
}

#endif
