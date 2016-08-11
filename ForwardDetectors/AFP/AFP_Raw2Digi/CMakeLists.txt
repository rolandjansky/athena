################################################################################
# Package: AFP_Raw2Digi
################################################################################

# Declare the package name:
atlas_subdir( AFP_Raw2Digi )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          ForwardDetectors/AFP/AFP_DigiEv
                          ForwardDetectors/AFP/AFP_RawEv
                          GaudiKernel
			  PRIVATE
			  Event/xAOD/xAODForward
			  )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( AFP_Raw2Digi
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODForward AthenaBaseComps AFP_RawEv)
 
# Install files from the package:
atlas_install_headers( AFP_Raw2Digi )

