# $Id: CMakeLists.txt 751303 2016-06-01 08:40:23Z krasznaa $
################################################################################
# Package: IOVDbSvc
################################################################################

# Declare the package name:
atlas_subdir( IOVDbSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CxxUtils
   Control/SGTools
   Control/StoreGate
   Database/APR/FileCatalog
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AthenaPOOL/PoolSvc
   Database/CoraCool
   Database/IOVDbDataModel
   Database/IOVDbMetaDataTools
   DetectorDescription/GeoModel/GeoModelInterfaces
   Event/EventInfo
   Event/EventInfoMgt )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( IOVDbSvcLib
   IOVDbSvc/*.h
   INTERFACE
   PUBLIC_HEADERS IOVDbSvc
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} GaudiKernel )

atlas_add_component( IOVDbSvc
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES}
   GaudiKernel AthenaBaseComps AthenaKernel CxxUtils SGTools StoreGateLib
   FileCatalog AthenaPoolUtilities CoraCool IOVDbDataModel EventInfo IOVDbSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
