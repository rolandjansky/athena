/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATTERNIDENTITY_H
#define PATTERNIDENTITY_H

#include "RPCcablingInterface/CMAparameters.h"

// for compatibility between gcc2.95 and gcc3.2
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    #include <sstream>
    typedef std::ostringstream __osstream;
#else
    // put your gcc 2.95 specific code here
    #include <strstream>
    typedef ostrstream __osstream;
#endif

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
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[1000];
    for (int i=0;i<1000;++i) buffer[i] = '\0';
    __osstream display(buffer,1000);
#endif

    data.Print(display,false);
    stream << display.str();
    return stream;
}

#endif
