################################################################################
# Package: DFStreamEventSelector
################################################################################
# macro(dump_var arg)
#   message(STATUS "SAMI ${arg}=${${arg}}")
# endmacro(dump_var)
# Declare the package name:
atlas_subdir( DFStreamEventSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Event/ByteStreamCnvSvcBase
   # Event/ByteStreamData
   GaudiKernel
   PRIVATE
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   Event/EventInfo
   Event/xAOD/xAODEventInfo
   )

# External dependencies:
find_package( tdaq-common COMPONENTS hltinterface )
# Libraries in the package:
atlas_add_library( DFStreamEventSelectorLib
   src/*.h src/*.cxx
`   PUBLIC_HEADERS DFStreamEventSelector
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} 
   LINK_LIBRARIES AthenaBaseComps GaudiKernel
   StoreGateLib rt ${TDAQ-COMMON_hltinterface_LIBRARY}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} 
   AthenaKernel EventInfo xAODEventInfo)

atlas_add_component( DFStreamEventSelector
   src/components/*.cxx
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} # need to include since IROBDataProviderSvc brings in ers and eformat
   LINK_LIBRARIES DFStreamEventSelectorLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
