################################################################################
# Package: StlAthenaPoolCnv
################################################################################

# Declare the package name:
atlas_subdir( StlAthenaPoolCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_component( StlAthenaPoolCnv
                     src/*.cxx
                     LINK_LIBRARIES GaudiKernel SGTools AthenaPoolCnvSvcLib )

