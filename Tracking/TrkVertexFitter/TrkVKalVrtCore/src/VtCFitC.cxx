/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/ForCFT.h"
#include "TrkVKalVrtCore/TrkVKalVrtCoreBase.h"
#include <cmath>
#include <iostream>

namespace Trk {


    //extern void digx(double *, double *, double *, long int , long int );
    extern void dsinv(long int *, double *, long int , long int *);
    extern void scaleg(double *, double *, long int  ,long int );


int vtcfitc( VKVertex * vk )
{

    int ii,ic,it,jc,index;

    double tmp[3];

/* -------------------------------------------------------- */
/*         Start of constraint treatment */
/* -------------------------------------------------------- */
    long int IERR = 0;
    long int totNC=0;  //total number of constraints
    long int NTRK = vk->TrackList.size();
    if (vk->ConstraintList.empty()) {return 0;}
//
    double dxyz[3]={vk->dxyz0[0],vk->dxyz0[1],vk->dxyz0[2]}; //nonconstraint vertex shift
//
// Extruction of derivatives
//
    std::vector<std::vector< Vect3DF> > tf0t;  // derivative collectors
    std::vector< Vect3DF >              th0t;  // derivative collectors
    std::vector< double >               taa;   // derivative collectors
    std::vector< Vect3DF > tmpVec;
    for(ii=0; ii<(int)vk->ConstraintList.size();ii++){
       totNC += vk->ConstraintList[ii]->NCDim;
       for(ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
         taa.push_back(  vk->ConstraintList[ii]->aa[ic] );
         th0t.push_back( vk->ConstraintList[ii]->h0t[ic] );
         tmpVec.clear();
         for(it=0; it<(int)vk->ConstraintList[ii]->f0t.size(); it++){
	    tmpVec.push_back( vk->ConstraintList[ii]->f0t[it][ic] );
         }
	 tf0t.push_back( tmpVec );
       }
    }
    if(totNC==0)return 0;
    tmpVec.clear();
//
    std::vector< std::vector<double> > denom;
    std::vector<double> tmpDV(totNC,0.);
    for(ic=0; ic<totNC; ic++) denom.push_back( tmpDV );
//
//  Start of calc
//
    std::vector< Vect3DF > al0(totNC);
    for(ic=0;ic<totNC; ic++){ al0[ic].X=0.; al0[ic].Y=0.; al0[ic].Z=0.;}
    std::vector<double> anum(totNC,0.);
    for (ic=0; ic<totNC; ++ic) {
	for (it = 0; it<NTRK; ++it) {

/*   summation of  WBCI * F0T  into vector  AL0 */
	    al0[ic].X +=  vk->tmpArr[it]->wbci[0] * tf0t[ic][it].X 
		        + vk->tmpArr[it]->wbci[3] * tf0t[ic][it].Y 
		        + vk->tmpArr[it]->wbci[6] * tf0t[ic][it].Z;
	    al0[ic].Y +=  vk->tmpArr[it]->wbci[1] * tf0t[ic][it].X
		        + vk->tmpArr[it]->wbci[4] * tf0t[ic][it].Y 
		        + vk->tmpArr[it]->wbci[7] * tf0t[ic][it].Z;
	    al0[ic].Z +=  vk->tmpArr[it]->wbci[2] * tf0t[ic][it].X 
		        + vk->tmpArr[it]->wbci[5] * tf0t[ic][it].Y 
		        + vk->tmpArr[it]->wbci[8] * tf0t[ic][it].Z;

	    anum[ic] +=  vk->tmpArr[it]->parf0[0] * tf0t[ic][it].X
	               + vk->tmpArr[it]->parf0[1] * tf0t[ic][it].Y 
		       + vk->tmpArr[it]->parf0[2] * tf0t[ic][it].Z;

	}
	al0[ic].X -=  th0t[ic].X;
	al0[ic].Y -=  th0t[ic].Y;
	al0[ic].Z -=  th0t[ic].Z;
	anum[ic] = 2.*anum[ic] + dxyz[0] * 2. * th0t[ic].X 
	                       + dxyz[1] * 2. * th0t[ic].Y
		               + dxyz[2] * 2. * th0t[ic].Z
		               + taa[ic];
    }

//std::cout<<" vertex="<<dxyz[0]<<", "<<dxyz[1]<<", "<<dxyz[2]<<'\n';
//std::cout<<"    h0t="<<th0t[0].X<<", "<<th0t[0].Y<<", "<<th0t[0].Z<<'\n';
//std::cout<<"    f0t="<<tf0t[0][0].X<<", "<<tf0t[0][0].Y<<", "<<tf0t[0][0].Z<<'\n';
//std::cout<<"    f0t="<<tf0t[0][1].X<<", "<<tf0t[0][1].Y<<", "<<tf0t[0][1].Z<<'\n';



/*  calculation (AL0)t * VCOV * AL0 and (F0T)t * WCI * F0T */
    TWRK * t_trk;
    for (ic=0; ic<totNC; ++ic) {
	for (jc=0; jc<totNC; ++jc) {
	    denom[ic][jc] =     al0[ic].X * vk->fitVcov[0] * al0[jc].X 
	                      + al0[ic].Y * vk->fitVcov[1] * al0[jc].X 
			      + al0[ic].Z * vk->fitVcov[3] * al0[jc].X 
			      + al0[ic].X * vk->fitVcov[1] * al0[jc].Y 
			      + al0[ic].Y * vk->fitVcov[2] * al0[jc].Y 
			      + al0[ic].Z * vk->fitVcov[4] * al0[jc].Y 
			      + al0[ic].X * vk->fitVcov[3] * al0[jc].Z 
			      + al0[ic].Y * vk->fitVcov[4] * al0[jc].Z 
			      + al0[ic].Z * vk->fitVcov[5] * al0[jc].Z 
			      + al0[jc].X * vk->fitVcov[0] * al0[ic].X 
			      + al0[jc].Y * vk->fitVcov[1] * al0[ic].X 
			      + al0[jc].Z * vk->fitVcov[3] * al0[ic].X 
			      + al0[jc].X * vk->fitVcov[1] * al0[ic].Y 
			      + al0[jc].Y * vk->fitVcov[2] * al0[ic].Y 
			      + al0[jc].Z * vk->fitVcov[4] * al0[ic].Y 
			      + al0[jc].X * vk->fitVcov[3] * al0[ic].Z 
			      + al0[jc].Y * vk->fitVcov[4] * al0[ic].Z 
			      + al0[jc].Z * vk->fitVcov[5] * al0[ic].Z;

	    for (it=0; it<NTRK; ++it) {
                t_trk=vk->tmpArr[it].get();
		denom[ic][jc] +=          tf0t[ic][it].X * t_trk->wci[0] * tf0t[jc][it].X 
					+ tf0t[ic][it].Y * t_trk->wci[1] * tf0t[jc][it].X 
					+ tf0t[ic][it].Z * t_trk->wci[3] * tf0t[jc][it].X 
					+ tf0t[ic][it].X * t_trk->wci[1] * tf0t[jc][it].Y 
					+ tf0t[ic][it].Y * t_trk->wci[2] * tf0t[jc][it].Y 
					+ tf0t[ic][it].Z * t_trk->wci[4] * tf0t[jc][it].Y 
					+ tf0t[ic][it].X * t_trk->wci[3] * tf0t[jc][it].Z 
					+ tf0t[ic][it].Y * t_trk->wci[4] * tf0t[jc][it].Z 
					+ tf0t[ic][it].Z * t_trk->wci[5] * tf0t[jc][it].Z 
					+ tf0t[jc][it].X * t_trk->wci[0] * tf0t[ic][it].X 
					+ tf0t[jc][it].Y * t_trk->wci[1] * tf0t[ic][it].X 
					+ tf0t[jc][it].Z * t_trk->wci[3] * tf0t[ic][it].X 
					+ tf0t[jc][it].X * t_trk->wci[1] * tf0t[ic][it].Y 
					+ tf0t[jc][it].Y * t_trk->wci[2] * tf0t[ic][it].Y 
					+ tf0t[jc][it].Z * t_trk->wci[4] * tf0t[ic][it].Y 
					+ tf0t[jc][it].X * t_trk->wci[3] * tf0t[ic][it].Z 
					+ tf0t[jc][it].Y * t_trk->wci[4] * tf0t[ic][it].Z 
					+ tf0t[jc][it].Z * t_trk->wci[5] * tf0t[ic][it].Z;
	    }
	}
    }

/*    Solving of system DENOM(i,j)*COEF(j)=ANUM(i) */
/*            for lagrange couplings               */
/*-------------------------------------------------*/
    auto coef = std::make_unique<double[]>(totNC);
    if (totNC == 1) {
	if (denom[0][0] != 0.) {
	    coef[0] = anum[0] / denom[0][0];
	} else {
	    coef[0] = 1.e3;
	}
    } else {
        auto adenom = std::make_unique<double[]>(totNC*totNC);
//        auto work   = std::make_unique<double[]>(totNC*totNC);
//        auto eigv   = std::make_unique<double[]>(totNC*totNC);
        auto scale  = std::make_unique<double[]>(totNC);
	for (ic=0; ic<totNC; ic++) {
	    for (jc=0; jc<totNC; jc++) {
		adenom[ic*totNC + jc] = denom[ic][jc];
	    }
	}
	scaleg(adenom.get(), scale.get(),  totNC,  totNC);
	//digx(adenom, eigv, work, totNC, 0);
	//if (eigv[0] <= eigv[totNC-1] * 1.e-10) {
	//    double sdet = fabs(eigv[0]) + eigv[totNC-1] * 1.e-15;
	//    for (ic=0; ic<totNC; ++ic) { adenom[ic*totNC + ic] += sdet;}
	//}
/* -- INVERT */
	dsinv(&totNC, adenom.get(), totNC, &IERR);
	if (IERR) {
	  return IERR;
	}
	for (ic=0; ic<totNC; ++ic) {
	    coef[ic] = 0.;
	    for (jc=0; jc<totNC; ++jc) {
		index = ic*totNC + jc;
		adenom[index] = scale[ic] * adenom[index] * scale[jc];
		coef[ic] += adenom[index] * anum[jc];
	    }
	}
    }


/*  new vertex */
    dxyz[0] = 0.;
    dxyz[1] = 0.;
    dxyz[2] = 0.;
    tmpVec.resize(totNC);
    for (ic=0; ic<totNC; ++ic) {
	tmpVec[ic].X =     vk->fitVcov[0] * al0[ic].X 
	                 + vk->fitVcov[1] * al0[ic].Y 
		         + vk->fitVcov[3] * al0[ic].Z;
	tmpVec[ic].Y =     vk->fitVcov[1] * al0[ic].X 
			 + vk->fitVcov[2] * al0[ic].Y 
			 + vk->fitVcov[4] * al0[ic].Z;
	tmpVec[ic].Z =     vk->fitVcov[3] * al0[ic].X 
			 + vk->fitVcov[4] * al0[ic].Y 
			 + vk->fitVcov[5] * al0[ic].Z;
	dxyz[0] += coef[ic] * tmpVec[ic].X;
	dxyz[1] += coef[ic] * tmpVec[ic].Y;
	dxyz[2] += coef[ic] * tmpVec[ic].Z;
    }
    vk->fitV[0]  =   vk->iniV[0] + vk->dxyz0[0] + dxyz[0];
    vk->fitV[1]  =   vk->iniV[1] + vk->dxyz0[1] + dxyz[1];
    vk->fitV[2]  =   vk->iniV[2] + vk->dxyz0[2] + dxyz[2];

//std::cout <<"New vrt="<<vk->dxyz0[0]<<", "<<vk->dxyz0[1]<<", "<<vk->dxyz0[2]<<'\n';
//std::cout<<"Ncntvrt shift="<<dxyz[0]<<", "<<dxyz[1]<<", "<<dxyz[2]<<'\n';

/*  new momenta */
    for (it=0; it<NTRK; ++it) {
        t_trk=vk->tmpArr[it].get();
	tmp[0] = 0.;
	tmp[1] = 0.;
	tmp[2] = 0.;
	for (ic=0; ic<totNC; ++ic) {
	    tmp[0] += coef[ic] * ( tf0t[ic][it].X + t_trk->wb[0]*tmpVec[ic].X 
	    		 			  + t_trk->wb[1]*tmpVec[ic].Y 
						  + t_trk->wb[2]*tmpVec[ic].Z);
	    tmp[1] += coef[ic] * ( tf0t[ic][it].Y + t_trk->wb[3]*tmpVec[ic].X 
	    					  + t_trk->wb[4]*tmpVec[ic].Y 
						  + t_trk->wb[5]*tmpVec[ic].Z);
	    tmp[2] += coef[ic] * ( tf0t[ic][it].Z + t_trk->wb[6]*tmpVec[ic].X 
	    					  + t_trk->wb[7]*tmpVec[ic].Y
						  + t_trk->wb[8]*tmpVec[ic].Z);
	}

/*    calculation WCI * TMP */
        VKTrack * trk = vk->TrackList[it].get();
	trk->fitP[0] -=  t_trk->wci[0]*tmp[0] + t_trk->wci[1]*tmp[1] + t_trk->wci[3]*tmp[2];
	trk->fitP[1] -=  t_trk->wci[1]*tmp[0] + t_trk->wci[2]*tmp[1] + t_trk->wci[4]*tmp[2];
	double concor =  t_trk->wci[3]*tmp[0] + t_trk->wci[4]*tmp[1] + t_trk->wci[5]*tmp[2];
	if( (trk->fitP[2] - concor)*trk->fitP[2] < 0) { concor=trk->fitP[2]/4.; } // VK 15.12.2009 - Protection against change of sign
	trk->fitP[2] -=  concor;
//std::cout<<" Npar="<<trk->fitP[0] <<", "<<trk->fitP[1]<<", "<<trk->fitP[2]<<'\n';
   }
/* =================================================================== */
    return 0;
} 

//
//  Get sum of squared aa[ic]  for all constraints. It's needed for postfit. 
//
double getCnstValues2( VKVertex * vk )
{
    if (vk->ConstraintList.empty()) return 0.;
    double sumSQ=0.;
    for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
       for(int ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
         sumSQ +=  (vk->ConstraintList[ii]->aa[ic])*(vk->ConstraintList[ii]->aa[ic]);
       }
    }
    return sumSQ;
}


} /* End of Trk namespace*/

