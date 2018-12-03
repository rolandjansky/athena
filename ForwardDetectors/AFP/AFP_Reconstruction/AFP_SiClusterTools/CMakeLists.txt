################################################################################
# Package: AFP_SiClusterTools
################################################################################

# Declare the package name:
atlas_subdir( AFP_SiClusterTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel 
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
			  Event/xAOD/xAODForward
			  PRIVATE
			  Control/AthLinks
			  )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Math GenVector )

atlas_add_component( AFP_SiClusterTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities GaudiKernel GeneratorObjects xAODForward AthLinks)


# Install files from the package:
atlas_install_headers( AFP_SiClusterTools )
atlas_install_joboptions( share/*.py )


