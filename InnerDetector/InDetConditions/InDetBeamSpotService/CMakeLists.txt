# $Id: CMakeLists.txt 773822 2016-09-19 10:04:57Z krasznaa $
################################################################################
# Package: InDetBeamSpotService
################################################################################

# Declare the package name:
atlas_subdir( InDetBeamSpotService )

# Possible extra dependencies:
set( extra_dep )
set( extra_lib )
if( NOT SIMULATIONBASE )
   set( extra_dep Tracking/TrkEvent/VxVertex )
   set( extra_lib VxVertex )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/GeoPrimitives
   GaudiKernel
   ${extra_dep}
   PRIVATE
   Control/AthenaBaseComps
   Control/CxxUtils
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/EventPrimitives )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( InDetBeamSpotServiceLib
   InDetBeamSpotService/*.h
   INTERFACE
   PUBLIC_HEADERS InDetBeamSpotService
   LINK_LIBRARIES GaudiKernel GeoPrimitives ${extra_lib} )

atlas_add_component( InDetBeamSpotService
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} GeoPrimitives GaudiKernel AthenaBaseComps CxxUtils
   StoreGateLib AthenaPoolUtilities EventPrimitives InDetBeamSpotServiceLib
   ${extra_lib} )

atlas_add_dictionary( InDetBeamSpotServiceDict
   InDetBeamSpotService/IBeamCondSvc.h
   InDetBeamSpotService/selection.xml
   LINK_LIBRARIES InDetBeamSpotServiceLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
