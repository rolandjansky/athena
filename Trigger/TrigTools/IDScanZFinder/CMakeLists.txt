################################################################################
# Package: IDScanZFinder
################################################################################

# Declare the package name:
atlas_subdir( IDScanZFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigTools/TrigInDetToolInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( IDScanZFinder
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IRegionSelector GaudiKernel TrigInDetEvent )
