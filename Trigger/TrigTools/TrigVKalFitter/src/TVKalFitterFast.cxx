/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigVKalFitter/TrigVKalFitter.h"
//-------------------------------------------------
//
#include<iostream>
#include<list>
namespace Trk {
 extern void vkvfast_( double* , double* , double* , double*);
}
//
//__________________________________________________________________________

  StatusCode TrigVKalFitter::VKalVrtFitFast(const std::vector<const  TrigInDetTrack*>& InpTrk,
                                            Amg::Vector3D& Vertex) {
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::list<double> xx,yy,zz;
    Vertex<<0.,0.,0.;
//
//------  extract information about selected tracks
//
    long int ntrk=0; 
    StatusCode sc = CvtTrigTrack(InpTrk,ntrk);
    if(sc.isFailure()) return StatusCode::FAILURE; 
    if(ntrk<1)         return StatusCode::FAILURE;
    if(ntrk==2){	 
    Trk::vkvfast_(&m_apar[0][0],&m_apar[1][0],&m_BMAG,out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
            Trk::vkvfast_(&m_apar[i][0],&m_apar[j][0],&m_BMAG,out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	xx.sort(); yy.sort(); zz.sort();

	std::list<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex<< out[0], out[1], out[2];


    return StatusCode::SUCCESS;
  }
