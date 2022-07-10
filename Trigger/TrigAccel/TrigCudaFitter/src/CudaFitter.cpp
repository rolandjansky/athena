#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <typeinfo>
#include "BFieldPoint.h"
#include "TrkTrackState.h"
#include "RecTrack.h"
#include "SpacePoint.h"
#include "SiCluster.h"
#include "PixelCluster.h"
#include "SCT_Cluster.h"
#include "SCT_EndCapCluster.h"
#include "TrkBaseNode.h"
#include "TrkPlanarSurface.h"
#include "DkfTrack.h"
#include "TrackFitter.h"
#include "CudaFitter.h"
#include "DataTypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <chrono>

#include <cuda_runtime.h>

/// Simple macro to run CUDA commands with
#define CUDA_CHECK( EXP )                                                  \
  do {                                                                     \
    const cudaError_t ce = EXP;                                            \
    if( ce != cudaSuccess ) {                                              \
      std::cerr << "Failed to execute: " << #EXP << std::endl;             \
      std::cerr << "Reason: " << cudaGetErrorString( ce ) << std::endl;    \
      return;                                                              \
    }                                                                      \
  } while( false )

extern "C" float runKernel(INPUT_TRACK_INFO_TYPE* d_In,
    DETECTOR_SURFACE_TYPE* d_Geo,
    OUTPUT_TRACK_INFO_TYPE* d_Out,
    HIT_INFO_TYPE* d_Hit,int Ntracks);

CudaFitter::CudaFitter(void) {

}

CudaFitter::~CudaFitter(void) {
}

void CudaFitter::fit(std::vector<const RecTrack*>& vpTracks) {

  DETECTOR_SURFACE_TYPE* pGeo = new DETECTOR_SURFACE_TYPE;
  DETECTOR_SURFACE_TYPE& geometry = *pGeo;
  OUTPUT_TRACK_INFO_TYPE* pOutput = new OUTPUT_TRACK_INFO_TYPE;
  OUTPUT_TRACK_INFO_TYPE& outputTracks = *pOutput;
  INPUT_TRACK_INFO_TYPE* pTracks = new INPUT_TRACK_INFO_TYPE;
  INPUT_TRACK_INFO_TYPE& inputTracks = *pTracks;
  HIT_INFO_TYPE* pHits = new HIT_INFO_TYPE;
  HIT_INFO_TYPE& hits=*pHits;

  INPUT_TRACK_INFO_TYPE* d_In;
  DETECTOR_SURFACE_TYPE* d_Geo;
  OUTPUT_TRACK_INFO_TYPE* d_Out;
  HIT_INFO_TYPE* d_Hit;

  cudaSetDevice(0);

  cudaDeviceSynchronize();

  auto totalStart = std::chrono::steady_clock::now();

  int nEvents = (int) vpTracks.size()/MAX_TRACK;

  std::vector<const RecTrack*>::const_iterator trIt=vpTracks.begin();

  long inDataSize=nEvents*(sizeof(INPUT_TRACK_INFO_TYPE)+sizeof(DETECTOR_SURFACE_TYPE)+sizeof(HIT_INFO_TYPE));
  long outDataSize=nEvents*sizeof(OUTPUT_TRACK_INFO_TYPE);
  printf("Input Data = %ld bytes, Output data = %ld bytes\n",inDataSize,outDataSize);

  int nFittedTracksCPU=0;
  //bool doCPU_Fitting=false;
  bool doCPU_Fitting=true;
  //printf("...allocating GPU memory.\n");

  CUDA_CHECK(cudaMalloc((void **)&d_In, sizeof(INPUT_TRACK_INFO_TYPE)));
  CUDA_CHECK(cudaMalloc((void **)&d_Geo, sizeof(DETECTOR_SURFACE_TYPE)));
  CUDA_CHECK(cudaMalloc((void **)&d_Hit, sizeof(HIT_INFO_TYPE)));
  CUDA_CHECK(cudaMalloc((void **)&d_Out, sizeof(OUTPUT_TRACK_INFO_TYPE)));

  int nFittedTracksGPU=0;
  float kernelTotal=0.0;
  float cpuTotal=0.0;

  for(int eventIdx = 0; eventIdx<nEvents;++eventIdx) {

    for(int trackIdx=0;trackIdx<MAX_TRACK;++trIt,++trackIdx) {

      inputTracks.m_parSet1[trackIdx].z=(float)(*trIt)->getParameters()[2];
      inputTracks.m_parSet1[trackIdx].w=(float)(*trIt)->getParameters()[3];
      double momentum = (sin((*trIt)->getParameters()[3])/(*trIt)->getParameters()[4]);
      inputTracks.m_parSet2[trackIdx].x=(float)momentum;
      inputTracks.m_parSet2[trackIdx].y=(float)(*trIt)->getField(0)->m_field[0];
      inputTracks.m_parSet2[trackIdx].z=(float)(*trIt)->getField(0)->m_field[1];
      inputTracks.m_parSet2[trackIdx].w=(float)(*trIt)->getField(0)->m_field[2];

      int nHits=0;
      int iSurface=0;
      int iSP=0;
      float firstSurface=true;
      double sp_coord[3];
      const std::vector<const SpacePoint*>& spv = (*trIt)->getSpacePoints();
      const SpacePoint* pSP = (*(*trIt)->getSpacePoints().begin());
      sp_coord[0]=pSP->m_x;sp_coord[1]=pSP->m_y;sp_coord[2]=pSP->m_z;

      for(std::vector<const SpacePoint*>::const_iterator spIt=spv.begin();spIt!=spv.end();++spIt) {
        iSP++;
        pSP = (*spIt);
        nHits+=(int) pSP->m_clusters.size();

        if(nHits>=MAX_HIT) {
          nHits -= (int) pSP->m_clusters.size();
          break;
        }
        for(std::vector<const SiCluster*>::const_iterator clIt = pSP->m_clusters.begin();
            clIt!=pSP->m_clusters.end();++clIt) {
          const SiCluster* pCL = (*clIt);
          TrkBaseNode* pBN = pCL->createDkfNode();
          TrkPlanarSurface* pS = pBN->getSurface();
          if(firstSurface) {
            firstSurface=false;
            double loc[3];
            pS->transformPointToLocal(sp_coord,loc);
            inputTracks.m_parSet1[trackIdx].x=(float)loc[0];
            inputTracks.m_parSet1[trackIdx].y=(float)loc[1];
          }
          int geoIdx = trackIdx + iSurface*MAX_TRACK;

          geometry.m_rowX[geoIdx].w=(float)pS->getCenter()[0];
          geometry.m_rowY[geoIdx].w=(float)pS->getCenter()[1];
          geometry.m_rowZ[geoIdx].w=(float)pS->getCenter()[2];
          geometry.m_rowX[geoIdx].x=(float)pS->getInvRotMatrix(0,0);
          geometry.m_rowX[geoIdx].y=(float)pS->getInvRotMatrix(0,1);
          geometry.m_rowX[geoIdx].z=(float)pS->getInvRotMatrix(0,2);
          geometry.m_rowY[geoIdx].x=(float)pS->getInvRotMatrix(1,0);
          geometry.m_rowY[geoIdx].y=(float)pS->getInvRotMatrix(1,1);
          geometry.m_rowY[geoIdx].z=(float)pS->getInvRotMatrix(1,2);
          geometry.m_rowZ[geoIdx].x=(float)pS->getInvRotMatrix(2,0);
          geometry.m_rowZ[geoIdx].y=(float)pS->getInvRotMatrix(2,1);
          geometry.m_rowZ[geoIdx].z=(float)pS->getInvRotMatrix(2,2);
          geometry.m_parSet[geoIdx].w=(float)0.025;
          geometry.m_parSet[geoIdx].x=(float)(*trIt)->getField(iSP)->m_field[0];
          geometry.m_parSet[geoIdx].y=(float)(*trIt)->getField(iSP)->m_field[1];
          geometry.m_parSet[geoIdx].z=(float)(*trIt)->getField(iSP)->m_field[2];

          int type = pBN->getNodeType();

          switch(type) {
            case 1 : 
              {
                const PixelCluster* pp = dynamic_cast<const PixelCluster*>(pCL);
                if(pp!=NULL) {
                  hits.m_hitType[geoIdx]=1;
                  hits.m_meas[geoIdx].x=(float)pp->m_m[0];
                  hits.m_meas[geoIdx].y=(float)pp->m_m[1];
                  hits.m_cov[geoIdx].x=(float)pp->m_cov[0][0];
                  hits.m_cov[geoIdx].y=(float)pp->m_cov[0][1];
                  hits.m_cov[geoIdx].z=(float)pp->m_cov[1][0];
                  hits.m_cov[geoIdx].w=(float)pp->m_cov[1][1];
                }
              }
              break;
            case 2 :
              {
                const SCT_Cluster* ps = dynamic_cast<const SCT_Cluster*>(pCL);
                if(ps!=NULL) {
                  hits.m_hitType[geoIdx]=2;
                  hits.m_meas[geoIdx].x=(float)ps->m_m;
                  hits.m_cov[geoIdx].x=(float)ps->m_cov;
                }
              }
              break;
            case 3 :
              {
                const SCT_EndCapCluster* pe = dynamic_cast<const SCT_EndCapCluster*>(pCL);
                if(pe!=NULL) {
                  hits.m_hitType[geoIdx]=3;
                  hits.m_meas[geoIdx].x=(float)pe->m_m;
                  hits.m_meas[geoIdx].y=(float)pe->m_R;
                  hits.m_cov[geoIdx].x=(float)pe->m_cov;
                }
              }
              break;
            default :
              std::cout<<"wrong polymorphic node type, class name = "<<(typeid(*pBN)).name()<<std::endl;
          }
          iSurface++;
          delete pBN;
          delete pS;
        }
      }
      inputTracks.m_nHits[trackIdx] = nHits;

    }

    int Ntracks=MAX_TRACK;

    if(doCPU_Fitting) {

      auto hostStart = std::chrono::steady_clock::now();

      runFixedPointSmoother(&inputTracks,&outputTracks,&geometry,&hits,Ntracks);

      auto hostStop = std::chrono::steady_clock::now();

      float hostTime = (float)std::chrono::duration_cast<std::chrono::microseconds>( hostStop - hostStart ).count() / 1000.0;;

      nFittedTracksCPU+=Ntracks;
      cpuTotal+=hostTime;
      int nBadTracks=0;    
      float avChi2 [[maybe_unused]]=0.0;
      for(int ii=0;ii<Ntracks;ii++) {
        if(outputTracks.m_status[ii]==0) nBadTracks++;
        else avChi2+=outputTracks.m_chi2[ii]/outputTracks.m_ndof[ii];
        // std::cout<<ii<<" status="<<outputTracks.m_status[ii]<<" chi2="<<outputTracks.m_chi2[ii]<<" ndof="<<outputTracks.m_ndof[ii]<<std::endl;
      }
      //printf("%d bad CPU tracks detected, chi2/ndof=%f\n",nBadTracks,avChi2/(Ntracks-nBadTracks));


      /*
         for(int ii=0;ii<Ntracks;ii++) {
         std::cout<<ii<<" chi2="<<outputTracks.m_chi2[ii]<<" ndof="<<outputTracks.m_ndof[ii]<<std::endl;
         }
         */
    } //doCPUfitting
    //printf("...copying input data to GPU mem.\n");

    CUDA_CHECK(cudaMemcpy(d_In, &inputTracks, sizeof(INPUT_TRACK_INFO_TYPE), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_Geo, &geometry, sizeof(DETECTOR_SURFACE_TYPE), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_Hit, &hits, sizeof(HIT_INFO_TYPE), cudaMemcpyHostToDevice));

    float kernelTime = 0;

    kernelTime=runKernel(d_In,d_Geo,d_Out,d_Hit,Ntracks);

    kernelTotal += kernelTime;

    CUDA_CHECK(cudaMemcpy(&outputTracks, d_Out, sizeof(OUTPUT_TRACK_INFO_TYPE), cudaMemcpyDeviceToHost));

    int nBadTracks=0;
    float avChi2=0.0;
    for(int ii=0;ii<Ntracks;ii++) {
      if(outputTracks.m_status[ii]==0) nBadTracks++;
      else avChi2+=outputTracks.m_chi2[ii]/outputTracks.m_ndof[ii];
      // std::cout<<ii<<" status="<<outputTracks.m_status[ii]<<" chi2="<<outputTracks.m_chi2[ii]<<" ndof="<<outputTracks.m_ndof[ii]<<std::endl;
    }
    printf("%d bad GPU tracks detected, chi2/ndof=%f\n",nBadTracks,avChi2/(Ntracks-nBadTracks));
    /*
       for(int ii=0;ii<Ntracks;ii++) {
       if(outputTracks.m_status[ii]==0) {
       std::cout<<ii<<" status="<<outputTracks.m_status[ii]<<" chi2="<<outputTracks.m_chi2[ii]
       <<" ndof="<<outputTracks.m_ndof[ii]<<std::endl;
       }
       }
       */
    nFittedTracksGPU+=MAX_TRACK;
    //    break;
  }

  delete pGeo;
  delete pOutput;
  delete pTracks;
  delete pHits;
  CUDA_CHECK(cudaFree(d_In));
  CUDA_CHECK(cudaFree(d_Out));
  CUDA_CHECK(cudaFree(d_Geo));
  CUDA_CHECK(cudaFree(d_Hit));

  auto totalStop = std::chrono::steady_clock::now();

  float totalTime = (float)std::chrono::duration_cast<std::chrono::microseconds>( totalStop - totalStart ).count() / 1000.0;;

  printf("Total test time: %f msecs.\n", totalTime);
  printf("GPU kernel time : %f msecs.\n",kernelTotal);
  printf("CPU time : %f msecs \n",cpuTotal);
  printf("GPU tracks fitted =%d\n",nFittedTracksGPU);
  printf("CPU tracks fitted =%d\n",nFittedTracksCPU);

  printf("Per event time: GPU=%f msecs. CPU=%f msecs\n",kernelTotal/nEvents,cpuTotal/nEvents);

  if(nFittedTracksGPU!=0) 
    printf("GPU time/track = %f microseconds\n",1000.0*kernelTotal/nFittedTracksGPU);
  if(nFittedTracksCPU!=0) 
    printf("CPU time/track = %f microseconds\n",1000.0*cpuTotal/nFittedTracksCPU);

}

void CudaFitter::runFixedPointSmoother(INPUT_TRACK_INFO_TYPE* d_In,
    OUTPUT_TRACK_INFO_TYPE* d_Out,
    DETECTOR_SURFACE_TYPE* d_Geo,
    HIT_INFO_TYPE* d_Hit, 
    int Ntracks) {
  float C=0.00002999975f;
  float minStep=30.0;

  for(int idx=0;idx<Ntracks;idx++){
    float lenCorr,sigmaMS,ms2,invSin, invSin2,lV[3],sinThetaLoc;
    float chi2=0.0;
    int ndof=-5;
    int status=1;

    EXTENDED_TRACK_STATE_TYPE ets;
    float4 initPar = (*d_In).m_parSet1[idx];
    //1. read first surface
    float m[3][3],c[3],gV[3],B0[3],gB[3],gP[3],sint,
          sinf,cost,cosf,a,b,cc,sl,CQ,J[5][5],JG[5][5];
    int hitType;
    float meas[2];
    float measCov[2][2];
    int geoIdx = idx;
    c[0]=(*d_Geo).m_rowX[geoIdx].w;c[1]=(*d_Geo).m_rowY[geoIdx].w;c[2]=(*d_Geo).m_rowZ[geoIdx].w;
    m[0][0]=(*d_Geo).m_rowX[geoIdx].x;m[0][1]=(*d_Geo).m_rowX[geoIdx].y;m[0][2]=(*d_Geo).m_rowX[geoIdx].z;
    m[1][0]=(*d_Geo).m_rowY[geoIdx].x;m[1][1]=(*d_Geo).m_rowY[geoIdx].y;m[1][2]=(*d_Geo).m_rowY[geoIdx].z;
    m[2][0]=(*d_Geo).m_rowZ[geoIdx].x;m[2][1]=(*d_Geo).m_rowZ[geoIdx].y;m[2][2]=(*d_Geo).m_rowZ[geoIdx].z;
    float g[3][3];
    g[0][0]= m[1][1]*m[2][2]-m[1][2]*m[2][1];
    g[1][0]=-m[1][0]*m[2][2]+m[1][2]*m[2][0];
    g[2][0]= m[1][0]*m[2][1]-m[1][1]*m[2][0];
    g[0][1]=-m[0][1]*m[2][2]+m[0][2]*m[2][1];
    g[1][1]= m[0][0]*m[2][2]-m[0][2]*m[2][0];
    g[2][1]=-m[0][0]*m[2][1]+m[0][1]*m[2][0];
    g[0][2]= m[0][1]*m[1][2]-m[0][2]*m[1][1];
    g[1][2]=-m[0][0]*m[1][2]+m[0][2]*m[1][0];
    g[2][2]= m[0][0]*m[1][1]-m[0][1]*m[1][0];
    ets.m_par[0]=ets.m_par[5]=initPar.x;
    ets.m_par[1]=ets.m_par[6]=initPar.y;
    ets.m_par[2]=ets.m_par[7]=initPar.z;
    ets.m_par[3]=ets.m_par[8]=initPar.w;
    initPar=(*d_In).m_parSet2[idx];
    ets.m_par[4]=ets.m_par[9]=initPar.x*1000.0f;
    B0[0]=initPar.y;B0[1]=initPar.z;B0[2]=initPar.w;
    memset(&ets.m_cov[0][0],0,sizeof(ets.m_cov));
    ets.m_cov[0][0]=ets.m_cov[5][5]=ets.m_cov[0][5]=ets.m_cov[5][0]=4.0f;
    ets.m_cov[1][1]=ets.m_cov[6][6]=ets.m_cov[1][6]=ets.m_cov[6][1]=20.0f;
    ets.m_cov[2][2]=ets.m_cov[7][7]=ets.m_cov[2][7]=ets.m_cov[7][2]=0.01f;
    ets.m_cov[3][3]=ets.m_cov[8][8]=ets.m_cov[3][8]=ets.m_cov[8][3]=0.01f;
    ets.m_cov[4][4]=ets.m_cov[9][9]=ets.m_cov[4][9]=ets.m_cov[9][4]=1.0f;
    float4 ip1=(*d_Geo).m_parSet[geoIdx];

    hitType = (*d_Hit).m_hitType[geoIdx];
    meas[0] = (*d_Hit).m_meas[geoIdx].x;
    meas[1] = (*d_Hit).m_meas[geoIdx].y;
    measCov[0][0] = (*d_Hit).m_cov[geoIdx].x;measCov[0][1] = (*d_Hit).m_cov[geoIdx].y;
    measCov[1][0] = (*d_Hit).m_cov[geoIdx].z;measCov[1][1] = (*d_Hit).m_cov[geoIdx].w;
    for(int iter = 1;iter<=(*d_In).m_nHits[idx];iter++) {
      //A1. update using previous (iter-1) hit

      bool doUpdate=true;
      float updState[10];
      float updCov[55];
      float dchi2=0.0;
      int dNdof=0;
      memset(&updState[0],0,sizeof(updState));
      memset(&updCov[0],0,sizeof(updCov));
      if(hitType==1) {//pixel
        //doUpdate=false;
        float B[10][2], K[10][2];
        float resid[2];
        B[0][0]=ets.m_cov[0][0];B[0][1]=ets.m_cov[0][1];
        B[1][0]=ets.m_cov[0][1];B[1][1]=ets.m_cov[1][1];
        B[2][0]=ets.m_cov[0][2];B[2][1]=ets.m_cov[1][2];
        B[3][0]=ets.m_cov[0][3];B[3][1]=ets.m_cov[1][3];
        B[4][0]=ets.m_cov[0][4];B[4][1]=ets.m_cov[1][4];
        B[5][0]=ets.m_cov[0][5];B[5][1]=ets.m_cov[1][5];
        B[6][0]=ets.m_cov[0][6];B[6][1]=ets.m_cov[1][6];
        B[7][0]=ets.m_cov[0][7];B[7][1]=ets.m_cov[1][7];
        B[8][0]=ets.m_cov[0][8];B[8][1]=ets.m_cov[1][8];
        B[9][0]=ets.m_cov[0][9];B[9][1]=ets.m_cov[1][9];
        resid[0]=meas[0]-ets.m_par[0];resid[1]=meas[1]-ets.m_par[1];
        float W[2][2], D[2][2];
        float det;
        W[0][0]=ets.m_cov[0][0]+measCov[0][0];
        W[1][0]=W[0][1]=ets.m_cov[0][1]+measCov[0][1];
        W[1][1]=ets.m_cov[1][1]+measCov[1][1];
        det=W[0][0]*W[1][1]-W[0][1]*W[0][1];det=1/det;
        D[0][0]=W[1][1]*det;D[0][1]=-W[0][1]*det;
        D[1][0]=D[0][1];D[1][1]=W[0][0]*det;
        dchi2=resid[0]*resid[0]*D[0][0]+2*resid[0]*resid[1]*D[0][1]+
          resid[1]*resid[1]*D[1][1];
        for(int ii=0;ii<10;ii++)
          for(int jj=0;jj<2;jj++)
            K[ii][jj]=B[ii][0]*D[0][jj]+B[ii][1]*D[1][jj];
        for(int ii=0;ii<10;ii++)
          updState[ii]=K[ii][0]*resid[0]+K[ii][1]*resid[1];
        int covIdx=0;
        for(int ii=0;ii<10;ii++) for(int jj=ii;jj<10;jj++) {
          updCov[covIdx]=-K[ii][0]*B[jj][0]-K[ii][1]*B[jj][1];covIdx++;
        }
        dNdof=2;
      }
      if(hitType==2) {// SCT barrel
        float B[10], K[10];
        float D, resid;
        for(int ii=0;ii<10;ii++) B[ii]=ets.m_cov[0][ii];
        D=1/(ets.m_cov[0][0]+measCov[0][0]);
        resid=meas[0]-ets.m_par[0];
        dchi2=resid*resid*D;
        for(int ii=0;ii<10;ii++) K[ii]=D*B[ii];
        for(int ii=0;ii<10;ii++) updState[ii]=K[ii]*resid;
        int covIdx=0;
        for(int ii=0;ii<10;ii++) for(int jj=ii;jj<10;jj++) {
          updCov[covIdx]=-K[ii]*B[jj];covIdx++;
        }
        dNdof=1;
      }
      if(hitType==3) {//endcap SCT
        //doUpdate=false;
        float corr;
        float H[2],B[10],K[10];
        corr=1/(ets.m_par[1]+meas[1]);
        H[0]=meas[1]*corr;
        H[1]=-ets.m_par[0]*meas[1]*corr*corr;
        B[0]=H[0]*ets.m_cov[0][0]+H[1]*ets.m_cov[0][1];
        B[1]=H[0]*ets.m_cov[0][1]+H[1]*ets.m_cov[1][1];
        B[2]=H[0]*ets.m_cov[0][2]+H[1]*ets.m_cov[1][2];
        B[3]=H[0]*ets.m_cov[0][3]+H[1]*ets.m_cov[1][3];
        B[4]=H[0]*ets.m_cov[0][4]+H[1]*ets.m_cov[1][4];
        B[5]=H[0]*ets.m_cov[0][5]+H[1]*ets.m_cov[1][5];
        B[6]=H[0]*ets.m_cov[0][6]+H[1]*ets.m_cov[1][6];
        B[7]=H[0]*ets.m_cov[0][7]+H[1]*ets.m_cov[1][7];
        B[8]=H[0]*ets.m_cov[0][8]+H[1]*ets.m_cov[1][8];
        B[9]=H[0]*ets.m_cov[0][9]+H[1]*ets.m_cov[1][9];
        corr=1+ets.m_par[1]/meas[1];
        float D=1/(B[0]*H[0]+B[1]*H[1]+measCov[0][0]*corr*corr);
        float resid=meas[0]-ets.m_par[0]*meas[1]/(ets.m_par[1]+meas[1]);
        dchi2=resid*resid*D;
        for(int ii=0;ii<10;ii++) K[ii]=D*B[ii];
        for(int ii=0;ii<10;ii++) updState[ii]=K[ii]*resid;
        int covIdx=0;
        for(int ii=0;ii<10;ii++) for(int jj=ii;jj<10;jj++) {
          updCov[covIdx]=-K[ii]*B[jj];covIdx++;
        }
        dNdof=1;
      }
      if(doUpdate) {
        chi2+=dchi2;
        ndof+=dNdof;
        for(int ii=0;ii<10;ii++) ets.m_par[ii]=ets.m_par[ii]+updState[ii];
        if(ets.m_par[2]>M_PI ) ets.m_par[2]-=2*M_PI;
        if(ets.m_par[2]<-M_PI) ets.m_par[2]+=2*M_PI;
        if(ets.m_par[7]>M_PI ) ets.m_par[7]-=2*M_PI;
        if(ets.m_par[7]<-M_PI) ets.m_par[7]+=2*M_PI;
        if(ets.m_par[3]<0.0)  ets.m_par[3]+=M_PI;
        if(ets.m_par[3]>M_PI) ets.m_par[3]-=M_PI;
        if(ets.m_par[8]<0.0)  ets.m_par[8]+=M_PI;
        if(ets.m_par[8]>M_PI) ets.m_par[8]-=M_PI;
        int covIdx=0;
        for(int ii=0;ii<10;ii++) {
          for(int jj=ii;jj<10;jj++) {
            ets.m_cov[ii][jj]=ets.m_cov[ii][jj]+updCov[covIdx];covIdx++;
            ets.m_cov[jj][ii]=ets.m_cov[ii][jj];
          }
        }
      }
      if(iter==(*d_In).m_nHits[idx]) break;
      //A2. local->global, J0
      sint=sin(ets.m_par[3]);
      cosf=cos(ets.m_par[2]);
      sinf=sin(ets.m_par[2]);
      cost=cos(ets.m_par[3]);
      float J0[7][5],r[3];
      gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;CQ=C*ets.m_par[4];
      memset(&J0[0][0],0,sizeof(J0));
      gP[0]=ets.m_par[0];gP[1]=ets.m_par[1];gP[2]=0.0;
      r[0]=c[0]+m[0][0]*gP[0]+m[0][1]*gP[1];
      r[1]=c[1]+m[1][0]*gP[0]+m[1][1]*gP[1];
      gP[2]=c[2]+m[2][0]*gP[0]+m[2][1]*gP[1];
      gP[0]=r[0];gP[1]=r[1];
      //std::cout<<"P/V: "<<gP[0]<<" "<<gP[1]<<" "<<gP[2]<<" "<<gV[0]<<" "<<gV[1]<<" "<<gV[2]<<std::endl;
      //std::cout<<"at "<<geoIdx<<" "<<c[0]<<" "<<c[1]<<" "<<c[2]<<std::endl;
      J0[0][0]=m[0][0];J0[0][1]=m[0][1];
      J0[1][0]=m[1][0];J0[1][1]=m[1][1];
      J0[2][0]=m[2][0];J0[2][1]=m[2][1];
      J0[3][2]=-sinf*sint;J0[3][3]=cosf*cost;
      J0[4][2]= cosf*sint;J0[4][3]=sinf*cost;
      J0[5][3]=-sint;J0[6][4]=1;
      //A3. read next surface
      geoIdx = iter*Ntracks+idx;
      c[0]=(*d_Geo).m_rowX[geoIdx].w;c[1]=(*d_Geo).m_rowY[geoIdx].w;c[2]=(*d_Geo).m_rowZ[geoIdx].w;
      //std::cout<<"to "<<geoIdx<<" "<<c[0]<<" "<<c[1]<<" "<<c[2]<<std::endl;
      m[0][0]=(*d_Geo).m_rowX[geoIdx].x;m[0][1]=(*d_Geo).m_rowX[geoIdx].y;m[0][2]=(*d_Geo).m_rowX[geoIdx].z;
      m[1][0]=(*d_Geo).m_rowY[geoIdx].x;m[1][1]=(*d_Geo).m_rowY[geoIdx].y;m[1][2]=(*d_Geo).m_rowY[geoIdx].z;
      m[2][0]=(*d_Geo).m_rowZ[geoIdx].x;m[2][1]=(*d_Geo).m_rowZ[geoIdx].y;m[2][2]=(*d_Geo).m_rowZ[geoIdx].z;
      float4 ip2=(*d_Geo).m_parSet[geoIdx];
      float radLength=ip2.w;
      g[0][0]= m[1][1]*m[2][2]-m[1][2]*m[2][1];
      g[1][0]=-m[1][0]*m[2][2]+m[1][2]*m[2][0];
      g[2][0]= m[1][0]*m[2][1]-m[1][1]*m[2][0];
      g[0][1]=-m[0][1]*m[2][2]+m[0][2]*m[2][1];
      g[1][1]= m[0][0]*m[2][2]-m[0][2]*m[2][0];
      g[2][1]=-m[0][0]*m[2][1]+m[0][1]*m[2][0];
      g[0][2]= m[0][1]*m[1][2]-m[0][2]*m[1][1];
      g[1][2]=-m[0][0]*m[1][2]+m[0][2]*m[1][0];
      g[2][2]= m[0][0]*m[1][1]-m[0][1]*m[1][0];

      //A4. global->global extrapolation, Jm
      float n[3];
      n[0]=m[0][2];n[1]=m[1][2];n[2]=m[2][2];
      float D[4];
      D[0]=n[0];
      D[1]=n[1];
      D[2]=n[2];
      D[3]=-n[0]*c[0]-n[1]*c[1]-n[2]*c[2];

      gB[0]=(ip1.x+ip2.x)/2;
      gB[1]=(ip1.y+ip2.y)/2;
      gB[2]=(ip1.z+ip2.z)/2;
      //std::cout<<"Bx="<<gB[0]<<" By="<<gB[1]<<" Bz="<<gB[2]<<std::endl;
      ip1=ip2;

      cc=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
      b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
      a=CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+gB[1]*(D[2]*gV[0]-D[0]*gV[2])+gB[2]*(D[0]*gV[1]-D[1]*gV[0]))/2;
      float descr=b*b-4*a*cc;
      if(descr<0.0) {
        status=0;
        continue;
      }

      sl=-cc/b;sl=sl*(1-a*sl/b);
      //std::cout<<"expected path="<<sl<<std::endl;
      int nStepMax;
      if(fabs(sl)<minStep) nStepMax=1;
      else {
        nStepMax=(int)(fabs(sl)/minStep)+1;
      }
      if((nStepMax<0)||(nStepMax>1000)){
        status=0;
        continue;
      }
      float Av=sl*CQ;
      float Ac=sl*Av/2;
      float C2=sl*sl*C/2;

      float DVx=gV[1]*gB[2]-gV[2]*gB[1];
      float DVy=gV[2]*gB[0]-gV[0]*gB[2];
      float DVz=gV[0]*gB[1]-gV[1]*gB[0];

      float V[3];
      V[0]=gV[0]+Av*DVx;V[1]=gV[1]+Av*DVy;V[2]=gV[2]+Av*DVz;

      float Jm[7][7];
      memset(&Jm[0][0],0,sizeof(Jm));
      float coeff[3], dadVx,dadVy,dadVz,dadQ,dsdx,dsdy,dsdz,dsdVx,dsdVy,dsdVz,dsdQ;
      coeff[0]=-cc*cc/(b*b*b);coeff[1]=cc*(1+3*cc*a/(b*b))/(b*b);coeff[2]=-(1+2*cc*a/(b*b))/b;
      dadVx=CQ*(-D[1]*gB[2]+D[2]*gB[1])/2;
      dadVy=CQ*( D[0]*gB[2]-D[2]*gB[0])/2;
      dadVz=CQ*(-D[0]*gB[1]+D[1]*gB[0])/2;
      dadQ=C*(D[0]*DVx+D[1]*DVy+D[2]*DVz)/2;
      dsdx=coeff[2]*D[0];
      dsdy=coeff[2]*D[1];
      dsdz=coeff[2]*D[2];
      dsdVx=coeff[0]*dadVx+coeff[1]*D[0];
      dsdVy=coeff[0]*dadVy+coeff[1]*D[1];
      dsdVz=coeff[0]*dadVz+coeff[1]*D[2];
      dsdQ=coeff[0]*dadQ;

      Jm[0][0]=1 +V[0]*dsdx;
      Jm[0][1]=    V[0]*dsdy;
      Jm[0][2]=    V[0]*dsdz;

      Jm[0][3]= sl+V[0]*dsdVx;
      Jm[0][4]=    V[0]*dsdVy+Ac*gB[2];
      Jm[0][5]=    V[0]*dsdVz-Ac*gB[1];
      Jm[0][6]=    V[0]*dsdQ+C2*DVx;

      Jm[1][0]=    V[1]*dsdx;
      Jm[1][1]=1 +V[1]*dsdy;
      Jm[1][2]=    V[1]*dsdz;

      Jm[1][3]=    V[1]*dsdVx-Ac*gB[2];
      Jm[1][4]= sl+V[1]*dsdVy;
      Jm[1][5]=    V[1]*dsdVz+Ac*gB[0];
      Jm[1][6]=    V[1]*dsdQ+C2*DVy;

      Jm[2][0]=    V[2]*dsdx;
      Jm[2][1]=    V[2]*dsdy;
      Jm[2][2]=1 +V[2]*dsdz;
      Jm[2][3]=    V[2]*dsdVx+Ac*gB[1];
      Jm[2][4]=    V[2]*dsdVy-Ac*gB[0];
      Jm[2][5]= sl+V[2]*dsdVz;
      Jm[2][6]=    V[2]*dsdQ+C2*DVz;

      Jm[3][0]=dsdx*CQ*DVx;
      Jm[3][1]=dsdy*CQ*DVx;
      Jm[3][2]=dsdz*CQ*DVx;

      Jm[3][3]=1 +dsdVx*CQ*DVx;
      Jm[3][4]=CQ*(dsdVy*DVx+sl*gB[2]);
      Jm[3][5]=CQ*(dsdVz*DVx-sl*gB[1]);

      Jm[3][6]=(CQ*dsdQ+C*sl)*DVx;

      Jm[4][0]=dsdx*CQ*DVy;
      Jm[4][1]=dsdy*CQ*DVy;
      Jm[4][2]=dsdz*CQ*DVy;

      Jm[4][3]=CQ*(dsdVx*DVy-sl*gB[2]);
      Jm[4][4]=1 +dsdVy*CQ*DVy;
      Jm[4][5]=CQ*(dsdVz*DVy+sl*gB[0]);

      Jm[4][6]=(CQ*dsdQ+C*sl)*DVy;

      Jm[5][0]=dsdx*CQ*DVz;
      Jm[5][1]=dsdy*CQ*DVz;
      Jm[5][2]=dsdz*CQ*DVz;
      Jm[5][3]=CQ*(dsdVx*DVz+sl*gB[1]);
      Jm[5][4]=CQ*(dsdVy*DVz-sl*gB[0]);
      Jm[5][5]=1 +dsdVz*CQ*DVz;
      Jm[5][6]=(CQ*dsdQ+C*sl)*DVz;

      Jm[6][6]=1.0f;

      float J1[5][7];

      memset(&J1[0][0],0,sizeof(J1));

      J1[0][0]=g[0][0];J1[0][1]=g[0][1];J1[0][2]=g[0][2];
      J1[1][0]=g[1][0];J1[1][1]=g[1][1];J1[1][2]=g[1][2];
      J1[2][3]=-V[1]/(V[0]*V[0]+V[1]*V[1]);
      J1[2][4]= V[0]/(V[0]*V[0]+V[1]*V[1]);
      J1[3][5]=-1/sqrt(1-V[2]*V[2]);
      J1[4][6]=1.0f;

      float Buf[5][7];
      memset(&Buf[0][0],0,sizeof(Buf));

      for(int ii=0;ii<7;ii++) {
        for(int jj=0;jj<2;jj++)
          Buf[jj][ii]=J1[jj][0]*Jm[0][ii]+J1[jj][1]*Jm[1][ii]+J1[jj][2]*Jm[2][ii];
        Buf[2][ii]=J1[2][3]*Jm[3][ii]+J1[2][4]*Jm[4][ii];
        Buf[3][ii]=J1[3][5]*Jm[5][ii];
        Buf[4][ii]=Jm[6][ii];
      }

      memset(&J[0][0],0,sizeof(J));
      for(int ii=0;ii<5;ii++)
        for(int jj=0;jj<5;jj++) {
          for(int kk=0;kk<7;kk++) J[ii][jj]+=Buf[ii][kk]*J0[kk][jj];
        }

      for(int ii=0;ii<5;ii++)
        for(int jj=0;jj<5;jj++) {
          JG[ii][jj]=0.0;for(int kk=0;kk<5;kk++) JG[ii][jj]+=J[ii][kk]*ets.m_cov[kk][jj];
        }
      for(int ii=0;ii<5;ii++)
        for(int jj=ii;jj<5;jj++) {
          ets.m_cov[ii][jj]=0.0;
          for(int kk=0;kk<5;kk++) ets.m_cov[ii][jj]+=JG[ii][kk]*J[jj][kk];
          ets.m_cov[jj][ii]=ets.m_cov[ii][jj];
        }
      memset(&JG[0][0],0,sizeof(JG));
      for(int ii=0;ii<5;ii++)
        for(int jj=0;jj<5;jj++) {
          for(int kk=0;kk<5;kk++) JG[ii][jj]+=J[ii][kk]*ets.m_cov[kk][jj+5];
        }
      for(int ii=0;ii<5;ii++) 
        for(int jj=0;jj<5;jj++) {
          ets.m_cov[ii][jj+5]=ets.m_cov[ii+5][jj]=JG[ii][jj];
        }

      int nStep=nStepMax;

      while(nStep>0) {
        cc=D[0]*gP[0]+D[1]*gP[1]+D[2]*gP[2]+D[3];
        b=D[0]*gV[0]+D[1]*gV[1]+D[2]*gV[2];
        a=0.5f*CQ*(gB[0]*(D[1]*gV[2]-D[2]*gV[1])+gB[1]*(D[2]*gV[0]-D[0]*gV[2])+gB[2]*(D[0]*gV[1]-D[1]*gV[0]));
        sl=-cc/b;
        sl=sl*(1-a*sl/b);
        float ds=sl/nStep;
        //path+=ds;
        Av=ds*CQ;
        Ac=0.5f*ds*Av;
        DVx=gV[1]*gB[2]-gV[2]*gB[1];
        DVy=gV[2]*gB[0]-gV[0]*gB[2];
        DVz=gV[0]*gB[1]-gV[1]*gB[0];
        gP[0]=gP[0]+gV[0]*ds+Ac*DVx;gP[1]=gP[1]+gV[1]*ds+Ac*DVy;gP[2]=gP[2]+gV[2]*ds+Ac*DVz;
        gV[0]=gV[0]+Av*DVx;gV[1]=gV[1]+Av*DVy;gV[2]=gV[2]+Av*DVz;
        nStep--;
      }
      float xc[3];
      xc[0]=gP[0]-c[0];xc[1]=gP[1]-c[1];xc[2]=gP[2]-c[2];
      gP[0]=g[0][0]*xc[0]+g[0][1]*xc[1]+g[0][2]*xc[2];
      gP[1]=g[1][0]*xc[0]+g[1][1]*xc[1]+g[1][2]*xc[2];
      gP[2]=g[2][0]*xc[0]+g[2][1]*xc[1]+g[2][2]*xc[2];

      ets.m_par[0]=gP[0];ets.m_par[1]=gP[1];
      ets.m_par[2]= atan2f(gV[1],gV[0]);
      if(fabs(gV[2])>1.0) {
        status = 0;
        continue;
      }
      ets.m_par[3]=acosf(gV[2]);
      //A5. material correction

      lV[0]=g[0][0]*gV[0]+g[0][1]*gV[1]+g[0][2]*gV[2];
      lV[1]=g[1][0]*gV[0]+g[1][1]*gV[1]+g[1][2]*gV[2];
      lV[2]=g[2][0]*gV[0]+g[2][1]*gV[1]+g[2][2]*gV[2];

      lenCorr=1/fabs(lV[2]);
      sinThetaLoc=sqrt(lV[0]*lV[0]+lV[1]*lV[1]);
      radLength*=lenCorr;
      sigmaMS=13.6f*fabs(0.001f*ets.m_par[4])*sqrt(radLength)*(1+0.038f*log(radLength));
      ms2=sigmaMS*sigmaMS;invSin=1/sinThetaLoc;invSin2=invSin*invSin;
      ets.m_cov[2][2]+=ms2*invSin2;ets.m_cov[3][3]+=ms2;ets.m_cov[2][3]+=ms2*invSin;
      ets.m_cov[3][2]=ets.m_cov[2][3];
      //A6. read next hit
      hitType = (*d_Hit).m_hitType[geoIdx];
      meas[0] = (*d_Hit).m_meas[geoIdx].x;
      meas[1] = (*d_Hit).m_meas[geoIdx].y;
      measCov[0][0] = (*d_Hit).m_cov[geoIdx].x;measCov[0][1] = (*d_Hit).m_cov[geoIdx].y;
      measCov[1][0] = (*d_Hit).m_cov[geoIdx].z;measCov[1][1] = (*d_Hit).m_cov[geoIdx].w;
    }

    //B1. reading first plane

    geoIdx = idx;
    c[0]=(*d_Geo).m_rowX[geoIdx].w;c[1]=(*d_Geo).m_rowY[geoIdx].w;c[2]=(*d_Geo).m_rowZ[geoIdx].w;
    m[0][0]=(*d_Geo).m_rowX[geoIdx].x;m[0][1]=(*d_Geo).m_rowX[geoIdx].y;m[0][2]=(*d_Geo).m_rowX[geoIdx].z;
    m[1][0]=(*d_Geo).m_rowY[geoIdx].x;m[1][1]=(*d_Geo).m_rowY[geoIdx].y;m[1][2]=(*d_Geo).m_rowY[geoIdx].z;
    m[2][0]=(*d_Geo).m_rowZ[geoIdx].x;m[2][1]=(*d_Geo).m_rowZ[geoIdx].y;m[2][2]=(*d_Geo).m_rowZ[geoIdx].z;
    g[0][0]= m[1][1]*m[2][2]-m[1][2]*m[2][1];
    g[1][0]=-m[1][0]*m[2][2]+m[1][2]*m[2][0];
    g[2][0]= m[1][0]*m[2][1]-m[1][1]*m[2][0];
    g[0][1]=-m[0][1]*m[2][2]+m[0][2]*m[2][1];
    g[1][1]= m[0][0]*m[2][2]-m[0][2]*m[2][0];
    g[2][1]=-m[0][0]*m[2][1]+m[0][1]*m[2][0];
    g[0][2]= m[0][1]*m[1][2]-m[0][2]*m[1][1];
    g[1][2]=-m[0][0]*m[1][2]+m[0][2]*m[1][0];
    g[2][2]= m[0][0]*m[1][1]-m[0][1]*m[1][0];
    ip1=(*d_Geo).m_parSet[geoIdx];
    float radLength=ip1.w;

    //B2. material correction

    sint=sin(ets.m_par[8]);
    cosf=cos(ets.m_par[7]);
    sinf=sin(ets.m_par[7]);
    cost=cos(ets.m_par[8]);

    gV[0]=sint*cosf;gV[1]=sint*sinf;gV[2]=cost;

    lV[0]=g[0][0]*gV[0]+g[0][1]*gV[1]+g[0][2]*gV[2];
    lV[1]=g[1][0]*gV[0]+g[1][1]*gV[1]+g[1][2]*gV[2];
    lV[2]=g[2][0]*gV[0]+g[2][1]*gV[1]+g[2][2]*gV[2];

    lenCorr=1/fabs(lV[2]);
    sinThetaLoc=sqrt(lV[0]*lV[0]+lV[1]*lV[1]);
    radLength*=lenCorr;
    sigmaMS=13.6f*fabs(0.001f*ets.m_par[9])*sqrt(radLength)*(1+0.038f*log(radLength));
    ms2=sigmaMS*sigmaMS;invSin=1/sinThetaLoc;invSin2=invSin*invSin;
    ets.m_cov[7][7]+=ms2*invSin2;ets.m_cov[8][8]+=ms2;ets.m_cov[7][8]+=ms2*invSin;
    ets.m_cov[8][7]=ets.m_cov[7][8];

    //B3. transform to global

    float dVxdf,dVydf,dVxdt,dVydt,
          dsdx0,dsdy0,dsdf0,dsdt0,dVzdt;

    gB[0]=(ip1.x+B0[0])/2;
    gB[1]=(ip1.y+B0[1])/2;
    gB[2]=(ip1.z+B0[2])/2;
    gP[0]=c[0]+m[0][0]*ets.m_par[5]+m[0][1]*ets.m_par[6];
    gP[1]=c[1]+m[1][0]*ets.m_par[5]+m[1][1]*ets.m_par[6];
    gP[2]=c[2]+m[2][0]*ets.m_par[5]+m[2][1]*ets.m_par[6];

    CQ=C*ets.m_par[9];
    a=3*CQ*gV[2]*(gB[0]*gV[1]-gB[1]*gV[0])/2;
    b=gV[0]*gV[0]+gV[1]*gV[1]+CQ*(gP[0]*(gV[1]*gB[2]-gV[2]*gB[1])+
        gP[1]*(gV[2]*gB[0]-gV[0]*gB[2]));
    cc=gP[0]*gV[0]+gP[1]*gV[1];
    sl=-cc/b;
    sl=sl*(1-sl*a/b);
    float s2Q=sl*sl*CQ/2;
    gP[0]+=gV[0]*sl+s2Q*(gV[1]*gB[2]-gV[2]*gB[1]);
    gP[1]+=gV[1]*sl+s2Q*(gV[2]*gB[0]-gV[0]*gB[2]);
    gP[2]+=gV[2]*sl+s2Q*(gV[0]*gB[1]-gV[1]*gB[0]);
    lV[0]=gV[0]+sl*CQ*(gV[1]*gB[2]-gV[2]*gB[1]);
    lV[1]=gV[1]+sl*CQ*(gV[2]*gB[0]-gV[0]*gB[2]);
    lV[2]=gV[2]+sl*CQ*(gV[0]*gB[1]-gV[1]*gB[0]);
    for(int ii=0;ii<3;ii++) gV[ii]=lV[ii];
    dVxdf=-sint*sinf;
    dVxdt= cost*cosf;
    dVydf= sint*cosf;
    dVydt= cost*sinf;
    dVzdt= -sint;

    dsdx0=-(m[0][0]*cosf+m[1][0]*sinf)/sint;
    dsdy0=-(m[0][1]*cosf+m[1][1]*sinf)/sint;
    dsdf0= (gP[0]*sinf-gP[1]*cosf)/sint;
    dsdt0= (gP[0]*cosf+gP[1]*sinf)*cost/(sint*sint);

    memset(&J[0][0],0,sizeof(J));

    J[4][4]=1;
    J[3][3]=1;
    J[2][0]=-CQ*gB[2]*dsdx0;
    J[2][1]=-CQ*gB[2]*dsdy0;
    J[2][2]=1-CQ*gB[2]*dsdf0;
    J[2][3]=-CQ*gB[2]*dsdt0;
    J[2][4]=-C*gB[2]*sl;

    J[1][0]=m[2][0]+dsdx0*gV[2];
    J[1][1]=m[2][1]+dsdy0*gV[2];
    J[1][2]=dsdf0*gV[2];
    J[1][3]=dsdt0*gV[2]+sl*dVzdt;
    float d0=sqrt(gP[0]*gP[0]+gP[1]*gP[1]);
    float delta=gV[0]*gP[1]-gV[1]*gP[0];
    if(delta<0.0) d0=-d0;

    J[0][0]=(gP[0]*(m[0][0]+dsdx0*gV[0])+gP[1]*(m[1][0]+dsdx0*gV[1]))/d0;
    J[0][1]=(gP[0]*(m[0][1]+dsdy0*gV[0])+gP[1]*(m[1][1]+dsdy0*gV[1]))/d0;
    J[0][2]=(gP[0]*(dVxdf*sl+dsdf0*gV[0])+gP[1]*(dVydf*sl+dsdf0*gV[1]))/d0;
    J[0][3]=(gP[0]*(dVxdt*sl+dsdt0*gV[0])+gP[1]*(dVydt*sl+dsdt0*gV[1]))/d0;

    for(int ii=0;ii<5;ii++)
      for(int jj=0;jj<5;jj++) {
        JG[ii][jj]=0.0;
        for(int kk=0;kk<5;kk++) JG[ii][jj]+=J[ii][kk]*ets.m_cov[kk+5][jj+5];
      }
    for(int ii=0;ii<5;ii++)
      for(int jj=ii;jj<5;jj++) {
        ets.m_cov[ii+5][jj+5]=0.0;
        for(int kk=0;kk<5;kk++) ets.m_cov[ii+5][jj+5]+=JG[ii][kk]*J[jj][kk];
        ets.m_cov[jj+5][ii+5]=ets.m_cov[ii+5][jj+5];
      }
    for(int ii=0;ii<5;ii++)
      for(int jj=0;jj<5;jj++) {
        JG[ii][jj]=0.0;
        for(int kk=0;kk<5;kk++) JG[ii][jj]+=J[ii][kk]*ets.m_cov[kk+5][jj];
      }
    for(int ii=0;ii<5;ii++)
      for(int jj=0;jj<5;jj++) {
        ets.m_cov[ii][jj+5]=ets.m_cov[ii+5][jj]=JG[ii][jj];
      }
    ets.m_par[5]=d0;
    ets.m_par[6]=gP[2];
    ets.m_par[7]=atan2f(gV[1],gV[0]);
    ets.m_par[8]=acosf(gV[2]);
    ets.m_par[4]=ets.m_par[4]/1000;
    ets.m_par[9]=ets.m_par[9]/1000;
    (*d_Out).m_status[idx] = status;
    (*d_Out).m_chi2[idx]=chi2;
    (*d_Out).m_ndof[idx]=ndof;

    (*d_Out).m_par0[idx]=ets.m_par[5];
    (*d_Out).m_par1[idx]=ets.m_par[6];
    (*d_Out).m_par2[idx]=ets.m_par[7];
    (*d_Out).m_par3[idx]=ets.m_par[8];
    (*d_Out).m_par4[idx]=ets.m_par[9];

    (*d_Out).m_cov00[idx]=ets.m_cov[5][5];
    (*d_Out).m_cov01[idx]=ets.m_cov[5][6];
    (*d_Out).m_cov02[idx]=ets.m_cov[5][7];
    (*d_Out).m_cov03[idx]=ets.m_cov[5][8];
    (*d_Out).m_cov04[idx]=ets.m_cov[5][9]/1000;

    (*d_Out).m_cov11[idx]=ets.m_cov[6][6];
    (*d_Out).m_cov12[idx]=ets.m_cov[6][7];
    (*d_Out).m_cov13[idx]=ets.m_cov[6][8];
    (*d_Out).m_cov14[idx]=ets.m_cov[6][9]/1000;

    (*d_Out).m_cov22[idx]=ets.m_cov[7][7];
    (*d_Out).m_cov23[idx]=ets.m_cov[7][8];
    (*d_Out).m_cov24[idx]=ets.m_cov[7][9]/1000;

    (*d_Out).m_cov33[idx]=ets.m_cov[8][8];
    (*d_Out).m_cov34[idx]=ets.m_cov[8][9]/1000;

    (*d_Out).m_cov44[idx]=ets.m_cov[9][9]/1000000;

  }
}
