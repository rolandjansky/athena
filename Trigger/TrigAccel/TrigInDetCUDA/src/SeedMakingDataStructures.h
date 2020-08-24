/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETCUDA_SEEDMAKINGDATASTRUCTURES_H
#define TRIGINDETCUDA_SEEDMAKINGDATASTRUCTURES_H

constexpr unsigned int MAX_MIDDLE_SP             = 100000;
constexpr unsigned int MAX_DOUBLET               = 10000000;
constexpr unsigned int NUM_MIDDLE_THREADS        = 32;
constexpr unsigned int OUTER_THREADS_MULTIPLIER  = 4; // i.e thread block is 32 x 4*192/32
constexpr unsigned int MAX_NUMBER_DOUBLETS       = 1500;
constexpr unsigned int NUM_TRIPLET_BLOCKS        = 1024;
constexpr unsigned int NUM_TRIPLET_THREADS       = 1024;
constexpr unsigned int NUM_DOUBLET_THREADS       = 16;
constexpr unsigned int MAX_TRIPLETS              = 300;
constexpr unsigned int TRIPLET_BUFFER_DEPTH      = 3;

typedef struct doubletInfo {
public:
  int m_nInner[MAX_MIDDLE_SP];
  int m_nOuter[MAX_MIDDLE_SP];
  char m_good[MAX_MIDDLE_SP];
} DOUBLET_INFO;

typedef struct doubletStorage {
public:
  int m_nItems;
  int m_nI;
  int m_nO;
  int m_spmIdx[MAX_MIDDLE_SP];  
  int m_innerStart[MAX_MIDDLE_SP];
  int m_outerStart[MAX_MIDDLE_SP];
  int m_inner[MAX_DOUBLET];
  int m_outer[MAX_DOUBLET];
} DOUBLET_STORAGE;

typedef struct gpuParameters {
  int m_nSMX;
  int m_nNUM_SMX_CORES;
  int m_nNUM_TRIPLET_BLOCKS;
} GPU_PARAMETERS;


#endif
