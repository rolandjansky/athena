################################################################################
# Package: TrigConfHLTData
################################################################################

# Declare the package name:
atlas_subdir( TrigConfHLTData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfL1Data )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrigConfHLTData
                   Root/*.cxx
                   PUBLIC_HEADERS TrigConfHLTData
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfL1Data )

#FIXME: Dictionary for AthAnalysisBase???
