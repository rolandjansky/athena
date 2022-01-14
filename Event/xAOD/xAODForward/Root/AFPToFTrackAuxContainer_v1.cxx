/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPToFTrackAuxContainer_v1.cxx
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-11-11
 * 
 * @brief  Implementation file for class xAOD::AFPToFTrackAuxContainer_v1
 * 
 */


// Local include(s):
#include "xAODForward/versions/AFPToFTrackAuxContainer_v1.h"
 
namespace xAOD {
 
  AFPToFTrackAuxContainer_v1::AFPToFTrackAuxContainer_v1()
    : AuxContainerBase() {
    
  AUX_VARIABLE( stationID );
  AUX_VARIABLE( TrainID );
  AUX_VARIABLE( TrainTime );
  AUX_VARIABLE( TrainSize );
  AUX_VARIABLE( TrainNSat );
  AUX_VARIABLE( algID );
  AUX_VARIABLE( bars );
}
 
} // namespace xAOD
