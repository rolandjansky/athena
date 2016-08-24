# $Id: CMakeLists.txt 769747 2016-08-24 08:07:58Z will $
################################################################################
# Package: xAODEventInfoCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODEventInfoCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODCnvInterfaces
   PRIVATE
   InnerDetector/InDetConditions/InDetBeamSpotService
   LumiBlock/LumiBlockComps
   Control/AthenaBaseComps
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/EventInfo
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTruth
   GaudiKernel )

# GeoPrimitives is used explicitly, because InDetBeamSpotService uses it. And
# it doesn't provide an interface library that we could link against here.

# Component(s) in the package:
atlas_add_component( xAODEventInfoCnv
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES xAODCnvInterfaces LumiBlockCompsLib AthenaBaseComps
   AthenaKernel AthenaPoolUtilities EventInfo xAODEventInfo xAODTruth GaudiKernel
   GeoPrimitives )

# Install files from the package:
atlas_install_headers( xAODEventInfoCnv )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
