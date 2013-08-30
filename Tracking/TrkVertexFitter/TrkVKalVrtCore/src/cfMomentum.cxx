/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include <math.h>
#include <iostream>

namespace Trk {

extern VKalVrtBMag vkalvrtbmag;

void vkPerigeeToP( double *perig3, double *pp, double BMAG)
{
    double const__ =BMAG  * vkalMagCnvCst;
    double phiv = perig3[1];
    double pt   = const__ / fabs(perig3[2]);
    pp[0] = pt * cos(phiv);
    pp[1] = pt * sin(phiv);
    pp[2] = pt / tan(perig3[0]);
} 



std::vector<double> getFitParticleMom( VKTrack * trk)
{
    std::vector<double> p(4);
    double magConst =vkalvrtbmag.bmag  * vkalMagCnvCst;

    double cth = 1. / tan( trk->fitP[0]);
    double phi      = trk->fitP[1];
    double pt       = magConst/ fabs( trk->fitP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}
std::vector<double> getIniParticleMom( VKTrack * trk)
{
    std::vector<double> p(4);
    double magConst =vkalvrtbmag.bmag  * vkalMagCnvCst;

    double cth = 1. / tan( trk->iniP[0]);
    double phi      =      trk->iniP[1];
    double pt       = magConst/ fabs( trk->iniP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}
std::vector<double> getCnstParticleMom( VKTrack * trk)
{
    std::vector<double> p(4);
    double magConst =vkalvrtbmag.bmag  * vkalMagCnvCst;

    double cth = 1. / tan( trk->cnstP[0]);
    double phi      =      trk->cnstP[1];
    double pt       = magConst/ fabs( trk->cnstP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}

}
