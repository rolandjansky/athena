/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_GPUQ_H
#define ISF_FASTCALOGPU_GPUQ_H

void gpu_assert( cudaError_t code, const char* file, const int line );

#ifndef gpuQ
#  define gpuQ( ans )                                                                                                  \
    { gpu_assert( ( ans ), __FILE__, __LINE__ ); }

#endif
#endif
