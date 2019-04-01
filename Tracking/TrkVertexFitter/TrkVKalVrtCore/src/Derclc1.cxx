/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <iostream>
#include <algorithm>

namespace Trk {


//
//   Mass constraint calculation in new data model.
//
//      cnstV and cnstP values are used!!!
//-----------------------------------------------

extern std::array<double, 4> getCnstParticleMom( VKTrack *, VKVertex * );

void  calcMassConstraint( VKMassConstraint * cnst )
{
    int it,itc;
    double ptot[4]={0.,0.,0.,0.};  
    double cth, invR, pp2, pt; 
    VKConstraintBase * base_cnst = (VKConstraintBase*) cnst;
    VKVertex * vk=cnst->getOriginVertex();
    std::vector<int> usedParticles=cnst->getUsedParticles();
    int usedNTRK = usedParticles.size();
    VKTrack * trk;
    std::vector< std::array<double, 4> > pp(usedNTRK);
    for( itc=0; itc<usedNTRK; itc++){
      it = usedParticles[itc];
      trk = vk->TrackList.at(it);
      pp[itc]=getCnstParticleMom( trk , vk);
      ptot[0] += pp[itc][0];    
      ptot[1] += pp[itc][1];    
      ptot[2] += pp[itc][2];    
      ptot[3] += pp[itc][3];    
   }
   
   double temp = 1.e-10;
   int ip=0; if( fabs(ptot[1]) > fabs(ptot[ip]) )ip=1; if( fabs(ptot[2]) > fabs(ptot[ip]) )ip=2;
   int im=0; if( fabs(ptot[1]) < fabs(ptot[im]) )im=1; if( fabs(ptot[2]) < fabs(ptot[im]) )im=2;
   int id=4; for(int i=0;i<3;i++) if(i!=ip && i!=im)id=i;
   if(id==4){
     std::cout<<"ERROR in mass constraint!!!"<<'\n';
     temp=ptot[3]*ptot[3]-ptot[0]*ptot[0]-ptot[1]*ptot[1]-ptot[2]*ptot[2];
   } else {
     temp = sqrt( (ptot[3]-ptot[ip])*(ptot[3]+ptot[ip]) );
     temp = sqrt( (  temp -ptot[id])*(  temp +ptot[id]) );
     temp =       (  temp -ptot[im])*(  temp +ptot[im]);
   }
   if(temp<1.e-10)temp=1.e-10;  //protection
   temp=sqrt(temp);             //system mass
//
//
    int numCNST=0;   //constraint number. Single constraint in this case
//
//Difference   
   base_cnst->aa[numCNST] = ( temp - cnst->getTargetMass() ) * ( temp + cnst->getTargetMass() ); 
//
//Derivatives               Here pp[][3] - particle energy, pp[][4] - squared particle mom
    for( itc=0; itc<usedNTRK; itc++){
      it = usedParticles[itc];
      trk  = vk->TrackList.at(it);
      invR = trk->cnstP[2];
      cth  = 1. / tan( trk->cnstP[0] );
      pt   = sqrt(pp[itc][0]*pp[itc][0] + pp[itc][1]*pp[itc][1]);
      pp2  =      pp[itc][0]*pp[itc][0] + pp[itc][1]*pp[itc][1] + pp[itc][2]*pp[itc][2];
      base_cnst->f0t[it][numCNST].X =  ptot[3] * (-pp2* cth / pp[itc][3]) 
			             - ptot[2] * (-pt   * (cth*cth + 1.));
      base_cnst->f0t[it][numCNST].Y = -ptot[0] * (-pp[itc][1]) 
			             - ptot[1] *   pp[itc][0];
      base_cnst->f0t[it][numCNST].Z =  ptot[3] * (-pp2/ (invR * pp[itc][3])) 
	                             - ptot[0] * (-pp[itc][0] / invR) 
			             - ptot[1] * (-pp[itc][1] / invR) 
			             - ptot[2] * (-pp[itc][2] / invR);

    }
    base_cnst->h0t[numCNST].X = 0.;
    base_cnst->h0t[numCNST].Y = 0.;
    base_cnst->h0t[numCNST].Z = 0.;
/* -- Relative normalisation. Now it is target mass (squared?) to make performance uniform */
    double Scale=0.025/(2.*cnst->getTargetMass()+1.); //28.03.2011 wrong idea for cascade. VK 06.05.2011 actually correct!
    //Scale=0.01;
    for (it = 0; it < (int)vk->TrackList.size(); ++it) {
	base_cnst->f0t[it][numCNST].X *=  Scale * 2;
	base_cnst->f0t[it][numCNST].Y *=  Scale * 2;
	base_cnst->f0t[it][numCNST].Z *=  Scale * 2;
    }
    base_cnst->aa[numCNST] *= Scale;
//std::cout.precision(11);
//std::cout<<" mass="<<temp<<", "<<cnst->getTargetMass()<<", "<<base_cnst->aa[numCNST]<<'\n';
//std::cout<<base_cnst->f0t[0][numCNST].X<<", "<<base_cnst->f0t[0][numCNST].Y<<", "
//         <<base_cnst->f0t[0][numCNST].Z<<'\n';
//std::cout<<base_cnst->f0t[1][numCNST].X<<", "<<base_cnst->f0t[1][numCNST].Y<<", "
//         <<base_cnst->f0t[1][numCNST].Z<<'\n';
}




} /* End of namespace */
