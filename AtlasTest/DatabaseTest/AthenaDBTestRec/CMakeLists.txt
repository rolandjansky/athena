# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaDBTestRec )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

include_directories(src)

# Component(s) in the package:
atlas_add_library( AthenaDBTestRecLib
                   src/lib/*.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel StoreGateLib
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} )

atlas_add_component( AthenaDBTestRec
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaDBTestRecLib )

atlas_add_poolcnv_library( AthenaDBTestRecPoolCnv
                           src/*.cxx
                           FILES AthenaDBTestRec/TestCoolRecPoolData.h AthenaDBTestRec/TestCoolRecPoolDataColl.h
                           MULT_CHAN_TYPES TestCoolRecPoolDataColl
                           INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                           LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaDBTestRecLib AthenaPoolCnvSvcLib CoraCool PersistentDataModel )

atlas_add_dictionary( AthenaDBTestRecDataDict
                      AthenaDBTestRec/AthenaDBTestRecDataDict.h
                      AthenaDBTestRec/selection.xml
                      LINK_LIBRARIES AthenaDBTestRecLib )

atlas_add_executable( TestCoolRecWriter
                      src/app/*.cxx
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaDBTestRecLib CoraCool FileCatalog PersistencySvc PersistentDataModel StorageSvc )

# Install files from the package:
atlas_install_joboptions( share/*.py )

