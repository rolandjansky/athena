################################################################################
# Package: GeneratorObjectsTPCnv
################################################################################

# Declare the package name:
atlas_subdir( GeneratorObjectsTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/SGTools
   Database/AthenaPOOL/AthenaPoolCnvSvc
   GaudiKernel
   Generators/GeneratorObjects
   PRIVATE
   Control/AthAllocators
   Control/AthenaKernel
   Generators/GenInterfaces )

# External dependencies:
find_package( HepMC )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( GeneratorObjectsTPCnv
   GeneratorObjectsTPCnv/*.h src/*.cxx
   PUBLIC_HEADERS GeneratorObjectsTPCnv
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${HEPMC_LIBRARIES} SGTools AthenaPoolCnvSvcLib GaudiKernel
   GeneratorObjects
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel GenInterfacesLib )

atlas_add_dictionary( GeneratorObjectsTPCnvDict
   GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
   GeneratorObjectsTPCnv/selection.xml
   LINK_LIBRARIES GeneratorObjectsTPCnv )

atlas_add_dictionary( OLD_GeneratorObjectsTPCnvDict
   GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
   GeneratorObjectsTPCnv/OLD_selection.xml
   LINK_LIBRARIES GeneratorObjectsTPCnv )
