/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <math.h>
#include <iostream>

namespace Trk {

extern vkalMagFld      myMagFld;

void vkPerigeeToP( double *perig3, double *pp, double BMAG)
{
    double constB =BMAG  * vkalMagCnvCst;
    double phiv = perig3[1];
    double pt   = constB / std::abs(perig3[2]);
    pp[0] = pt * cos(phiv);
    pp[1] = pt * sin(phiv);
    pp[2] = pt / tan(perig3[0]);
} 



std::array<double, 4> getFitParticleMom( VKTrack * trk, VKVertex *vk)
{
    std::array<double, 4> p;
    double fieldPos[3];
    fieldPos[0]=vk->refIterV[0]+vk->fitV[0];
    fieldPos[1]=vk->refIterV[1]+vk->fitV[1];
    fieldPos[2]=vk->refIterV[2]+vk->fitV[2];
    double magConst =myMagFld.getMagFld(fieldPos,(vk->vk_fitterControl).get())  * myMagFld.getCnvCst();

    double cth = 1. / tan( trk->fitP[0]);
    double phi      = trk->fitP[1];
    double pt       = magConst/ std::abs( trk->fitP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}
std::array<double, 4> getFitParticleMom( VKTrack * trk, double BMAG)
{
    std::array<double, 4> p;
    double magConst =BMAG  * vkalMagCnvCst;

    double cth = 1. / tan( trk->fitP[0]);
    double phi      = trk->fitP[1];
    double pt       = magConst/ std::abs( trk->fitP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}

std::array<double, 4> getIniParticleMom( VKTrack * trk, VKVertex *vk)
{
    std::array<double, 4> p;
    double magConst = myMagFld.getMagFld(vk->refIterV,(vk->vk_fitterControl).get())  * myMagFld.getCnvCst();

    double cth = 1. / tan( trk->iniP[0]);
    double phi      =      trk->iniP[1];
    double pt       = magConst/ std::abs( trk->iniP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}
std::array<double, 4> getIniParticleMom( VKTrack * trk, double BMAG)
{
    std::array<double, 4> p;
    double magConst =BMAG  * vkalMagCnvCst;

    double cth = 1. / tan( trk->iniP[0]);
    double phi      =      trk->iniP[1];
    double pt       = magConst/ std::abs( trk->iniP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}


std::array<double, 4> getCnstParticleMom( VKTrack * trk, VKVertex *vk )
{
    std::array<double, 4> p;
    double cnstPos[3];
    cnstPos[0]=vk->refIterV[0]+vk->cnstV[0];
    cnstPos[1]=vk->refIterV[1]+vk->cnstV[1];
    cnstPos[2]=vk->refIterV[2]+vk->cnstV[2];
    double magConst = myMagFld.getMagFld(cnstPos,(vk->vk_fitterControl).get())  * myMagFld.getCnvCst();

    double cth = 1. / tan( trk->cnstP[0]);
    double phi      =      trk->cnstP[1];
    double pt       = magConst/ std::abs( trk->cnstP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}
std::array<double, 4> getCnstParticleMom( VKTrack * trk, double BMAG )
{
    std::array<double, 4> p;
    double magConst =BMAG  * vkalMagCnvCst;

    double cth = 1. / tan( trk->cnstP[0]);
    double phi      =      trk->cnstP[1];
    double pt       = magConst/ std::abs( trk->cnstP[2] );
    double m   = trk->getMass();
    p[0] = pt * cos(phi);
    p[1] = pt * sin(phi);
    p[2] = pt * cth;
    p[3] = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2] + m*m );
    return p;
}

}
