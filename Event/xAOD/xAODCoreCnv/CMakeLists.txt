################################################################################
# Package: xAODCoreCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODCoreCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthContainers
   Control/AthContainersInterfaces
   Control/AthLinks
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/SGTools
   Event/xAOD/xAODCore
   GaudiKernel )

# Make use of ROOT:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( xAODCoreCnv
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaBaseComps
   AthenaKernel SGTools xAODCore GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )
