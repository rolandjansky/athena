/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSSTAUFCN_H
#define MUGIRLNSSTAUFCN_H

#include "MuGirlStau/minim.h"
#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"

namespace MuGirlNS
{
    class StauTool;

    class StauFcn: public IObjective
    {
    public:
        StauFcn(StauTool* pStauTool, MsgStream& log);
        virtual ~StauFcn();
        virtual double function(double a, StauTechnology eTech);
        bool selectRange(double& min, double& max);
        void singleBeta(double beta);

    private:
        StauTool* m_pStau;
        MsgStream& m_log;

    };
} // end of namespace MuGirlNS

#endif //MUGIRLNSSTAUFCN_H
