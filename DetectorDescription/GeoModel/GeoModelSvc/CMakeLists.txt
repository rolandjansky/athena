################################################################################
# Package: GeoModelSvc
################################################################################

# Declare the package name:
atlas_subdir( GeoModelSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          Event/EventInfo
                          Event/EventInfoMgt
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )


find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( GeoModelSvc
                     src/GeoModelSvc.cxx
                     src/GeoDbTagSvc.cxx
                     src/RDBMaterialManager.cxx
                     src/components/GeoModelSvc_entries.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GeoModelUtilities EventInfo GaudiKernel RDBAccessSvcLib EventInfoMgtLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

