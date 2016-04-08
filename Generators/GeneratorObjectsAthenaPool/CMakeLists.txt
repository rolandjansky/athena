################################################################################
# Package: GeneratorObjectsAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( GeneratorObjectsAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Generators/GeneratorObjects
                          Generators/GeneratorObjectsTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( GeneratorObjectsAthenaPoolPoolCnv
                           src/*.cxx
                           FILES GeneratorObjects/HijingEventParams.h GeneratorObjects/McEventCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv )

# Install files from the package:
atlas_install_headers( GeneratorObjectsAthenaPool )
atlas_install_joboptions( share/*.py )

