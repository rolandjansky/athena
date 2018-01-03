################################################################################
# Package: AthenaDBTestRec
################################################################################

# Declare the package name:
atlas_subdir( AthenaDBTestRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/CoraCool
                          Database/PersistentDataModel
                          Event/EventInfo )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

include_directories(src)

# Component(s) in the package:
atlas_add_library( AthenaDBTestRecLib
                   src/lib/*.cxx
                   PUBLIC_HEADERS AthenaDBTestRec
                   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CollectionBase CollectionUtilities PersistencySvc StorageSvc CoraCool PersistentDataModel EventInfo )

atlas_add_component( AthenaDBTestRec
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities GaudiKernel CollectionBase CollectionUtilities PersistencySvc StorageSvc AthenaPoolCnvSvcLib CoraCool PersistentDataModel EventInfo AthenaDBTestRecLib )

atlas_add_poolcnv_library( AthenaDBTestRecPoolCnv
                           src/*.cxx
                           FILES AthenaDBTestRec/TestCoolRecPoolData.h AthenaDBTestRec/TestCoolRecPoolDataColl.h
                           MULT_CHAN_TYPES TestCoolRecPoolDataColl
                           INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                           LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities GaudiKernel CollectionBase CollectionUtilities PersistencySvc StorageSvc AthenaPoolCnvSvcLib CoraCool PersistentDataModel EventInfo AthenaDBTestRecLib )

atlas_add_dictionary( AthenaDBTestRecDataDict
                      AthenaDBTestRec/AthenaDBTestRecDataDict.h
                      AthenaDBTestRec/selection.xml
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities GaudiKernel CollectionBase CollectionUtilities PersistencySvc StorageSvc AthenaPoolCnvSvcLib CoraCool PersistentDataModel EventInfo AthenaDBTestRecLib )

atlas_add_executable( TestCoolRecWriter
                      src/app/*.cxx
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities GaudiKernel CollectionBase CollectionUtilities PersistencySvc StorageSvc AthenaPoolCnvSvcLib CoraCool PersistentDataModel EventInfo AthenaDBTestRecLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

