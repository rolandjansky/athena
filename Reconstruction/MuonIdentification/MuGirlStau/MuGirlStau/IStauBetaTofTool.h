/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISTAUBETATOFTOOL_H
#define ISTAUBETATOFTOOL_H

#include "MuonRecToolInterfaces/IMuonTofTool.h"

namespace MuGirlNS
{

    class IStauBetaTofTool: public Muon::IMuonTofTool
    {
    public:
        virtual void setBeta(double beta = 1.) = 0;
        virtual void setTTrack(double tTrack = 0) = 0;
        virtual void setTShift(double tShift = 0) = 0;
    };

}
#endif // ISTAUBETATOFTOOL_H
