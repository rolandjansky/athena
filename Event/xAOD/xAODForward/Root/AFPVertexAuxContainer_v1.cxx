/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPVertexAuxContainer_v1.cxx
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-12-08
 * 
 * @brief  Implementation file for class xAOD::AFPVertexAuxContainer_v1
 * 
 */


// Local include(s):
#include "xAODForward/versions/AFPVertexAuxContainer_v1.h"
 
namespace xAOD {
 
  AFPVertexAuxContainer_v1::AFPVertexAuxContainer_v1()
    : AuxContainerBase() {
    
  AUX_VARIABLE( position );
  AUX_VARIABLE( distA );
  AUX_VARIABLE( distC );
  AUX_VARIABLE( algID );
  AUX_VARIABLE( protons );
  AUX_VARIABLE( tofTracks );
}
 
} // namespace xAOD
