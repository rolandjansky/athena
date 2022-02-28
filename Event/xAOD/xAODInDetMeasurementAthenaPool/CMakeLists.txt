# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODInDetMeasurementAthenaPool )

# Component(s) in the package.
atlas_add_poolcnv_library( xAODInDetMeasurementAthenaPoolPoolCnv
   FILES xAODInDetMeasurement/PixelClusterContainer.h
   xAODInDetMeasurement/PixelClusterAuxContainer.h
   xAODInDetMeasurement/StripClusterContainer.h
   xAODInDetMeasurement/StripClusterAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::PixelClusterContainer xAOD::PixelClusterAuxContainer xAOD::StripClusterContainer xAOD::StripClusterAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODInDetMeasurement )
