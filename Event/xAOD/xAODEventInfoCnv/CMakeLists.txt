# $Id: CMakeLists.txt 769747 2016-08-24 08:07:58Z will $
################################################################################
# Package: xAODEventInfoCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODEventInfoCnv )

# Optional dependencies:
set( extra_libs )
set( extra_private_deps )

if( NOT SIMULATIONBASE )
   set( extra_private_deps InnerDetector/InDetConditions/InDetBeamSpotService LumiBlock/LumiBlockData)
   set( extra_libs GeoPrimitives LumiBlockData )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODCnvInterfaces
   PRIVATE
   ${extra_private_deps}
   Control/AthenaBaseComps
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolUtilities
   InnerDetector/InDetConditions/BeamSpotConditionsData
   Event/EventInfo
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTruth
   GaudiKernel )

# GeoPrimitives is used explicitly, because InDetBeamSpotService uses it. And
# it doesn't provide an interface library that we could link against here.

# Component(s) in the package:
atlas_add_component( xAODEventInfoCnv
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES xAODCnvInterfaces  AthenaBaseComps
   AthenaKernel AthenaPoolUtilities EventInfo xAODEventInfo xAODTruth BeamSpotConditionsData GaudiKernel
   ${extra_libs} )

# Install files from the package:
atlas_install_headers( xAODEventInfoCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
