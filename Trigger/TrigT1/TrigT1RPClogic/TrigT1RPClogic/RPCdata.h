/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RPCDATA_H
#define RPCDATA_H

#include <iostream>
#include <list>

#include "MuonCablingTools/BaseObject.h"

#include "TrigT1RPClogic/RPCdigit.h"

class RPCdata : public BaseObject
{
    public:
    typedef std::list < const RPCdigit* > digitList;

    public:
    RPCdata(std::string);
    RPCdata(const char*);
    RPCdata(const RPCdata&);
    virtual ~RPCdata();

    public:
    virtual digitList eta_digits_list(void) const = 0;
    virtual digitList phi_digits_list(void) const = 0;
    virtual int how_many(int type,
		         int sector,
		         int station,
		         int layer,
		         int chamber,
		         int strip ) const = 0;
};


#endif
