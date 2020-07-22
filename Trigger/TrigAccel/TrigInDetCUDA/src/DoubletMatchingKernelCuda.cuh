/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETCUDA_DOUBLETMATCHINGKERNELCUDA_CUH
#define TRIGINDETCUDA_DOUBLETMATCHINGKERNELCUDA_CUH


#include <cuda_runtime.h>
#include "SeedMakingDataStructures.h"


__global__ static void doubletMatchingKernel(TrigAccel::SEED_FINDER_SETTINGS* dSettings, 
					     TrigAccel::SPACEPOINT_STORAGE* dSpacepoints, 
					     TrigAccel::DETECTOR_MODEL* dDetModel, 
					     DOUBLET_INFO* d_Info,
					     DOUBLET_STORAGE* d_Storage,
					     TrigAccel::OUTPUT_SEED_STORAGE* d_Out, int maxItem) {

  __shared__ int nInner;
  __shared__ int nOuter;

  __shared__ int innerStart;
  __shared__ int outerStart;

  __shared__ int spmIdx;
  __shared__ float rm;

  __shared__ float covZ;
  __shared__ float covR;

  __shared__ float x0;
  __shared__ float y0;
  __shared__ float z0;

  __shared__ float cosA;
  __shared__ float sinA;

  __shared__ bool isPixel;

  //  __shared__ float R2inv_array[MAX_NUMBER_DOUBLETS];
  __shared__ float Rinv_array[MAX_NUMBER_DOUBLETS];
  __shared__ float t_array[MAX_NUMBER_DOUBLETS];
  __shared__ int spIdx_array[MAX_NUMBER_DOUBLETS];
  __shared__ float u_array[MAX_NUMBER_DOUBLETS];
  __shared__ float v_array[MAX_NUMBER_DOUBLETS];
  __shared__ bool isSCT_array[MAX_NUMBER_DOUBLETS];

  //  __shared__ float covZ_array[MAX_NUMBER_DOUBLETS];
  // __shared__ float covR_array[MAX_NUMBER_DOUBLETS];

  __shared__ float tCov_array[MAX_NUMBER_DOUBLETS];


  __shared__ int PairIdx_array[MAX_TRIPLETS];
  __shared__ float Q_array[MAX_TRIPLETS];
  __shared__ int sortedIdx[MAX_TRIPLETS];


  __shared__ int iDoublet;
  __shared__ int startOfOuter;
  __shared__ int nPairs;
  __shared__ int nTriplets;

  const double dtCut = 0.25;
  const float radLen = 0.036;
  const float dp = 13.6/dSettings->m_tripletPtMin;
  const float CovMS = dp*dp*radLen;


  const float ptCoeff = 0.29997*dSettings->m_magFieldZ/2;// ~0.3 
  const float minPt2 = dSettings->m_tripletPtMin*dSettings->m_tripletPtMin; 
  const float ptCoeff2 = ptCoeff*ptCoeff;
  const float maxD0 = dSettings->m_tripletD0Max;
  const float maxD0_PPS = dSettings->m_tripletD0_PPS_Max;


  for(int itemIdx = blockIdx.x;itemIdx<maxItem;itemIdx += gridDim.x) {

    if(threadIdx.x==0) {

      nTriplets = 0;
      iDoublet=0;      

      spmIdx = d_Storage->m_spmIdx[itemIdx];
      nInner = d_Info->m_nInner[spmIdx];
      nOuter = d_Info->m_nOuter[spmIdx];

      nPairs = nInner*nOuter; 

      atomicAdd(&d_Out->m_nO,nOuter);
      atomicAdd(&d_Out->m_nI,nInner);

      innerStart = d_Storage->m_innerStart[itemIdx];
      outerStart = d_Storage->m_outerStart[itemIdx];
      
      int spmType = dSpacepoints->m_type[spmIdx];

      rm = dSpacepoints->m_r[spmIdx];
      covZ = dSpacepoints->m_covZ[spmIdx];
      covR = dSpacepoints->m_covR[spmIdx];
    
      x0 = dSpacepoints->m_x[spmIdx];
      y0 = dSpacepoints->m_y[spmIdx];
      z0 = dSpacepoints->m_z[spmIdx];
      cosA = x0/rm;
      sinA = y0/rm;
      isPixel = (spmType == 1);
    }
    __syncthreads();    

    
    for(int innerIdx = threadIdx.x; innerIdx<nInner;innerIdx+=blockDim.x) {
     
      int k = atomicAdd(&iDoublet,1);  
      
      if(k<MAX_NUMBER_DOUBLETS) {

	int spiIdx = d_Storage->m_inner[innerStart + innerIdx];

	spIdx_array[k] = spiIdx;
	
	float dx_inn = dSpacepoints->m_x[spiIdx] - x0; 
	float dy_inn = dSpacepoints->m_y[spiIdx] - y0;
	float dz_inn = dSpacepoints->m_z[spiIdx] - z0;

	float R2inv = 1.0/(dx_inn*dx_inn+dy_inn*dy_inn); 
	Rinv_array[k] = sqrt(R2inv);	
	t_array[k] = Rinv_array[k]*dz_inn;

	tCov_array[k] = R2inv*(covZ + dSpacepoints->m_covZ[spiIdx] + t_array[k]*t_array[k]*(covR + dSpacepoints->m_covR[spiIdx]));
	
	float xn_inn = dx_inn*cosA + dy_inn*sinA; 
	float yn_inn =-dx_inn*sinA + dy_inn*cosA;	

	u_array[k] = xn_inn*R2inv;
	v_array[k] = yn_inn*R2inv;

	isSCT_array[k] = (dSpacepoints->m_type[spiIdx] == 2);	
      }
    }

    __syncthreads();

    if(threadIdx.x==0) {
      if(iDoublet>MAX_NUMBER_DOUBLETS) iDoublet = MAX_NUMBER_DOUBLETS;
      startOfOuter=iDoublet;
    }	

    __syncthreads();
    	
    for(int outerIdx = threadIdx.x; outerIdx<nOuter;outerIdx+=blockDim.x) {  
    	  
      int k = atomicAdd(&iDoublet,1);
      
      if(k<MAX_NUMBER_DOUBLETS) {

	int spoIdx = d_Storage->m_outer[outerStart + outerIdx];

	spIdx_array[k] = spoIdx;
	
	float dx_out =  dSpacepoints->m_x[spoIdx] - x0;
	float dy_out =  dSpacepoints->m_y[spoIdx] - y0;
	float dz_out = -dSpacepoints->m_z[spoIdx] + z0;

	float R2inv = 1.0/(dx_out*dx_out+dy_out*dy_out);
	Rinv_array[k] = sqrt(R2inv);
	t_array[k] = Rinv_array[k]*dz_out;

	tCov_array[k] = R2inv*(covZ + dSpacepoints->m_covZ[spoIdx] + t_array[k]*t_array[k]*(covR + dSpacepoints->m_covR[spoIdx]));

	float xn_out = dx_out*cosA + dy_out*sinA; 
	float yn_out =-dx_out*sinA + dy_out*cosA;	

	u_array[k] = xn_out*R2inv;
	v_array[k] = yn_out*R2inv;
	
	isSCT_array[k] = (dSpacepoints->m_type[spoIdx] == 2);
      }
    }
  
  __syncthreads();

  for(int pairIdx = threadIdx.x;pairIdx<nPairs;pairIdx += blockDim.x) {

    int doublet_i = pairIdx / nOuter; // inner doublet
    int doublet_j = startOfOuter + pairIdx % nOuter; //outer doublet
    
    if(doublet_i >= MAX_NUMBER_DOUBLETS || doublet_j >=MAX_NUMBER_DOUBLETS ) continue;

    //int spiIdx = spIdx_array[doublet_i];
    //int spoIdx = spIdx_array[doublet_j];
    
    //retrieve shared data for doublets doublet_i and doublet_j and apply cut(s)	  
    
    //0. dt matching
    float t_inn = t_array[doublet_i];
    float t_out = t_array[doublet_j];
    float dt = t_inn - t_out;	
    if(fabs(dt)>dtCut) continue;
    
    //1. rz matching

    float t_inn2 = t_inn*t_inn;
    float tCov_inn = tCov_array[doublet_i];
    float tCov_out = tCov_array[doublet_j];

    double dCov = CovMS*(1+t_inn2);
    
    float covdt = tCov_inn + tCov_out; 
    covdt += 2*Rinv_array[doublet_i]*Rinv_array[doublet_j]*(t_inn*t_out*covR + covZ); 
    float dt2 = dt*dt*(1/9.0);
    if(dt2 > covdt+dCov) continue;//i.e. 3-sigma cut 

    //2. pT estimate 

    float u_inn = u_array[doublet_i];
    float v_inn =  v_array[doublet_i];  
    float u_out = u_array[doublet_j];
    float v_out =  v_array[doublet_j];
    
    float du = u_out - u_inn; 
    if(du==0.0) continue;
    float A = (v_out - v_inn)/du; 
    float B = v_inn - A*u_inn; 
    float pT2 = ptCoeff2*(1+A*A)/(B*B); 
    if(pT2 < minPt2) continue;
    
    //3. the 3-sigma cut with estimated pT 
    
    float frac = minPt2/pT2; 
    if(dt2 > covdt+frac*dCov) continue;

    //4. d0 cut 
    
    float d0 = rm*(B*rm-A);
    float fd0 = fabs(d0);

    if(fd0 > maxD0) continue;

    bool isSCT_1 = isSCT_array[doublet_i];
    bool isSCT_3 = isSCT_array[doublet_j];

    if (isSCT_3 && isPixel && fd0 > maxD0_PPS) continue; 

    //Calculate Quality    

    float Q = d0*d0;
    Q += isSCT_3*(1000.0*isSCT_1 + (1-isSCT_1)*10000.0);

    int l = atomicAdd(&nTriplets, 1);
    if(l<MAX_TRIPLETS) {
      PairIdx_array[l] = pairIdx;
      Q_array[l] = Q;
      sortedIdx[l] = 0;
    }

  }

  __syncthreads();


  if(nTriplets>TRIPLET_BUFFER_DEPTH) {//sorting
    
    if(threadIdx.x == 0){
      if(nTriplets>MAX_TRIPLETS)  {
	nTriplets = MAX_TRIPLETS;
      }      
    }
 
    __syncthreads();

    for(int tIdx=threadIdx.x;tIdx<nTriplets*nTriplets;tIdx+=blockDim.x) {
      int i = tIdx/nTriplets;
      int j = tIdx%nTriplets;
      int d = (Q_array[i] > Q_array[j]) + (Q_array[i] == Q_array[j])*(j<i);
      atomicAdd(&sortedIdx[i],d);
    }
  }
 
  __syncthreads();
  
  if(threadIdx.x == 0) {
    if(nTriplets>0) {
      int nT = nTriplets;
      if(nT>TRIPLET_BUFFER_DEPTH) {nT = TRIPLET_BUFFER_DEPTH;}
      int k = atomicAdd(&d_Out->m_nSeeds, nT);    
      int nStored=0;
      for(int tIdx=0;tIdx<nTriplets;tIdx++) {
	if(sortedIdx[tIdx]<TRIPLET_BUFFER_DEPTH) {//store this triplet

	  int pairIdx = PairIdx_array[tIdx];
	  int doublet_i = pairIdx / nOuter; // inner doublet
	  int doublet_j = startOfOuter + pairIdx % nOuter; //outer doublet
	  int spiIdx = spIdx_array[doublet_i];
	  int spoIdx = spIdx_array[doublet_j];
	  d_Out->m_innerIndex[k+nStored] = dSpacepoints->m_index[spiIdx];
	  d_Out->m_middleIndex[k+nStored] = dSpacepoints->m_index[spmIdx];
	  d_Out->m_outerIndex[k+nStored] = dSpacepoints->m_index[spoIdx];
	  d_Out->m_Q[k+nStored] = Q_array[tIdx];
	  nStored++;
	}
      }
      if(nStored!=nT) {
        atomicAdd(&d_Out->m_nErrors,1);
      }
    }     
  }
  __syncthreads();
  }
}

#endif
