/*   
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/ 

// $Id$ 
// Gaudi/Athena include(s): 
#include "AthenaKernel/TPCnvFactory.h" 
// EDM include(s): 
#include "xAODPFlow/TrackCaloClusterAuxContainer.h" 
#include "xAODPFlow/versions/TrackCaloClusterAuxContainer_v1.h" 
// Local include(s): 
#include "xAODTrackCaloClusterAuxContainerCnv_v1.h" 

// Declare the T/P converter(s): 
DECLARE_TPCNV_FACTORY( xAODTrackCaloClusterAuxContainerCnv_v1,
                       xAOD::TrackCaloClusterAuxContainer,
                       xAOD::TrackCaloClusterAuxContainer_v1,   
                       Athena::TPCnvVers::Old ) 