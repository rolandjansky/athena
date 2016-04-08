################################################################################
# Package: TrkDetDescrAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrTPCnv
                          Tracking/TrkDetDescr/TrkGeometry )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkDetDescrAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkGeometry/MaterialStepCollection.h TrkGeometry/LayerMaterialMap.h TrkGeometry/ElementTable.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel TrkDetDescrTPCnv TrkGeometry )

