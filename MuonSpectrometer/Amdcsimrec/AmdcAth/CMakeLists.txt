################################################################################
# Package: AmdcAth
################################################################################

# Declare the package name:
atlas_subdir( AmdcAth )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AmdcAthLib
                   src/*.cxx
                   PUBLIC_HEADERS AmdcAth
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel AmdcCore MuonAlignmentData StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AtlasDetDescr AmdcStand MuonReadoutGeometry PathResolver RDBAccessSvcLib )

atlas_add_component( AmdcAth
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel AmdcCore MuonAlignmentData StoreGateLib SGtests AtlasDetDescr AmdcStand MuonReadoutGeometry PathResolver AmdcAthLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

