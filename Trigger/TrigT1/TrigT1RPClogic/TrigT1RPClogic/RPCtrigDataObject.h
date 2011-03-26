/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <iostream>
#include <string>

#include "MuonCablingTools/BaseObject.h"

#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"
#endif


class RPCtrigDataObject : public BaseObject
{
    private:
    int m_number;

    public:
    RPCtrigDataObject (int,std::string);
    RPCtrigDataObject (int,const char*);
    RPCtrigDataObject (const RPCtrigDataObject&);
    ~RPCtrigDataObject(){}

    RPCtrigDataObject operator = (const RPCtrigDataObject&);

    void set_number (int);

    int number(void)         const {return m_number;}

    void PrintElement(std::ostream&,std::string,bool) const {}
    void error_header(void);

    void Print(std::ostream&,bool) const;
};


template <class X> X& operator<<(X& stream,const RPCtrigDataObject& data)
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
