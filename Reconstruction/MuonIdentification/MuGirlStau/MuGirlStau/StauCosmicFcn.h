/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STAUCOSMICFCN_H_
#define STAUCOSMICFCN_H_

#include "MuGirlStau/minim.h"
#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"

namespace MuGirlNS
{
    class StauTool;

    class StauCosmicFcn: public IObjective
    {
    public:
        StauCosmicFcn(StauTool* pStauTool, MsgStream& log, double beta = 1.0);
        virtual ~StauCosmicFcn();
        virtual double function(double a, StauTechnology eTech);
        bool selectRange(double& min, double& max);
        void singleTTrack(double tTrack);

    private:
        StauTool* m_pStau;
        MsgStream& m_log;
        double m_beta;

    };

} // end of namespace MuGirlNS

#endif /*STAUCOSMICFCN_H_*/
