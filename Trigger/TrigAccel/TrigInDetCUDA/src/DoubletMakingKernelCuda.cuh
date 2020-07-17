/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETCUDA_DOUBLETMAKINGKERNELCUDA_CUH
#define TRIGINDETCUDA_DOUBLETMAKINGKERNELCUDA_CUH

#include <cuda_runtime.h>
#include "SeedMakingDataStructures.h"

__global__ static void doubletMakingKernel(TrigAccel::SEED_FINDER_SETTINGS* dSettings, 
					   TrigAccel::SPACEPOINT_STORAGE* dSpacepoints, 
					   TrigAccel::DETECTOR_MODEL* dDetModel, 
					   TrigAccel::OUTPUT_SEED_STORAGE* d_Out, 
					   DOUBLET_INFO* d_Info,
					   DOUBLET_STORAGE* d_Storage, 
					   int nLayers, int nSlices) {

  __shared__ int spBegin;
  __shared__ int spEnd;
  __shared__ int nMiddleSPs;

  __shared__ bool hasDoublets;

  __shared__ int outerPos[NUM_MIDDLE_THREADS];
  __shared__ int innerPos[NUM_MIDDLE_THREADS];

  const float zTolerance = 3.0; 
  const float maxEta = 2.7;
  const float maxDoubletLength = 200.0;
  const float minDoubletLength = 10.0;
  const float maxOuterRadius = 550.0;
  
  const int sliceIdx = blockIdx.x;
  const int layerIdx = blockIdx.y;

  if(threadIdx.x == 0 && threadIdx.y == 0) {
    const TrigAccel::SPACEPOINT_LAYER_RANGE& slr = dSpacepoints->m_phiSlices[sliceIdx];
    spBegin = slr.m_layerBegin[layerIdx];
    spEnd = slr.m_layerEnd[layerIdx];
    nMiddleSPs = spEnd-spBegin;
  }
  __syncthreads();

  if(nMiddleSPs == 0) return;

  const float zMinus = dSettings->m_zedMinus - zTolerance; 
  const float zPlus  = dSettings->m_zedPlus  + zTolerance; 
  const float maxTheta = 2*atan(exp(-maxEta));
  const float maxCtg = cos(maxTheta)/sin(maxTheta);
  const bool DoPSS = dSettings->m_tripletDoPSS;
  const float minOuterZ = dSettings->m_zedMinus - maxOuterRadius*maxCtg - zTolerance; 
  const float maxOuterZ = dSettings->m_zedPlus + maxOuterRadius*maxCtg + zTolerance; 

  //1. get a tile of middle spacepoints

  __syncthreads();

  for(int spmIdx=threadIdx.x+spBegin;spmIdx<spEnd;spmIdx+=blockDim.x) {

    if(threadIdx.y ==0) {
       hasDoublets = d_Info->m_good[spmIdx] == 1;
    }
    __syncthreads();

    if(!hasDoublets) continue;

    if(threadIdx.y ==0) {//loading pre-calculated numbers of doublets ...

      int nInner = d_Info->m_nInner[spmIdx];
      int nOuter = d_Info->m_nOuter[spmIdx];

      outerPos[threadIdx.x] = atomicAdd(&d_Storage->m_nO, nOuter);
      innerPos[threadIdx.x] = atomicAdd(&d_Storage->m_nI, nInner);
      int k = atomicAdd(&d_Storage->m_nItems, 1);
      d_Storage->m_spmIdx[k] = spmIdx;
      d_Storage->m_innerStart[k] = innerPos[threadIdx.x];
      d_Storage->m_outerStart[k] = outerPos[threadIdx.x];
    }

    __syncthreads();
    
    int spmType = dSpacepoints->m_type[spmIdx];
    float zm = dSpacepoints->m_z[spmIdx];
    float rm = dSpacepoints->m_r[spmIdx];
    bool isPixel = (spmType == 1);

    //2. loop over other phi-bins / layers

    for(int deltaPhiIdx=-1;deltaPhiIdx<=1;deltaPhiIdx++) {
      int nextPhiIdx = sliceIdx + deltaPhiIdx;
      if(nextPhiIdx>=nSlices) nextPhiIdx = 0;
      if(nextPhiIdx<0) nextPhiIdx=nSlices-1;
      const TrigAccel::SPACEPOINT_LAYER_RANGE& next_slr = dSpacepoints->m_phiSlices[nextPhiIdx];

      for(int nextLayerIdx=0;nextLayerIdx<nLayers;nextLayerIdx++) {

	if(nextLayerIdx == layerIdx) continue;

	int next_spBegin = next_slr.m_layerBegin[nextLayerIdx];
	int next_spEnd = next_slr.m_layerEnd[nextLayerIdx];

	if(next_spEnd == next_spBegin) continue;//no spacepoints in this layer

	const TrigAccel::SILICON_LAYER& layerGeo =  dDetModel->m_layers[nextLayerIdx];
	bool isBarrel = (layerGeo.m_type == 0);
	float refCoord = layerGeo.m_refCoord;
	
	if(isBarrel && fabs(refCoord-rm)>maxDoubletLength) continue;

	//boundaries for nextLayer

	float minCoord = 10000.0;
	float maxCoord =-10000.0;

	if(isBarrel) {
	  minCoord = zMinus + refCoord*(zm-zMinus)/rm;
	  maxCoord = zPlus + refCoord*(zm-zPlus)/rm;
	}
	else {
	  minCoord = rm*(refCoord-zMinus)/(zm-zMinus);
	  maxCoord = rm*(refCoord-zPlus)/(zm-zPlus);
	}

	if(minCoord>maxCoord) {
	  float tmp = maxCoord;maxCoord = minCoord;minCoord = tmp;
	}

	if(layerGeo.m_maxBound<minCoord || layerGeo.m_minBound>maxCoord) continue;

	//3. get a tile of inner/outer spacepoints

	for(int spIdx=threadIdx.y+next_spBegin;spIdx<next_spEnd;spIdx+=blockDim.y) {

	  float zsp = dSpacepoints->m_z[spIdx];
	  float rsp = dSpacepoints->m_r[spIdx];

	  float spCoord = (isBarrel) ? zsp : rsp;

	  if(spCoord<minCoord || spCoord>maxCoord) continue;

	  bool isPixel2 = (dSpacepoints->m_type[spIdx] == 1);
	  float dr = rsp - rm;

	  if(fabs(dr)>maxDoubletLength || fabs(dr)<minDoubletLength) continue;

	  if(!DoPSS && dr<0 && !isPixel && isPixel2) continue; 

	  float dz = zsp - zm;
	  float t = dz/dr;

	  if(fabs(t)>maxCtg) continue;
          //if(dr > 0) {//outer doublet
            float outZ = zsp + (maxOuterRadius-rsp)*t; 
            if(outZ < minOuterZ || outZ > maxOuterZ) continue;
          //}
	  if(dr > 0) {
	    int k = atomicAdd(&outerPos[threadIdx.x],1);
	    d_Storage->m_outer[k] = spIdx;
	  }
	  else {
	    int k = atomicAdd(&innerPos[threadIdx.x],1);
	    d_Storage->m_inner[k] = spIdx;
	  }
	}
      }
    }
  }
  __syncthreads();

  if(threadIdx.x == 0 && threadIdx.y == 0) {
    atomicAdd(&d_Out->m_nMiddleSps, nMiddleSPs);
  }

}

#endif