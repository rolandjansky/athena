/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
//-------------------------------------------------
//
#include<iostream>
#include<vector>
#include "TrkTrack/TrackInfo.h"


namespace Trk {
 extern void vkvfast_( double* , double* , double* , double*);
 extern void vkvFastV( double* , double* , double* vRef, double dbmag, double*);
}
//
//__________________________________________________________________________
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


namespace Trk{

//-----------------------------------------------------------------------------------------
//  Standard (old) code
//



  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const Track*>& InpTrk,
                                              Amg::Vector3D& Vertex,
                                              IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
//
//  Convert particles and setup reference frame
//
    int ntrk=0; 
    StatusCode sc = CvtTrkTrack(InpTrk,ntrk,state);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,BMAG_CUR;
    state.m_fitField.getMagFld(0.,0.,0.,fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.1) BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::vector<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -state.m_refFrameX, -state.m_refFrameY, -state.m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&state.m_apar[0][0],&state.m_apar[1][0],&BMAG_CUR,out);
      Trk::vkvFastV(&state.m_apar[0][0],&state.m_apar[1][0], xyz0, BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&state.m_apar[i][0],&state.m_apar[j][0],&BMAG_CUR,out);
            Trk::vkvFastV(&state.m_apar[i][0],&state.m_apar[j][0], xyz0, BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	std::sort(xx.begin(), xx.end());
	std::sort(yy.begin(), yy.end());
	std::sort(zz.begin(), zz.end());

	std::vector<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + state.m_refFrameX;
    Vertex[1]= out[1] + state.m_refFrameY;
    Vertex[2]= out[2] + state.m_refFrameZ;


    return StatusCode::SUCCESS;
  }


  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                              Amg::Vector3D& Vertex,
                                              IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
//
//  Convert particles and setup reference frame
//
    int ntrk=0; 
    StatusCode sc = CvtTrackParticle(InpTrk,ntrk,state);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,BMAG_CUR;
    state.m_fitField.getMagFld(0.,0.,0.,fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.1) BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::vector<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -state.m_refFrameX, -state.m_refFrameY, -state.m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&state.m_apar[0][0],&state.m_apar[1][0],&BMAG_CUR,out);
      Trk::vkvFastV(&state.m_apar[0][0],&state.m_apar[1][0], xyz0, BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&state.m_apar[i][0],&state.m_apar[j][0],&BMAG_CUR,out);
            Trk::vkvFastV(&state.m_apar[i][0],&state.m_apar[j][0], xyz0, BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	std::sort(xx.begin(), xx.end());
	std::sort(yy.begin(), yy.end());
	std::sort(zz.begin(), zz.end());

	std::vector<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + state.m_refFrameX;
    Vertex[1]= out[1] + state.m_refFrameY;
    Vertex[2]= out[2] + state.m_refFrameZ;


    return StatusCode::SUCCESS;
  }



  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const TrackParameters*>& InpTrk,
                                              Amg::Vector3D& Vertex,
                                              IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
//
//  Convert particles and setup reference frame
//
    int ntrk=0; 
    StatusCode sc = CvtTrackParameters(InpTrk,ntrk,state);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,BMAG_CUR;
    state.m_fitField.getMagFld(0.,0.,0.,fx,fy,BMAG_CUR);
    if(fabs(BMAG_CUR) < 0.1) BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::vector<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -state.m_refFrameX, -state.m_refFrameY, -state.m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&state.m_apar[0][0],&state.m_apar[1][0],&BMAG_CUR,out);
      Trk::vkvFastV(&state.m_apar[0][0],&state.m_apar[1][0], xyz0, BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&state.m_apar[i][0],&state.m_apar[j][0],&BMAG_CUR,out);
            Trk::vkvFastV(&state.m_apar[i][0],&state.m_apar[j][0], xyz0, BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	std::sort(xx.begin(), xx.end());
	std::sort(yy.begin(), yy.end());
	std::sort(zz.begin(), zz.end());

	std::vector<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); ++i,++it1){}; for(i=0; i<(n/2+1);++i,++it2){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + state.m_refFrameX;
    Vertex[1]= out[1] + state.m_refFrameY;
    Vertex[2]= out[2] + state.m_refFrameZ;


    return StatusCode::SUCCESS;
  }


}

