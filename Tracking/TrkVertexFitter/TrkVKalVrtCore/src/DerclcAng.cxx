/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <iostream>
#include <math.h>
#include <algorithm>

namespace Trk {


//
//   Angular constraints calculation in new data model.
//
//      cnstV and cnstP values are used!!!
//-----------------------------------------------

void calcPhiConstraint( VKPhiConstraint * cnst)
{
    VKConstraintBase * base_cnst = (VKConstraintBase*) cnst;
    VKVertex * vk=cnst->getOriginVertex();
    int NTRK = vk->TrackList.size();
    int i,j;
    double Scale=1.;   // Scaling for better precision VK 28.03.2011 Wrong for error matrix!!! Should always be 1.!!!
 
    double diff=0., aa=0;
    std::vector<double> deriv(NTRK,0);
    for(i=0; i<NTRK-1; i++){
       for(j=i+1; j<NTRK; j++){  
          diff  = vk->TrackList[i]->cnstP[1] - vk->TrackList[j]->cnstP[1];
          while(diff >  M_PI) diff -= 2.*M_PI;
          while(diff < -M_PI) diff += 2.*M_PI;
          aa       += diff*Scale;
	  deriv[i] +=      Scale;
	  deriv[j] -=      Scale;
       }
    }

    int numCNST=0;   //constraint number. Single constraint in this case
    base_cnst->h0t[numCNST].X = 0.;
    base_cnst->h0t[numCNST].Y = 0.;
    base_cnst->h0t[numCNST].Z = 0.;
    for(i=0; i<NTRK; i++){
       base_cnst->f0t[i][numCNST].X = 0.;
       base_cnst->f0t[i][numCNST].Y = deriv[i];
       base_cnst->f0t[i][numCNST].Z = 0.;
    }
    base_cnst->aa[numCNST]=aa;
//std::cout.precision(11);
//std::cout<<" new delta Phi="<<base_cnst->aa[numCNST]<<'\n';
//std::cout<<base_cnst->f0t[0][numCNST].X<<", "<<base_cnst->f0t[0][numCNST].Y<<", "
//         <<base_cnst->f0t[0][numCNST].Z<<'\n';
//std::cout<<base_cnst->f0t[1][numCNST].X<<", "<<base_cnst->f0t[1][numCNST].Y<<", "
//         <<base_cnst->f0t[1][numCNST].Z<<'\n';
}

void calcThetaConstraint( VKThetaConstraint * cnst)
{
    VKConstraintBase * base_cnst = (VKConstraintBase*) cnst;
    VKVertex * vk=cnst->getOriginVertex();
    int NTRK = vk->TrackList.size();
    int i,j;
    double Scale=1.;   // Scaling for better precision  VK 28.03.2011 Wrong for error matrix!!! Should always be 1.!!!
 
    double diff=0., der=0., aa=0;
    std::vector<double> deriv(NTRK,0);
    for(i=0; i<NTRK-1; i++){
       for(j=i+1; j<NTRK; j++){  
          diff = sin(vk->TrackList[i]->cnstP[0] - vk->TrackList[j]->cnstP[0])*Scale;
          der  =                                                              Scale;
          aa +=  diff;
	  deriv[i] += der;
	  deriv[j] -= der;
       }
    }

    int numCNST=0;   //constraint number. Single constraint in this case
    base_cnst->h0t[numCNST].X = 0.;
    base_cnst->h0t[numCNST].Y = 0.;
    base_cnst->h0t[numCNST].Z = 0.;
    for(i=0; i<NTRK; i++){
       base_cnst->f0t[i][numCNST].X = deriv[i];
       base_cnst->f0t[i][numCNST].Y = 0.;
       base_cnst->f0t[i][numCNST].Z = 0.;
    }
    base_cnst->aa[numCNST]=aa;
//std::cout.precision(11);
//std::cout<<" new delta Theta="<<base_cnst->aa[numCNST]<<'\n';
//std::cout<<base_cnst->f0t[0][numCNST].X<<", "<<base_cnst->f0t[0][numCNST].Y<<", "
//         <<base_cnst->f0t[0][numCNST].Z<<'\n';
//std::cout<<base_cnst->f0t[1][numCNST].X<<", "<<base_cnst->f0t[1][numCNST].Y<<", "
//         <<base_cnst->f0t[1][numCNST].Z<<'\n';
}

void calcPlaneConstraint( VKPlaneConstraint * cnst)
{
    VKConstraintBase * base_cnst = (VKConstraintBase*) cnst;
    VKVertex * vk=cnst->getOriginVertex();
    int NTRK = vk->TrackList.size();
    double curV[3] = {vk->refIterV[0]+vk->cnstV[0], vk->refIterV[1]+vk->cnstV[1],vk->refIterV[2]+vk->cnstV[2]};
 
 
    int numCNST=0;   //constraint number. Single constraint in this case
    base_cnst->h0t[numCNST].X = cnst->getA();
    base_cnst->h0t[numCNST].Y = cnst->getB();
    base_cnst->h0t[numCNST].Z = cnst->getC();
    for(int i=0; i<NTRK; i++){
       base_cnst->f0t[i][numCNST].X = 0.;
       base_cnst->f0t[i][numCNST].Y = 0.;
       base_cnst->f0t[i][numCNST].Z = 0.;
    }
    base_cnst->aa[numCNST]=cnst->getA()*curV[0]+cnst->getB()*curV[1]+cnst->getC()*curV[2] - cnst->getD() ;
//std::cout.precision(11);
//std::cout<<" new plane="<<base_cnst->aa[numCNST]<<'\n';
}

} /* End of namespace */

