/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMUDIGIT_H
#define SIMUDIGIT_H

#include <string>

#include "TrigT1RPClogic/RPCdigit.h"

class RPCrawData;

class RPCrawDigit: public RPCdigit
{ 
    private:

    const RPCrawData* m_data;

    public:
    RPCrawDigit(int,unsigned int, float [4]);
    //RPCrawDigit(int,std::list<unsigned int>,float,const RPCGeometry*);
    RPCrawDigit(const RPCrawDigit&);
    ~RPCrawDigit() {}

    RPCrawDigit operator=(const RPCrawDigit&);

    void set_data(const RPCrawData&);

    const RPCrawData* data(void) const {return m_data;} 
};

#endif
