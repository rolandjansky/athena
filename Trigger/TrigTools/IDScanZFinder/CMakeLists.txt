################################################################################
# Package: IDScanZFinder
################################################################################

# Declare the package name:
atlas_subdir( IDScanZFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( IDScanZFinderLib
                   src/*.cxx
                   PUBLIC_HEADERS IDScanZFinder
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IRegionSelector GaudiKernel TrigInDetEvent )

atlas_add_component( IDScanZFinder
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IRegionSelector GaudiKernel TrigInDetEvent IDScanZFinderLib )

