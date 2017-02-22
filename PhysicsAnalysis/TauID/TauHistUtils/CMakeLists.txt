################################################################################
# Package: TauHistUtils
################################################################################

# Declare the package name:
atlas_subdir( TauHistUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTau 
			  TrkValHistUtils)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TauHistUtils
                   src/*.cxx
		   PUBLIC_HEADERS TrkValHistUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODPFlow xAODTau TrkValHistUtils)

