/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BITPATTERN_H
#define BITPATTERN_H

#include "TrigT1RPClogic/RPCtrigDataObject.h"
#include "TrigT1RPClogic/PATTERNidentity.h"
#include "TrigT1RPClogic/RPCdigit.h"

class bitPATTERN : public RPCtrigDataObject
{
    private:
    PATTERNidentity m_id;
    int m_size;
    const RPCdigit **m_digits;

    void shift_left(void);
    void shift_right(void);

    public:
    bitPATTERN(std::string,const PATTERNidentity&,int);
    bitPATTERN(const bitPATTERN&);
    ~bitPATTERN();

    bitPATTERN& operator=(const bitPATTERN&);

    void set(const RPCdigit*,int);
    void unset(int);
    void reset(void);

    bool any(void) const;
    bool test(int) const;

    bool operator<(const bitPATTERN&);

    const RPCdigit* operator[](int) const;
    const PATTERNidentity& id(void) const {return m_id;}

    int size(void)  const {return m_size;}

    bitPATTERN  operator& (const bitPATTERN&) const;
    bitPATTERN& operator&=(const bitPATTERN&);
    bitPATTERN  operator| (const bitPATTERN&) const;
    bitPATTERN& operator|=(const bitPATTERN&);
    bitPATTERN  operator^ (const bitPATTERN&) const;
    bitPATTERN& operator^=(const bitPATTERN&);
    bitPATTERN& operator<<(int);
    bitPATTERN& operator>>(int);

    void dump(uint32_t&, uint32_t&) const;

    void Print(std::ostream&,bool detail) const;
};

template <class X> X& operator<<(X& stream,const bitPATTERN& data)
{
    std::ostringstream display;
    data.Print(display,false);
    stream << display.str();
    return stream;
}

#endif
