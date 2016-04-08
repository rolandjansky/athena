################################################################################
# Package: TrigExMTHistNtup
################################################################################

# Declare the package name:
atlas_subdir( TrigExMTHistNtup )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          GaudiKernel
                          Trigger/TrigMonitoring/TrigMonitorBase )

# External dependencies:
find_package( AIDA )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigExMTHistNtup
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests EventInfo GaudiKernel TrigMonitorBaseLib )

# Install files from the package:
atlas_install_headers( TrigExMTHistNtup )
atlas_install_joboptions( share/*.py share/entries.C )

