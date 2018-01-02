################################################################################
# Package: RegistrationServices
################################################################################

# Declare the package name:
atlas_subdir( RegistrationServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/APR/CollectionBase
                          Database/APR/POOLCore
                          Database/APR/FileCatalog
                          Database/AthenaPOOL/AthenaPoolKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/DBDataModel
                          Database/AthenaPOOL/PoolSvc
                          Database/IOVDbSvc
                          Database/PersistentDataModel )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( RegistrationServices
                     src/RegistrationStream.cxx
                     src/RegistrationStreamDefMeta.cxx
                     src/RegistrationStreamLCGTool.cxx
                     src/RegistrationStreamTagTool.cxx
                     src/InputCollectionMetadataCopy.cxx
                     src/IOVRegistrationSvc.cxx
                     src/OutputConditionsAlg.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests CollectionBase POOLCore FileCatalog AthenaPoolUtilities DBDataModel PersistentDataModel )

# Install files from the package:
atlas_install_headers( RegistrationServices )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

