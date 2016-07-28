################################################################################
# Package: OffloadSvc
################################################################################

# Declare the package name:
atlas_subdir( OffloadSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/SGTools
                          Control/StoreGate
                          Event/EventInfo
                          GaudiKernel )

# External dependencies:
find_package( APE )
find_package( yampl )

# this line failed automatic conversion in cmt2cmake :
# action checkreq "echo 'skipping checkreq'"

# Component(s) in the package:
atlas_add_component( OffloadSvc
                     src/OffloadSvc.cxx
                     src/IOffloadSvc.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${YAMPL_INCLUDE_DIRS} ${APE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${YAMPL_LIBRARIES} ${APE_LIBRARIES} AthenaBaseComps SGTools StoreGateLib SGtests EventInfo GaudiKernel )

# Install files from the package:
atlas_install_headers( OffloadSvc )

