################################################################################
# Package: GeneratorObjectsTPCnv
################################################################################

# Declare the package name:
atlas_subdir( GeneratorObjectsTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
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
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( GeneratorObjectsTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS GeneratorObjectsTPCnv
                         INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} SGTools AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects AthAllocators AthenaKernel )

atlas_add_dictionary( GeneratorObjectsTPCnvDict
                      GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
                      GeneratorObjectsTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} SGTools AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects AthAllocators AthenaKernel GeneratorObjectsTPCnv )

atlas_add_dictionary( OLD_GeneratorObjectsTPCnvDict
                      GeneratorObjectsTPCnv/GeneratorObjectsTPCnvDict.h
                      GeneratorObjectsTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} SGTools AthenaPoolCnvSvcLib GaudiKernel GeneratorObjects AthAllocators AthenaKernel GeneratorObjectsTPCnv )

