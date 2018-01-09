################################################################################
# Package: HepMCWeightSvc
################################################################################

# Declare the package name:
atlas_subdir( HepMCWeightSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   Control/AthenaBaseComps
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/IOVDbDataModel
   Database/IOVDbMetaDataTools
   Event/EventInfo
   Generators/GenInterfaces )

find_package( Boost COMPONENTS algorithm )
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( HepMCWeightSvc
   src/*.cxx src/components/*.cxx
   NO_PUBLIC_HEADERS
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel AthenaBaseComps
   AthenaPoolUtilities IOVDbDataModel EventInfo )

# Install files from the package:
atlas_install_headers( HepMCWeightSvc )
