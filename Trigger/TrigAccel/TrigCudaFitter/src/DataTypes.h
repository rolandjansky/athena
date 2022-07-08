#include "vector_types.h"

#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

//#define MAX_TRACK 50
#define MAX_HIT   16
//#define BLOCK_SIZE 32

#define MAX_TRACK 20480 // 5120 10240
#define BLOCK_SIZE 64

typedef struct InputTrackInfoStruct {
  int m_nHits[MAX_TRACK];
  float4 m_parSet1[MAX_TRACK];
  float4 m_parSet2[MAX_TRACK];
} INPUT_TRACK_INFO_TYPE;

typedef struct OutputTrackInfoStruct {
  int m_status[MAX_TRACK];
  int m_ndof[MAX_TRACK];
  float m_par0[MAX_TRACK];
  float m_par1[MAX_TRACK];
  float m_par2[MAX_TRACK];
  float m_par3[MAX_TRACK];
  float m_par4[MAX_TRACK];
  float m_chi2[MAX_TRACK];
  float m_cov00[MAX_TRACK];float m_cov01[MAX_TRACK];float m_cov02[MAX_TRACK];float m_cov03[MAX_TRACK];float m_cov04[MAX_TRACK];
  float m_cov11[MAX_TRACK];float m_cov12[MAX_TRACK];float m_cov13[MAX_TRACK];float m_cov14[MAX_TRACK];
  float m_cov22[MAX_TRACK];float m_cov23[MAX_TRACK];float m_cov24[MAX_TRACK];
  float m_cov33[MAX_TRACK];float m_cov34[MAX_TRACK];
  float m_cov44[MAX_TRACK];
	
} OUTPUT_TRACK_INFO_TYPE;

typedef struct DetectorSurfaceStruct {
  float4 m_rowX[MAX_TRACK*MAX_HIT];
  float4 m_rowY[MAX_TRACK*MAX_HIT];
  float4 m_rowZ[MAX_TRACK*MAX_HIT];
  float4 m_parSet[MAX_TRACK*MAX_HIT];
} DETECTOR_SURFACE_TYPE;

typedef struct HitInfoStruct {
  int m_hitType[MAX_TRACK*MAX_HIT];
  float2 m_meas[MAX_TRACK*MAX_HIT];
  float4 m_cov[MAX_TRACK*MAX_HIT];
} HIT_INFO_TYPE;

typedef struct ExtendedTrackStateStruct {
  float m_par[10];
  float m_cov[10][10];
} EXTENDED_TRACK_STATE_TYPE;

typedef struct ExtendedTrackStateStruct2 {
  float m_par[10];
  float m_cov[55];
} EXTENDED_TRACK_STATE_TYPE_2;

typedef struct JacobianStruct {
  float m_J[5][5];
} JACOBIAN_TYPE;

#endif
