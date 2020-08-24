################################################################################
# Package: GeoModelSvc
################################################################################

# Declare the package name:
atlas_subdir( GeoModelSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( GeoModelSvc
                     src/GeoModelSvc.cxx
                     src/GeoDbTagSvc.cxx
                     src/RDBMaterialManager.cxx
                     src/components/GeoModelSvc_entries.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GeoModelUtilities EventInfo GaudiKernel RDBAccessSvcLib EventInfoMgtLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

