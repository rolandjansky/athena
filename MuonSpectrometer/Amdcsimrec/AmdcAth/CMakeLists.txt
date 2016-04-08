################################################################################
# Package: AmdcAth
################################################################################

# Declare the package name:
atlas_subdir( AmdcAth )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          MuonSpectrometer/Amdcsimrec/AmdcCore
                          MuonSpectrometer/MuonAlignment/MuonAlignmentData
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          Event/EventInfoMgt
                          MuonSpectrometer/Amdcsimrec/AmdcStand
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AmdcAthLib
                   src/*.cxx
                   PUBLIC_HEADERS AmdcAth
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel AmdcCore MuonAlignmentData StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AtlasDetDescr EventInfo AmdcStand MuonReadoutGeometry PathResolver )

atlas_add_component( AmdcAth
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel AmdcCore MuonAlignmentData StoreGateLib SGtests AtlasDetDescr EventInfo AmdcStand MuonReadoutGeometry PathResolver AmdcAthLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

