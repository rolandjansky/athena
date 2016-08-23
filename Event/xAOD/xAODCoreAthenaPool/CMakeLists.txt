# $Id: CMakeLists.txt 769660 2016-08-23 15:51:19Z krasznaa $
################################################################################
# Package: xAODCoreAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODCoreAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthContainers
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODCore )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCoreAthenaPoolPoolCnv
   src/*.cxx
   FILES xAODCore/AuxInfoBase.h xAODCore/AuxContainerBase.h
   xAODCore/ShallowAuxContainer.h xAODCore/ShallowAuxInfo.h
   TYPES_WITH_NAMESPACE xAOD::AuxInfoBase xAOD::AuxContainerBase
   xAOD::ShallowAuxContainer xAOD::ShallowAuxInfo
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODCore )
