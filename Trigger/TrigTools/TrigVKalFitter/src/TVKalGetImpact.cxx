/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrigVKalFitter/TrigVKalFitter.h"
#include "TrigVKalFitter/VKalVrtAtlas.h"
//-------------------------------------------------
//
#include<iostream>

namespace Trk {
 extern   
  void cfimp(long int TrkID, long int  ICH, long int IFL, double* PAR, double* ERR,
              double* VRT, double* VCOV,
	      double* RIMP, double* RCOV, double*  SIGN);
 extern vkalMagFld      myMagFld;
 extern vkalPropagator  myPropagator;
}
//
//__________________________________________________________________________
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



  double TrigVKalFitter::VKalGetImpact(const TrigInDetTrack* InpTrk,const Amg::Vector3D& Vertex, const long int Charge,
                                  std::vector<double>& Impact, std::vector<double>& ImpactError){
//
//------ Variables and arrays needed for fitting kernel
//
    double VrtInp[3]={Vertex.x(),Vertex.y(),Vertex.z()}; 
    double VrtCov[6]={0.,0.,0.,0.,0.,0.};
    double RIMP[5],RCOV[3];
    double SIGNIF=0.;
    std::vector<const TrigInDetTrack*> InpTrkList;
    InpTrkList.push_back(InpTrk);
//
//--Preparation
    Impact.clear(); ImpactError.clear(); 
//
    Trk::myMagFld.setMagHandler(m_fitField);             // needed for reenterability
    if(m_PropagatorType <=1 ){                           // needed for reenterability
       Trk::myPropagator.setTypeProp(m_PropagatorType);  // needed for reenterability
    }
//
//------  extract information about selected tracks
//
    long int ntrk=0; 
    StatusCode sc = CvtTrigTrack(InpTrkList,ntrk);
    if(sc.isFailure() ) return -9999.0;         //Something is wrong in conversion
    if(ntrk != 1)       return -9999.0;         //Something is wrong in conversion
//

    long int vkCharge=Charge;
    Trk::cfimp( 0, vkCharge, 0, &m_apar[0][0], &m_awgt[0][0], &VrtInp[0], &VrtCov[0], &RIMP[0], &RCOV[0], &SIGNIF);

    Impact.push_back(RIMP[0]);
    Impact.push_back(RIMP[1]);
    Impact.push_back(RIMP[2]);
    Impact.push_back(RIMP[3]);
    Impact.push_back(RIMP[4]);
    ImpactError.push_back(RCOV[0]);
    ImpactError.push_back(RCOV[1]);
    ImpactError.push_back(RCOV[2]);
    return SIGNIF;

  }




